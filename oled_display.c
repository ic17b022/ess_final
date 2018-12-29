/*
 * oled_display.c
 *
 *  Created on: Dec 27, 2018
 *      Author: marcaurel
 */

#include "local_inc/common.h"
#include "local_inc/oled_display.h"
#include <ti/drivers/SPI.h>

SPI_Handle handle;

static void commandSPI(uint8_t reg, uint8_t value);
static void wait_ms(uint32_t delay);

static void wait_ms(uint32_t delay) {
    // wait for delay in ms Frecquency 120MHz
    SysCtlDelay(120000000 / 1000 * delay);
}

void initSPI(void) {

    SPI_Params params;

    PinoutSet();

    SETBIT(LED01,1);

    SPI_Params_init(&params);
    params.transferMode = SPI_MODE_BLOCKING;    // enable blocking mode
    params.transferCallbackFxn = NULL;          // Blocking mode, no Call Back function
    params.mode = SPI_SLAVE;                   // SPI is master
    params.frameFormat = SPI_POL0_PHA0;         // polarity 0, rising 1 edge
    params.bitRate = 10000;                     // bitrate 10 kHz
    params.dataSize = 8;                        // datasize is 8 Bit

    handle = SPI_open(Board_SPI0, &params);
    if (!handle) {
        System_printf("SPI2 did not open.");
        System_flush();
    } else {
        System_printf("SPI2 has handle at address: %p.\n", handle);
        System_flush();
    }

}

void OLED_power_on_short(void) {
    // (1) Power ON VDD, VDDIO.
    // (2) After VDD, VDDIO become stable and wait for 100ms(t1),
    //      set RSTB pin LOW (logic low) for at least 1ms (t2)
    //      and then HIGH(logic high).
    // (3) After set RSTB pin HIGH (logic high), wait for at least 50ms (t3). Then Power ON VCC_C
    // (4) After VCC_C become stable, set register 0x02 with value 0x01
    //      for display ON.
    //      Data/Scan will be ON after 200ms (tAF).

    wait_ms(100);
    SETBIT(OLED_RST, 0);
    wait_ms(5);
    SETBIT(OLED_RST, 1);
    wait_ms(50);
    commandSPI(OLED_DISPLAY_ON_OFF, 0x01);
    // commandSPI(0x00,0x00);
}

// Send command to OLED
void commandSPI(uint8_t reg, uint8_t value) {
    uint8_t txBuf[1];
    bool success;

    txBuf[0] = reg;
    SPI_Transaction spiTransaction;
    spiTransaction.count = 8;       // Datasize 8 Bits
    spiTransaction.txBuf = txBuf;
    spiTransaction.rxBuf = NULL;

    SETBIT(OLED_RW, 0); // Set the peripheral to write -> mcu write to periph

    // Write to register
    SETBIT(OLED_CS, 0);
    SETBIT(OLED_DC, 0);
    success = SPI_transfer(handle, &spiTransaction);
    SETBIT(OLED_CS, 1);
    if (!success) {
        System_printf("Register transaction failed");
        System_flush();
    }

    wait_ms(1);
    txBuf[0] = value;
    // Write into the register
    SETBIT(OLED_CS, 0);
    SETBIT(OLED_DC, 1);
    success = SPI_transfer(handle, &spiTransaction);
    SETBIT(OLED_CS, 1);
    if (!success) {
        System_printf("Value transaction failed");
        System_flush();
    }
}



