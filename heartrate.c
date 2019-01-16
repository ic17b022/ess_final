/*
 * heartrate.c
 *
 *  Created on: Dec 27, 2018
 *      Author: manuel
 */
#include "local_inc/common.h"
#include "local_inc/heartrate.h"
#include "local_inc/broker.h"

#include <ti/drivers/I2C.h>

#define SLAVEADDR_READ 0b10101111
#define SLAVEADDR_WRITE 0b10101110
#define SLAVEADDR 0b1010111 //tiva ware appends the one and zero on its own?

static void heartrate_run();
static void init();
static void readFIFOData();
static void I2C_write(uint8_t reg, uint8_t value);
static uint8_t I2C_read(uint8_t reg);
static void I2C_readFIFO(uint8_t* array);

I2C_Handle handle;

void create_heartrate_task(int prio)
{
    Task_Params params;
    Task_Handle taskHeartrate;
    Error_Block eb;
    /* Create OLED startup task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&params);
    params.stackSize = 512; /* stack in bytes */
    params.priority = prio; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskHeartrate = Task_create(heartrate_run, &params, &eb);
    if (taskHeartrate == NULL)
    {
        System_abort("TaskLed create failed");
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

    //since the power on interrupt is a lie we initialize here.
    init();

    while (1)       //GPIO INt pin suchen anschauen implementieren
    {
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

static void readFIFOData(){
    uint8_t read_ptr;
    uint8_t write_ptr;
    short samples;
    uint8_t buffer[4];
    int i;
    int test = 0;

    read_ptr = I2C_read(0x04);
    write_ptr = I2C_read(0x02);

    samples = write_ptr - read_ptr;

    if(samples < 0)
        samples = 0x0f - read_ptr + write_ptr;
    else if (samples == 0)  //when the buffer is full read and write pointer point to the same adress and since we got an interrupt there has to be data
        samples = 16;

    for (i = 0; i < samples; i++){
        I2C_readFIFO(buffer);
        test = buffer[0];
        test = (test << 8) + buffer[1];
        System_printf("%u \n",test);
        System_flush();
        //send data to broker
    }
}

static void I2C_write(uint8_t reg, uint8_t value){
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

static uint8_t I2C_read(uint8_t reg){
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

static void I2C_readFIFO(uint8_t* array){
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
