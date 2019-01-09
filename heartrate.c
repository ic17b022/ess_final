/*
 * heartrate.c
 *
 *  Created on: Dec 27, 2018
 *      Author: manuel
 */
#include "local_inc/common.h"
#include "local_inc/heartrate.h"

#include <ti/drivers/I2C.h>

#define SLAVEADDR_READ 0b10101111
#define SLAVEADDR_WRITE 0b10101110
#define SLAVEADDR 0b1010111 //tiva ware appends the one and zero on its own?

static void I2C();

void create_heartrate_task(int prio) {
    Task_Params params;
    Task_Handle taskHeartrate;
    Error_Block eb;
    /* Create OLED startup task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&params);
    params.stackSize = 512; /* stack in bytes */
    params.priority = prio; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskHeartrate = Task_create(I2C, &params, &eb);
    if (taskHeartrate == NULL) {
        System_abort("TaskLed create failed");
    }
}

static void I2C(){
    I2C_Handle      handle;
    I2C_Params      i2cparams;
    I2C_Transaction i2c;
    uint8_t readBuffer[4];     //I2C always reads 4 bytes? (or at least max 4 bytes?)
    uint8_t writeBuffer[4];    //same as above? maybe less is ok? revisit this

    memset(readBuffer,0, sizeof(readBuffer));

    I2C_Params_init(&i2cparams);
    i2cparams.bitRate = I2C_400kHz;
    i2cparams.transferMode = I2C_MODE_BLOCKING;/*important if you call I2C_transfer in Task context*/
    handle = I2C_open(EK_TM4C1294XL_I2C8, &i2cparams);
    if (handle == NULL) {
        System_abort("I2C was not opened");
    }

//read interrupt status register
//    i2c.slaveAddress = SLAVEADDR;
//    i2c.readCount = 0;
//    i2c.readBuf = NULL;
//    writeBuffer[0] = 0x00; /*address of interrupt status register*/
//    i2c.writeCount = 1;
//    i2c.writeBuf = writeBuffer;
//
//    if (!I2C_transfer(handle, &i2c)) {
//        System_abort("Bad I2C transfer!");
//    }
//
//    i2c.slaveAddress = SLAVEADDR;
//    i2c.readCount = 1;
//    i2c.readBuf = (uint8_t*)&readBuffer[0];
//    i2c.writeCount = 0;
//    i2c.writeBuf = NULL;
//
//    if (!I2C_transfer(handle, &i2c)) {
//        System_abort("Bad I2C transfer!");
//    }

    //transaction 1 enable interrupts
    i2c.slaveAddress = SLAVEADDR;
    i2c.readCount = 0;
    i2c.readBuf = NULL;
    writeBuffer[0] = 0x01; /*address of interrupt enable register*/
    writeBuffer[1] = 0b11110000; /*enable all interrupts*/
    i2c.writeCount = 2;
    i2c.writeBuf = &writeBuffer[0];

    if (!I2C_transfer(handle, &i2c)) {
        System_abort("Bad I2C transfer!");
    }

    //transaction 2 read interrupt register
    i2c.slaveAddress = SLAVEADDR;
    i2c.readCount = 1;
    i2c.readBuf = &readBuffer[0];
    writeBuffer[0] = 0x01; /*address of interrupt enable register*/
    i2c.writeCount = 1;
    i2c.writeBuf = &writeBuffer[0];

    if (!I2C_transfer(handle, &i2c)) {
        System_abort("Bad I2C transfer!");
    }

    //powerup generates an interrupt that needs(?) to be cleared?

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
