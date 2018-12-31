/*
 * oled_display.c
 *
 *  Created on: Dec 27, 2018
 *      Author: marcaurel
 */

#include "local_inc/oled_display.h"

static SPI_Handle handle;
static uint32_t ui32SysClkFreq;

/* ******* CONSTANTS  ********** */
/* Constant Addresses of PINS, muxing with typedefs */
static const PinAddress OLED_RST = {OLED_RST_PORT, OLED_RST_PIN};
static const PinAddress OLED_DC = {OLED_DC_PORT, OLED_DC_PIN};
static const PinAddress OLED_CS = {OLED_CS_PORT, OLED_CS_PIN};
static const PinAddress OLED_RW = {OLED_RW_PORT, OLED_RW_PIN};

static const PinAddress LED01  = {LED_01_PORT, LED_01_PIN};
static const PinAddress LED02  = {LED_02_PORT, LED_02_PIN};
static const PinAddress LED03  = {LED_03_PORT, LED_03_PIN};
static const PinAddress LED04  = {LED_04_PORT, LED_04_PIN};

static void commandSPI(uint8_t reg, uint8_t value);
//static void transferSPI(uint8_t reg, uint8_t value);
static void wait_ms(uint32_t delay);
static void OLED_power_on(void);
static void OLED_power_off(void);
static void wait_ms(uint32_t delay) {
    // wait for delay in ms Frequency 120MHz
    SysCtlDelay(ui32SysClkFreq / 3000 * delay);
}

/* Common */
void Pinmux (void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    // LED 03 + LED04
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);    // LED 01 + LED02
    // Confgure the GPIO Pins for the onboard LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    /* Control pins for OLED Boosterpack 1*/
#if SSIM_2
    // Init the 4 necessary GPIO pins to drive the SSI2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    // AN Booster   ->  D3 OLED (PE4)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);    // RST Booster  ->  RST OLED (PC7)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);    // CS Booster   ->  CSB OLED (PH2)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);    // PWM Booster  ->  A0 OLED (PM3)

    // Configure the necessary GPIO Pin to drive the SPI2
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_3);
#endif

    // Configure the necessary GPIO Pin to drive the SPI3
#if SSIM_3

    // Init the 4 necessary GPIO pins to drive the SSI3
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);    // AN Booster   ->  D3 OLED (PD2)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);    // RST Booster  ->  RST OLED (PP4) + CS Booster   ->  CSB OLED (PP5)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);    // PWM Booster  ->  A0 OLED (PM7)

    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_7);
#endif

    SSIClockSourceSet(OLED_SSI_BASE, SSI_CLOCK_SYSTEM);
    SSIConfigSetExpClk(OLED_SSI_BASE, ui32SysClkFreq, OLED_SSI_MODE, SSI_MODE_MASTER, SSI_FREQUENCY, 8);
    SSIAdvModeSet(OLED_SSI_BASE, SSI_ADV_MODE_LEGACY);      // Operation in
    SSIEnable(OLED_SSI_BASE);
}

extern void initSPI(uint32_t systemFrequency) {
    ui32SysClkFreq = systemFrequency;
    Pinmux();                       // Do all necessary pin muxing

    SPI_Params params;
    SETBIT(LED01,1);
    SPI_Params_init(&params);
    params.transferMode = SPI_MODE_BLOCKING;    // enable blocking mode
    params.transferCallbackFxn = NULL;          // Blocking mode, no Call Back function
    params.mode = SPI_MASTER;                   // SPI is master
    params.frameFormat = OLED_SSI_MODE;         // polarity 1, rising 2 edge 1:1
    params.bitRate = SSI_FREQUENCY;             // bitrate 5 MHz
    params.dataSize = 8;                        // datasize is 8 Bit

    handle = SPI_open(Board_SPI0, &params);
    if (!handle) {
        System_printf("SPI2 did not open.");
        System_flush();
    } else {
        System_printf("SPI2 has handle at address: %p.\n", handle);
        System_flush();
    }
    SETBIT(OLED_CS, 1);             // Set Chip select to high, not seleted
}
extern void setup_power_on_task(xdc_String name) {
    Task_Params taskLedParams;
    Task_Handle taskLed;
    Error_Block eb;
    /* Create OLED startup task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.instance->name = name;
    taskLedParams.stackSize = 1024; /* stack in bytes */
    taskLedParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskLed = Task_create((Task_FuncPtr)OLED_power_on, &taskLedParams, &eb);
    if (taskLed == NULL) {
        System_abort("TaskLed create failed");
    }
}

static void OLED_power_on(void) {
    // wait for 100ms
    wait_ms(100);
    // Set RST to LOW
    SETBIT(OLED_RST, 0);
    wait_ms(10); // wait 10 ms
    // Set RST to HIGH
    SETBIT(OLED_RST, 1);
    SETBIT(LED02, 1);
    wait_ms(10); // wait 10 ms
    /* soft Reset */
    commandSPI(OLED_SOFT_RESET, 0x00);
    /* Standby ON/OFF */
    commandSPI(OLED_STANDBY_ON_OFF, 0x01);  // Standby ON
    wait_ms(5);           // wait 5 ms
    commandSPI(OLED_STANDBY_ON_OFF, 0x00);  // Standby OFF
    wait_ms(5);           // wait 5 ms
    /* Set Oscillator operation */
    commandSPI(OLED_ANALOG_CONTROL,0x00);          // using external resistor and internal OSC
    /* Set frame rate */
    commandSPI(OLED_OSC_ADJUST,0x03);              // frame rate : 95Hz
    /* Set active display area of panel */
    commandSPI(OLED_DISPLAY_X1,0x00);
    commandSPI(OLED_DISPLAY_X2,0x5F);
    commandSPI(OLED_DISPLAY_Y1,0x00);
    commandSPI(OLED_DISPLAY_Y2,0x5F);
    /* Select the RGB data format and set the initial state of RGB interface port */
    commandSPI(OLED_RGB_IF,0x00);                 // RGB 8bit interface
    /* Set RGB polarity */
    commandSPI(OLED_RGB_POL,0x00);              // DOT clock polarity
    /* Set display mode control */
    commandSPI(OLED_DISPLAY_MODE_CONTROL,0x80);   // SWAP:BGR, Reduce current : Normal, DC[1:0] : Normal
    /* Set MCU Interface */
    commandSPI(OLED_CPU_IF,0x00);                 // MPU External interface mode, 8bits
    /* Set Memory Read/Write mode */
    commandSPI(OLED_MEMORY_WRITE_READ,0x00);
    /* Set row scan direction */
    commandSPI(OLED_ROW_SCAN_DIRECTION,0x00);     // Column : 0 --> Max, Row : 0 --> Max
    /* Set row scan mode */
    commandSPI(OLED_ROW_SCAN_MODE,0x00);          // Alternate scan mode
    /* Set column current */
    commandSPI(OLED_COLUMN_CURRENT_R,0x6E);
    commandSPI(OLED_COLUMN_CURRENT_G,0x4F);
    commandSPI(OLED_COLUMN_CURRENT_B,0x77);
    /* Set row overlap */
    commandSPI(OLED_ROW_OVERLAP,0x00);            // Band gap only
    /* Set discharge time */
    commandSPI(OLED_DISCHARGE_TIME,0x01);         // Discharge time : normal discharge
    /* Set peak pulse delay */
    commandSPI(OLED_PEAK_PULSE_DELAY,0x00);
    /* Set peak pulse width */
    commandSPI(OLED_PEAK_PULSE_WIDTH_R,0x02);
    commandSPI(OLED_PEAK_PULSE_WIDTH_G,0x02);
    commandSPI(OLED_PEAK_PULSE_WIDTH_B,0x02);
    /* Set precharge current */
    commandSPI(OLED_PRECHARGE_CURRENT_R,0x14);
    commandSPI(OLED_PRECHARGE_CURRENT_G,0x50);
    commandSPI(OLED_PRECHARGE_CURRENT_B,0x19);
    /* Set row scan on/off  */
    commandSPI(OLED_ROW_SCAN_ON_OFF,0x00);        // Normal row scan
    /* Set scan off level */
    commandSPI(OLED_SCAN_OFF_LEVEL,0x04);         // VCC_C*0.75
    /* Set memory access point */
    commandSPI(OLED_DISPLAYSTART_X,0x00);
    commandSPI(OLED_DISPLAYSTART_Y,0x00);
    /* Display ON */
    commandSPI(OLED_DISPLAY_ON_OFF,0x01);
}

static void OLED_power_off(void) {
    // Set STANDBY_ON_OFF
    commandSPI(OLED_STANDBY_ON_OFF, 0x01);  // Standby ON
        wait_ms(5);           // wait 5 ms
}
//// Send command with transfer, outcommented doing with tivaware
//static void commandSPI(uint8_t reg, uint8_t value) {
//    uint8_t txBuf[1];
//    bool retVal;
//    txBuf[0] = reg;
//    SPI_Transaction spiTrans;
//    spiTrans.count = 1;        // send 1 Byte
//    spiTrans.txBuf = txBuf;
//    spiTrans.rxBuf = NULL;
//
//    SETBIT(OLED_RW, 0); // Set the peripheral to write -> mcu write to periph
//    // Write to register
//    SETBIT(OLED_CS, 0);
//    SETBIT(OLED_DC, 0);
//    retVal = SPI_transfer(handle, &spiTrans);
//    if (!retVal) {
//        System_printf("Register transfer failed.\n");
//        System_flush();
//    } else {
//        System_printf("Register %u transferred.\n", txBuf[0]);
//        System_flush();
//    }
//    SETBIT(OLED_CS, 1);
//
//    // Write into the register
//    txBuf[0] = value;
//    SETBIT(OLED_CS, 0);
//    SETBIT(OLED_DC, 1);
//    retVal = SPI_transfer(handle, &spiTrans);
//    if (!retVal) {
//        System_printf("Value transfer failed.\n");
//        System_flush();
//    } else {
//        System_printf("Value %u transferred.\n", txBuf[0]);
//        System_flush();
//    }
//    SETBIT(OLED_CS, 1);
//}
// Send command to OLED
static void commandSPI(uint8_t reg, uint8_t value) {
    SETBIT(OLED_RW, 0); // Set the peripheral to write -> mcu write to periph
    // Write to register
    SETBIT(OLED_CS, 0);
    SETBIT(OLED_DC, 0);
    SSIDataPut(OLED_SSI_BASE, reg);
    while(SSIBusy(OLED_SSI_BASE));
    SETBIT(OLED_CS, 1);

    // Write into the register
    SETBIT(OLED_CS, 0);
    SETBIT(OLED_DC, 1);
    SSIDataPut(OLED_SSI_BASE, value);
    while(SSIBusy(OLED_SSI_BASE));
    SETBIT(OLED_CS, 1);
}
