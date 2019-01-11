/*! \file oled_hal.c
 *  \date Jan 3, 2019
 *  \author Valentin Platzgummer - ic17b096
 *  \brief hardware abstraction layer with all necessary drivers for the OLED 96x96
 */
// ----------------------------------------------------------------------------- includes ---
#include "local_inc/oled_hal.h"

//! \addtogroup group_oled_hal
//! @{

// ----------------------------------------------------------------------------- globals ---
static volatile uint32_t ui32SysClkFreq;
static volatile SPI_Handle handle;

//! \brief Constant Address of PIN OLED Reset
static const PinAddress OLED_RST = {OLED_RST_PORT, OLED_RST_PIN};
//! \brief Constant Address of PIN OLED SPI Command/ value select
static const PinAddress OLED_DC = {OLED_DC_PORT, OLED_DC_PIN};
//! \brief Constant Address of PIN OLED SPI Chip select
static const PinAddress OLED_CS = {OLED_CS_PORT, OLED_CS_PIN};
//! \brief Constant Address of PIN OLED Read/ Write select
static const PinAddress OLED_RW = {OLED_RW_PORT, OLED_RW_PIN};

//! \brief Constant Addresses of the boards LED01
static const PinAddress LED01  = {LED_01_PORT, LED_01_PIN};
//! \brief Constant Addresses of the boards LED02
static const PinAddress LED02  = {LED_02_PORT, LED_02_PIN};
//! \brief Constant Addresses of the boards LED03
static const PinAddress LED03  = {LED_03_PORT, LED_03_PIN};
//! \brief Constant Addresses of the boards LED04
static const PinAddress LED04  = {LED_04_PORT, LED_04_PIN};

// ----------------------------------------------------------------------------- functions ---
static void commandSPI(uint8_t reg, uint8_t value);
static void writeOLED_indexRegister(uint8_t reg);
static void writeOLED_dataRegister(uint8_t data);
static void wait_ms(uint32_t delay);
static color16 createColorPixelFromRGB(color24 rgbData);
// ----------------------------------------------------------------------- implementations ---

//! \brief predefined color white
const color24 whiteColor = {0xFF,0xFF,0xFF};
//! \brief predefined color black
const color24 blackColor = {0x00,0x00,0x00};
//! \brief predefined color red
const color24 redColor = {0xFF,0x00,0x00};
//! \brief predefined color green
const color24 greenColor = {0x00,0xFF,0x00};
//! \brief predefined color blue
const color24 blueColor = {0x00,0x00,0xFF};
/*!
 * \brief dela the system for a given time
 * \param delay uint32_t delay time in milliseconds 10^-3 sec
 */
static void wait_ms(uint32_t delay) {
    // wait for delay in ms Frequency 120MHz
    SysCtlDelay(ui32SysClkFreq / 3000 * delay);
}
/*!
 * \brief Confgure the GPIO Pins for the used peripherals
 * Pins are set correctly for following peripherals
 *  - SPI
 *      - Boosterpack input for SPI is configurable via preprocessor define SSIM_2 or SSIM_3 as needed
 * - LEDs:
 *      - all 4 onboard LEDS
 */
static void Pinmux (void) {
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
/*!
 *
 * \brief enable the SPI connection from the board
 * \param systemFrequency the system frequency set at program start
 * needed to set the bitrate frequency accordingly to the system cycle
 */
extern void initSPI(uint32_t systemFrequency) {
    ui32SysClkFreq = systemFrequency;
    Pinmux();                       // Do all necessary pin muxing

    SPI_Params params;
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
        SETBIT(LED01, 1);   // Set LED01 on to signal SPI is working!
        System_printf("SPI2 has handle at address: %p.\n", handle);
        System_flush();
    }
    SETBIT(OLED_CS, 1);             // Set Chip select to high, not selected
}
/*!
 * \brief draw a given char onto the OLED display
 * The char get printed one by one, and printed onto the screen. Font size may be choose between 3 different sizes.
 * \param c the character get printed onto the screen (char in ascii value)
 * \param font the selected font with all associated data
 * \param fontColor the color of the printed char in classic 24Bit RGB (no alpha channel)
 * \param bgColor background color for the char, because no alpha channel is supported
 * \param origin the lower left corner of the char in the screen coordinates
 */
void drawChar(char c, fontContainer *font, color24 fontColor, color24 bgColor, point origin) {
    // select middle of screen
    // create font rectangle FONT_WIDTH x FONT_HEIGHT (7x13), Text is drawn upside down
    // calculate from the right margin
    // set the drawing window for each char, according to the given font size
    commandSPI(OLED_MEM_X1, OLED_DISPLAY_X_MAX - origin.x);
    commandSPI(OLED_MEM_X2, OLED_DISPLAY_X_MAX -origin.x + font->fontWidth);
    commandSPI(OLED_MEM_Y1, origin.y);
    commandSPI(OLED_MEM_Y2, origin.y + font->fontHeight);
    // write from bottom to top
    commandSPI(OLED_MEMORY_WRITE_READ, OLED_MEMORY_WRITE_READ_HORZ_INC_VERT_INC);
    // enable DDRAM for writing
    writeOLED_indexRegister(OLED_DDRAM_DATA_ACCESS_PORT);
    uint8_t i,j,k,value;

    color16 charCol = createColorPixelFromRGB(fontColor);
    color16 backCol = createColorPixelFromRGB(bgColor);
    // outer loop defines the height od each char
    for (i = 0; i < font->fontHeight * font->fontDepthByte; i+= font->fontDepthByte) {
        // each char may wider than 8 bit
        for (k = 0; k < font->fontDepthByte; k++) {
            value = font->font[c * font->charArrayLength + i + k];
            // step through each bit of the value to find if set or unset.
            for (j = 0; j < 8; j++) {
                if (value & 1) {
                    writeOLED_dataRegister(charCol.upperByte);
                    writeOLED_dataRegister(charCol.lowerByte);
                } else {
                    writeOLED_dataRegister(backCol.upperByte);
                    writeOLED_dataRegister(backCol.lowerByte);
                }
                value >>= 1; // step bitwise through the font (8Bit)
            }
        }
    }
}
/*
 * \brief shuts the OLED Display down
 */
void OLED_power_off(void) {
    // Set STANDBY_ON_OFF
    createBackgroundFromImage(cool_image);
    wait_ms(10000);           // wait 10  s
    commandSPI(OLED_DISPLAY_ON_OFF, 0x00);  // Display OFF
    wait_ms(5);           // wait 5 ms
}

static void adressEntireOLED(void) {
    // center the screen
    commandSPI(OLED_DISPLAYSTART_X, 0);
    commandSPI(OLED_DISPLAYSTART_Y, 0);
    // select entire screen
    commandSPI(OLED_MEM_X1, 0);
    commandSPI(OLED_MEM_X2, OLED_DISPLAY_X_MAX);
    commandSPI(OLED_MEM_Y1, 0);
    commandSPI(OLED_MEM_Y2, OLED_DISPLAY_Y_MAX);
}
/*!
 * \brief create a background with an uniform color for the display-
 * \param rgbColor color24, background color in classic 24Bit RGB (no alpha channel)
 */
void createBackgroundFromColor(color24 rgbColor) {
    adressEntireOLED();
    color16 color;
    uint16_t i;
    color = createColorPixelFromRGB(rgbColor);
    // enable DDRAM for writing
    writeOLED_indexRegister(OLED_DDRAM_DATA_ACCESS_PORT);
    // loop through all pixel of oled, register is 8 bit wide, but has to be 16, so double it!
    for (i = 0; i < OLED_DISPLAY_MAX_PIXEL; i++) {
        writeOLED_dataRegister(color.upperByte);  // Upper Byte first Transmission S. 10/43
        writeOLED_dataRegister(color.lowerByte);
    }
}
/*!
 * \brief create a background from an given image.
 * \param screenimage image in bitmap format, supplied by a c-array
 */
void createBackgroundFromImage(image screenimage) {
    uint16_t i;
    // enable DDRAM for writing
    writeOLED_indexRegister(OLED_DDRAM_DATA_ACCESS_PORT);
    // loop through all pixel of oled, register is 8 bit wide, but has to be 16, so double it!
    for (i = 1; i < (screenimage.width * screenimage.height * screenimage.bytes_per_pixel + 1); i++) {
        writeOLED_dataRegister(screenimage.pixel_data[i]);
    }
}
/*!
 * \brief Convert a 24Bit(8:8:8) RGB value to 16 Bit RGB (5:6:5) Pixel value
 * \param rgbData color value of a pixel in 24bit RGB(8:8:8) no Alpha cannel
 * \return converted colorvalue in 16bit RGB space (5:6:5) space, divided into 2 Byte (MSB and LSB)
 */
static color16 createColorPixelFromRGB(color24 rgbData) {
    color16 result_color;
    uint16_t result;
    result = (rgbData.red  & 0xFF) / 5 << 11;
    result |= (rgbData.green & 0xFF) / 6 << 5;
    result |= (rgbData.blue & 0xFF) / 5;
    result_color.upperByte = (result >> 8) & 0xFF;
    result_color.lowerByte = result & 0xFF;
    return result_color;
}
/*!
 * \brief enables or disables the screensaver scroll down functionality (build in)
 * \param enable 1 for enabling function, 0 for disabling
 */
void toggleDownScroll(bool enable) {
    // Enable/ disable Screen saver
    commandSPI(0xD0, enable<<7);
    // Configure screen saver update time -> 0xFF 2sec of Time
    commandSPI(0xD3, 0xFF);
    // Screen Saver 'Down Scroll';
    commandSPI(0xD2, enable<<1);
    // Set LED04 to signaling Screen saver mode ON
    SETBIT(LED04, enable);
}

/*!
 * \brief write to the OLEDs Controller Index register.
 * \param reg index of the requested register
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
/*!
 * \brief write to the OLEDs Controller data register.
 * \param reg value for the selected register
 */
static void writeOLED_dataRegister(uint8_t data) {
    SETBIT(OLED_CS, 0);
    SETBIT(OLED_DC, 1);
    SSIDataPut(OLED_SSI_BASE, data);
    while(SSIBusy(OLED_SSI_BASE));
    SETBIT(OLED_CS, 1);
}
/*!
 * \brief send a complete command to the OLED controller,
 * \param reg select the desired index register from controller
 * \param value value get written into the selected register
 */
static void commandSPI(uint8_t reg, uint8_t value) {
    // Write to register
    writeOLED_indexRegister(reg);
    // Write into the register
    writeOLED_dataRegister(value);
}
/*!
 * \brief power on and initialize procedure of the OLED.
 * all necessary initial parameters are written to the corresponding register
 */
void OLED_power_on(void) {
    // wait for 100ms
    wait_ms(100);
    // Set RST to LOW
    SETBIT(OLED_RST, 0);
    wait_ms(10); // wait 10 ms
    // Set RST to HIGH
    SETBIT(OLED_RST, 1);
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
    commandSPI(OLED_DISPLAY_X2,OLED_DISPLAY_X_MAX);
    commandSPI(OLED_DISPLAY_Y1,0x00);
    commandSPI(OLED_DISPLAY_Y2,OLED_DISPLAY_Y_MAX);
    /* Select the RGB data format and set the initial state of RGB interface port */
    commandSPI(OLED_RGB_IF,0x00);                 // RGB 8bit interface
    /* Set RGB polarity */
    commandSPI(OLED_RGB_POL,0x00);              // DOT clock polarity
    /* Set display mode control */
    commandSPI(OLED_DISPLAY_MODE_CONTROL,0x80);   // SWAP:BGR, Reduce current : Normal, DC[1:0] : Normal
    /* Set MCU Interface */
    commandSPI(OLED_CPU_IF,0x00);                 // MPU External interface mode, 8bits
    /* Set Memory Read/Write mode */
    commandSPI(OLED_MEMORY_WRITE_READ,OLED_MEMORY_WRITE_READ_HORZ_DEC_VERT_INC);      // When MDIR0= 1, Horizontal address counter is decreased. S. Datasheet 24/43
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
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
