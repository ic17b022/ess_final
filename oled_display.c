/*! \file oled_display.c
 *  \date Dec 27, 2018
 *  \author Valentin Platzgummer
 *  \brief all necessary drivers for the OLED 96x96 and the task to use it in the main thread
 */

#include "local_inc/oled_display.h"
#include "resources/image.h"
#include "resources/font.h"
#include "local_inc/UART_Task.h"

static volatile uint32_t ui32SysClkFreq;
static volatile SPI_Handle handle;

/* ******* CONSTANTS  ********** */
/// \brief Constant Addresses of PINS to drive the OLED Adresses are packed into a struct
static const PinAddress OLED_RST = {OLED_RST_PORT, OLED_RST_PIN};
static const PinAddress OLED_DC = {OLED_DC_PORT, OLED_DC_PIN};
static const PinAddress OLED_CS = {OLED_CS_PORT, OLED_CS_PIN};
static const PinAddress OLED_RW = {OLED_RW_PORT, OLED_RW_PIN};

/// \brief Constant Addresses of the boards LED- PINS, Adresses are packed into a struct
static const PinAddress LED01  = {LED_01_PORT, LED_01_PIN};
static const PinAddress LED02  = {LED_02_PORT, LED_02_PIN};
static const PinAddress LED03  = {LED_03_PORT, LED_03_PIN};
static const PinAddress LED04  = {LED_04_PORT, LED_04_PIN};

// Forward Declarations
static void commandSPI(uint8_t reg, uint8_t value);
static void writeOLED_indexRegister(uint8_t reg);
static void writeOLED_dataRegister(uint8_t data);
static void wait_ms(uint32_t delay);
static void OLED_Fxn(void);
static void OLED_power_on(void);
static void OLED_power_off(void);
static void createBackgroundFromImage(image screenimage);
static void createBackgroundFromColor(uint32_t rgbColor);
static color16 createColorPixelFromRGB(uint32_t rgbData);
static void drawChar(char c, uint32_t fontColor, uint32_t bgColor, point origin);


static void wait_ms(uint32_t delay) {
    // wait for delay in ms Frequency 120MHz
    SysCtlDelay(ui32SysClkFreq / 3000 * delay);
}

// Confgure the GPIO Pins for the onboard LEDs
void Pinmux (void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    // LED 03 + LED04
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);    // LED 01 + LED02

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
/* \fn setup_OLED_task
 * \brief create a new OLED Task and initialize it with the necessary parameters.
 * \param name xdc_String, identifying name of the task
 * \param priority uitn8_t initial priority of the task (1-15) 15 is highest priority
 */
extern void setup_OLED_task(xdc_String name, uint8_t priority) {
    Task_Params taskLedParams;
    Task_Handle taskLed;
    Error_Block eb;
    /* Create OLED startup task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.instance->name = name;
    taskLedParams.stackSize = 1024; /* stack in bytes */
    taskLedParams.priority = priority; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskLed = Task_create((Task_FuncPtr)OLED_Fxn, &taskLedParams, &eb);
    if (taskLed == NULL) {
        System_abort("TaskLed create failed");
    }
}
static void OLED_Fxn(void) {
    // power on OLED
    OLED_power_on();
    createBackgroundFromImage(logo_image);
    createBackgroundFromColor(0x00FF00);
    point origin = { 20,20 };
    bool sem_timeout;

    while (1) {
       sem_timeout = Semaphore_pend(sem, BIOS_WAIT_FOREVER);
        if (!sem_timeout) {
            System_printf("Semaphore has time out.\n");
            System_flush();
        }
        drawChar(charContainer, 0xFF0000, 0x00FF00, origin);
        origin.x += FONT_SPACING; // Note text is drawing backwards
        }

}
/*! \fn drawChar
 * \brief draw a given char onto the OLEd display
 * The char get printed one by one, and printed onto the screen. with the current font size 7x13
 * max 12 chars per line.
 * \param c char, the character get printed onto the screen (char in ascii value)
 * \param fontColor uint32_t, the color of the printed char in classic 24Bit RGB (no alpha channel)
 * \param bgColor uint32_t, background color for the char, because no alpha channel is supported
 * \param origin point, the lower left corner of the char in the screen cooridnates
 */
static void drawChar(char c, uint32_t fontColor, uint32_t bgColor, point origin) {
    // select middle of screen
    // create font rectangle FONT_WIDTH x FONT_HEIGHT (7x13), Text is drawn upside down
    // calculate from the right margin
    commandSPI(OLED_MEM_X1, OLED_DISPLAY_X_MAX - origin.x);
    commandSPI(OLED_MEM_X2, OLED_DISPLAY_X_MAX -origin.x + FONT_WIDTH);
    commandSPI(OLED_MEM_Y1, origin.y);
    commandSPI(OLED_MEM_Y2, origin.y + FONT_HEIGHT);
    // write from bottom to top
    commandSPI(OLED_MEMORY_WRITE_READ, OLED_MEMORY_WRITE_READ_HORZ_INC_VERT_DEC);
    // enable DDRAM for writing
    writeOLED_indexRegister(OLED_DDRAM_DATA_ACCESS_PORT);
    uint8_t i,j,value;
    color16 charCol = createColorPixelFromRGB(fontColor);
    color16 backCol = createColorPixelFromRGB(bgColor);
    System_printf("Drawing char: %c, code: ", c);
    for (i = 0; i < FONT_HEIGHT; i++) {
        value = chars[c - FONT_STARTING_NUMBER][i];
        for (j = 0; j < FONT_BIT_PER_CHAR; j++) {
            if (value & 1) {
                writeOLED_dataRegister(charCol.upperByte);
                writeOLED_dataRegister(charCol.lowerByte);
            } else {
                writeOLED_dataRegister(backCol.upperByte);
                writeOLED_dataRegister(backCol.lowerByte);
            }
            value >>= 1; // step bitwise through the font (8Bit)
        }
        System_printf("%u ", chars[c - 0x20][i]);
    }
    System_printf("\n");
    System_flush();
}
static void OLED_power_off(void) {
    // Set STANDBY_ON_OFF
    createBackgroundFromImage(cool_image);
    wait_ms(10000);           // wait 10  s
    commandSPI(OLED_DISPLAY_ON_OFF, 0x00);  // Display OFF
    wait_ms(5);           // wait 5 ms
}
/*! \fn createBackgroundFromColor
 * \brief create a background with an uniform color for the display-
 * \param rgbColor uint32_t, background color in classic 24Bit RGB (no alpha channel)
 */
static void createBackgroundFromColor(uint32_t rgbColor) {
    color16 color;
    uint16_t i;
    color = createColorPixelFromRGB(rgbColor);
    // enable DDRAM for writing
    writeOLED_indexRegister(OLED_DDRAM_DATA_ACCESS_PORT);
    // loop through all pixel of oled, register is 8 bit wide, but has to be 16, so double it!
    for (i = 0; i < OLED_DISPLAY_X_MAX * OLED_DISPLAY_Y_MAX; i++) {
        writeOLED_dataRegister(color.upperByte);  // Upper Byte first Transmission S. 10/43
        writeOLED_dataRegister(color.lowerByte);
    }
}
/*! \brief create a background from an given image.
 * \param screenimage image, inamge in bitmap format, supplied by a c-array
 */
static void createBackgroundFromImage(image screenimage) {
    uint16_t i;
    // enable DDRAM for writing
    writeOLED_indexRegister(OLED_DDRAM_DATA_ACCESS_PORT);
    // loop through all pixel of oled, register is 8 bit wide, but has to be 16, so double it!
    for (i = 1; i < (screenimage.width * screenimage.height * screenimage.bytes_per_pixel + 1); i++) {
        writeOLED_dataRegister(screenimage.pixel_data[i]);
    }
}
/*! \fn createColorPixelFromRGB
 * \brief Convert a 24Bit(8:8:8) RGB value to 16 Bit RGB (5:6:5) Pixel value
 * \param rgbData uint32_t color value of a pixel in 24bit RGB(8:8:8) no Alpha cannel
 * \return color16 converted colorvalue in 16bit RGB space (5:6:5) space, divided into 2 Byte (MSB and LSB)
 */
static color16 createColorPixelFromRGB(uint32_t rgbData) {
    color16 result_color;
    uint16_t result;
    result = (rgbData >> 16 & 0xFF) / 5 << 11;
    result |= (rgbData >> 8 & 0xFF) / 6 << 5;
    result |= (rgbData & 0xFF) / 5;
    result_color.upperByte = (result >> 8) & 0xFF;
    result_color.lowerByte = result & 0xFF;
    return result_color;
}
/*! \fn writeOLED_indexRegister
 * \brief write to the OLEDs Controller Index register.
 * \param reg uint8_t the index of the requested register
 */
static void writeOLED_indexRegister(uint8_t reg) {
    SETBIT(OLED_RW, 0); // Set the peripheral to write -> mcu write to periph
    // Write to register
    SETBIT(OLED_CS, 0);
    SETBIT(OLED_DC, 0);
    SSIDataPut(OLED_SSI_BASE, reg);
    while(SSIBusy(OLED_SSI_BASE));
    SETBIT(OLED_CS, 1);
}
/*! \fn  writeOLED_dataRegister
 * \brief write to the OLEDs Controller data register.
 * \param reg uint8_t write the value to the selected register
 */
static void writeOLED_dataRegister(uint8_t data) {
    SETBIT(OLED_CS, 0);
    SETBIT(OLED_DC, 1);
    SSIDataPut(OLED_SSI_BASE, data);
    while(SSIBusy(OLED_SSI_BASE));
    SETBIT(OLED_CS, 1);
}
/*! \fn commandSPI
 * \brief send a complete command to the OLED controller,
 * \param reg uint8_t select the desired index register from controller
 * \param value uint8_t this value get written into the selected register
 */
static void commandSPI(uint8_t reg, uint8_t value) {
    // Write to register
    writeOLED_indexRegister(reg);
    // Write into the register
    writeOLED_dataRegister(value);
}
/*! \fn OLED_power_on
 * \ brief power on and initialize procedure of the OLED. */
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
    commandSPI(OLED_MEMORY_WRITE_READ,0x01);      // When MDIR0= 1, Horizontal address counter is decreased. S. Datasheet 24/43
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
    System_printf("OLED powered on.\n");
    System_flush();
}
