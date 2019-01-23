/*
 * heartrate.c
 *
 *  Created on: Dec 27, 2018
 *      Author: manuel
 */
#include "local_inc/common.h"
#include "local_inc/heartrate.h"

#include <ti/sysbios/hal/Hwi.h>
#include <inc/hw_ints.h>
#include <driverlib/sysctl.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>

#define SLAVEADDR_READ 0b10101111
#define SLAVEADDR_WRITE 0b10101110
#define SLAVEADDR 0b1010111 //tiva ware appends the one and zero on its own?
#define FREQUENCY 5000  //in milliseconds (although documentation says ticks)
#define SENSOR_DATA_SIZE 300

static void heartrate_run();
static void init();
static void readFIFOData();
static void I2C_write(uint8_t reg, uint8_t value);
static uint8_t I2C_read(uint8_t reg);
static void I2C_readFIFO(uint8_t* array);
static int comparison(const void* a, const void* b);
static void initInterrupt();
static void interruptFunction(unsigned int index);

I2C_Handle handle;

unsigned short sensor_data[SENSOR_DATA_SIZE];
unsigned short data_count;
bool inter = false;

void create_heartrate_tasks(int prio)
{
    Task_Params params;
    Task_Handle taskHeartrate;
    Error_Block eb;
    /* Create heartrate task */
    Error_init(&eb);
    Task_Params_init(&params);
    params.stackSize = 512; /* stack in bytes */
    params.priority = prio; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    params.instance->name = "heartrate";

    taskHeartrate = Task_create(heartrate_run, &params, &eb);
    if (taskHeartrate == NULL)
        System_abort("TaskLed create failed");
    else
    {
        System_printf("Created heartrate main Task\n");
        System_flush();
    }

    //create heartrate clock task
    Clock_Params clockParams;
    Clock_Handle myClock;

    Error_init(&eb);
    Clock_Params_init(&clockParams);
    clockParams.period = FREQUENCY;
    clockParams.startFlag = TRUE;
    clockParams.arg = 0; // Don't have anything to pass to the function
    myClock = Clock_create(clockFunction, FREQUENCY, &clockParams, &eb); //Frequency is passed into create_clock to configure ticks waited until first invoke, and clockparams.period for every invoke after that
    if (myClock == NULL)
        System_abort("Clock create failed");
    else
    {
        System_printf("Created Clock Task\n");
        System_flush();
    }
}

//I2CIntRegister(SLAVEADDR, interruptFunction);
static void heartrate_run()
{
    I2C_Params i2cparams;
    uint8_t readBuffer = 0; //I2C always reads 4 bytes? (or at least max 4 bytes?)

    I2C_Params_init(&i2cparams);
    i2cparams.bitRate = I2C_400kHz;
    i2cparams.transferMode = I2C_MODE_BLOCKING;/*important if you call I2C_transfer in Task context*/
    handle = I2C_open(EK_TM4C1294XL_I2C8, &i2cparams);
    if (handle == NULL)
    {
        System_abort("I2C was not opened");
    }

    initInterrupt();
    //since the power on interrupt is a lie we initialize here.
    init();

    while (1)       //GPIO INt pin suchen anschauen implementieren
    {
//        if (inter)
//        {
//            System_printf("Interrupt received! \n");
//            System_flush();
//            inter = false;
//        }

        //read interrupt register
        readBuffer = I2C_read(0x00);

        switch (readBuffer)
        {
        case 0b00000001: //Power On -> init; Or not. Interrupt is a lie.
            System_printf("Halleluja, der Messiah hat vorbeigeschaut!");
            System_flush();
            break;

        case 0b00100000: //heartrate Data ready -> go fetch
            readFIFOData();
            break;
        case 0b00000000:
            //no interrupts, nothing to do
            break;
        default:
            System_printf("funky interrupts %u", readBuffer);
            System_flush();
            break;
        }

    }

    //powerup generates an interrupt that needs(?) to be cleared?

    //heartrate only mode: red LED inactive, only IR LED used

    //set mode (enable SpO2 and heartrate?) 010 = heartrate only, 011 = SpO2
    //set sample rate. 000 = 50 Hz (lowest)
    //set SpO2 resolution. 13 -16 bits. Takes 2 bytes either way -> 16bit? 11
    //set LED current. No idea. Don't care about the mA, no idea what difference it makes for the output
    //pp 19 got some mumbo jumbo about resistance and peak powersupply. maybe just set it to 50Hz sample rate and 200 pulse width (13bit precision)
    //to avoid the hassle? (both heartrate and SpO2)

    //3 transactions per read? P. 14 ff
    //transfer() seems to be the way to actually transmit stuff
    //open() and close() are not per transmission, but per peripheral ie. do that at the beginning and during shutdown (? == never?)

    //reading temp is a bit of a wtf? 0 and 1° get the same hex value? pp 18
    //register 0x16 for full degrees, 0x17 for fractions
    //also full degrees can be negative, fractions are always positive and need to be ADDED not appended

    I2C_close(handle);
}

static void init()
{
    /* prepare sensor data buffer */
    memset(sensor_data, 0, sizeof(*sensor_data) * SENSOR_DATA_SIZE);
    data_count = 0;

    //set mode to 010 in mode configuration register for heartrate only
    I2C_write(0x06, 0b00000010);

    //set sample rate to 000 in SpO2 config register (apparently also configures the IR LED for heartrate)
    //for 50 samples per second and pulse width 11 for 16 bit resolution (lowest res is 13, so its 2 bytes either way)
    I2C_write(0x07, 0b00000011);

    //set IR LED current to 1111 in LED configuration register. This means 50 mA for maximum power. Mostly because we can.
    I2C_write(0x09, 0b00001111);

    //initialise FIFO to known (empty state)
    //set FIFO write pointer to zero
    I2C_write(0x02, 0x00);
    //set FIFO overflow counter to zero
    I2C_write(0x03, 0x00);
    //set FIFO read pointer to zero
    I2C_write(0x04, 0x00);

    //enable heartrate interrupt in interrupt enable register
    I2C_write(0x01, 0b00100000);
}

static void readFIFOData()
{
    uint8_t read_ptr;
    uint8_t write_ptr;
    short samples;
    uint8_t buffer[4];
    int i;
    unsigned short temp;

    read_ptr = I2C_read(0x04);
    write_ptr = I2C_read(0x02);

    samples = write_ptr - read_ptr;

    if (samples < 0)
        samples = 0x0f - read_ptr + write_ptr;
    else if (samples == 0) //when the buffer is full read and write pointer point to the same address and since we got an interrupt there has to be data
        samples = 16;

    for (i = 0; i < samples; i++)
    {
        I2C_readFIFO(buffer);
        temp = (buffer[0] << 8) + buffer[1];
        if (temp > 30000 && i < SENSOR_DATA_SIZE)
        { //if there are meaningful values and we still have space in the array
            sensor_data[data_count] = temp;
            data_count++;
        }
    }

    /* einzelne Werte mit value/max * 96 auf eine Kurve mit höhe 96 pixel bringen (und max 96 davon liefern wegen breite)? */
}

static void I2C_write(uint8_t reg, uint8_t value)
{
    I2C_Transaction i2c;
    uint8_t writeBuffer[2];

    //enable heartrate interrupt
    i2c.slaveAddress = SLAVEADDR;
    i2c.readCount = 0;
    i2c.readBuf = NULL;
    writeBuffer[0] = reg; /*address of interrupt enable register*/
    writeBuffer[1] = value; /*enable heartrate interrupt*/
    i2c.writeCount = 2;
    i2c.writeBuf = &writeBuffer[0];

    if (!I2C_transfer(handle, &i2c))
    {
        System_abort("Bad I2C transfer!");
    }
}

static uint8_t I2C_read(uint8_t reg)
{
    I2C_Transaction i2c;
    uint8_t readBuffer = 0;

    i2c.slaveAddress = SLAVEADDR;
    i2c.readCount = 1;
    i2c.readBuf = &readBuffer;
    i2c.writeCount = 1;
    i2c.writeBuf = &reg;

    if (!I2C_transfer(handle, &i2c))
    {
        System_abort("Bad I2C transfer!");
    }

    return readBuffer;
}

static void I2C_readFIFO(uint8_t* array)
{
    I2C_Transaction i2c;
    uint8_t reg = 0x05;

    i2c.slaveAddress = SLAVEADDR;
    i2c.readCount = 4;
    i2c.readBuf = array;
    i2c.writeCount = 1;
    i2c.writeBuf = &reg;

    if (!I2C_transfer(handle, &i2c))
    {
        System_abort("Bad I2C transfer!");
    }
}

void clockFunction()
{
    unsigned short median;
    unsigned short temp_data[SENSOR_DATA_SIZE];
    memcpy(temp_data, sensor_data, sizeof(*sensor_data) * data_count);
    int i;
    uint8_t heartrate = 0;

    qsort(temp_data, data_count, sizeof(*sensor_data), comparison);
    median = temp_data[data_count / 2]; //yes, dividing data_count by two rounds down in case of uneven amounts of data. I don't care.

    for (i = 0; i < data_count; i = i + 2)
    {
        if (sensor_data[i] <= median && sensor_data[i + 1] >= median)
            heartrate++;
    }

    memset(sensor_data, 0, sizeof(*sensor_data) * SENSOR_DATA_SIZE);
    data_count = 0;

    heartrate = heartrate * 12; //extrapolate from 5 seconds to 1 Minute

    //send data to broker
    Mailbox_post(heartrateMailbox, &heartrate, BIOS_NO_WAIT);
}

static int comparison(const void* a, const void* b)
{
    return (*(unsigned short*) a - *(unsigned short*) b);
}

static void initInterrupt()
{
    GPIO_setCallback(2, interruptFunction);
    GPIO_enableInt(2);
}

static void interruptFunction(unsigned int index)
{
    GPIO_clearInt(2);
    inter = true;
}
