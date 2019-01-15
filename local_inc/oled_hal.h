/*! \file oled_hal.h
 *  \date Jan 3, 2019
 *  \author Valentin Platzgummer - ic17b096
 *  \brief Header for hardware abstraction layer with all necessary drivers for the OLED 96x96
 */

#ifndef OLED_HAL_H_
#define OLED_HAL_H_

// ----------------------------------------------------------------------------- includes ---
#include "common.h"
#include "../resources/image.h"
#include "../resources/font.h"

//! \addtogroup group_oled_hal
//! @{

// ----------------------------------------------------------------------------- typedefs ---
//! \brief struct PinAddress store the entire Address (Base Port and Pin Number) of a given PIN
typedef struct PinAddress {
    uint32_t port;  //!< Port Base Address
    uint8_t pin;    //!< Pin Number (0-7)
} PinAddress;

//! \brief struct color16 stores the upper and lower significant Bytes form a 16Bit color word
typedef struct color16 {
    uint8_t upperByte;  //!< Most significant Byte of a 16Bit Color word
    uint8_t lowerByte;  //!< Least significant Byte of a 16Bit Color word
} color16;
//! \brief struct color24 stores the 3 color values RBG
typedef struct color24 {
    uint8_t red;    //!< red  0-255
    uint8_t green;  //!< green 0-255
    uint8_t blue;   //!< blue 0-255
} color24;
//! \brief struct point stores the coordinates of a given point
typedef struct point {
    uint8_t x;  //!< uint8_t x-coordinate of the point 0-95
    uint8_t y;  //!< uint8_t y-coordinate of the point 0-95
} point;
//! \brief struct rect stores the coordinates of a given rect
typedef struct rect {
    point origin;   //!< point coordinate origin lower left point
    uint8_t width;  //!< width of the rect
    uint8_t height; //!< height of the rect
} rect;
// ----------------------------------------------------------------------------- defines ---
// switch between ssi2 port and ssi3 port in case of necessary
#define SSIM_2 1    //!< Use the Boosterpack Port 2
#define SSIM_3 0    //!< Use Boosterpack Port 3

#define OLED_SOFT_RESET 0x01
#define OLED_DISPLAY_ON_OFF 0x02
#define OLED_ANALOG_CONTROL 0x0F    //
#define OLED_STANDBY_ON_OFF 0x14
#define OLED_OSC_ADJUST 0x1A
#define OLED_ROW_SCAN_DIRECTION 0x09
#define OLED_DISPLAY_X1 0x30
#define OLED_DISPLAY_X2 0x31
#define OLED_DISPLAY_Y1 0x32
#define OLED_DISPLAY_Y2 0x33
#define OLED_DISPLAYSTART_X 0x38
#define OLED_DISPLAYSTART_Y 0x39
#define OLED_CPU_IF 0x0D
#define OLED_MEM_X1 0x34
#define OLED_MEM_X2 0x35
#define OLED_MEM_Y1 0x36
#define OLED_MEM_Y2 0x37
#define OLED_MEMORY_WRITE_READ 0x1D
#define OLED_DDRAM_DATA_ACCESS_PORT 0x08
#define OLED_DISCHARGE_TIME 0x18
#define OLED_PEAK_PULSE_DELAY 0x16
#define OLED_PEAK_PULSE_WIDTH_R 0x3A
#define OLED_PEAK_PULSE_WIDTH_G 0x3B
#define OLED_PEAK_PULSE_WIDTH_B 0x3C
#define OLED_PRECHARGE_CURRENT_R 0x3D
#define OLED_PRECHARGE_CURRENT_G 0x3E
#define OLED_PRECHARGE_CURRENT_B 0x3F
#define OLED_COLUMN_CURRENT_R 0x40
#define OLED_COLUMN_CURRENT_G 0x41
#define OLED_COLUMN_CURRENT_B 0x42
#define OLED_ROW_OVERLAP 0x48
#define OLED_SCAN_OFF_LEVEL 0x49
#define OLED_ROW_SCAN_ON_OFF 0x17
#define OLED_ROW_SCAN_MODE 0x13
#define OLED_SCREEN_SAVER_CONTEROL 0xD0
#define OLED_SS_SLEEP_TIMER 0xD1
#define OLED_SCREEN_SAVER_MODE 0xD2
#define OLED_SS_UPDATE_TIMER 0xD3
#define OLED_RGB_IF 0xE0
#define OLED_RGB_POL 0xE1
#define OLED_DISPLAY_MODE_CONTROL 0xE5
//! \brief Direction of OLED displaying order
#define OLED_MEMORY_WRITE_READ_HORZ_INC_VERT_INC 0  //!< horizontal increment, vertical increment, starts upper left
#define OLED_MEMORY_WRITE_READ_HORZ_DEC_VERT_INC 1  //!< horizontal decrement, vertical increment, starts upper right
#define OLED_MEMORY_WRITE_READ_HORZ_INC_VERT_DEC 2  //!< horizontal increment, vertical decrement, starts lower left
#define OLED_MEMORY_WRITE_READ_HORZ_DEC_VERT_DEC 3  //!< horizontal decrement, vertical decrement, starts lower right

#define OLED_DISPLAY_X_MAX 0x5F               //!< maximum amount of pixel x coordinate
#define OLED_DISPLAY_Y_MAX 0x5F               //!< maximum amount of pixel y coordinate
#define OLED_DISPLAY_MAX_PIXEL 0x2400           //!< maximum amount of pixel square
#define OLED_DISPLAY_BYTES_PIXEL 2          //!< maximum amount of pixel x coordinate

// Control pins for OLED Boosterpack 1
#if SSIM_2
#define OLED_RW_PORT GPIO_PORTE_BASE        //!< RW Select, SPI2
#define OLED_RW_PIN 4
#define OLED_RST_PORT GPIO_PORTC_BASE       //!< Reset SPI, SPI2
#define OLED_RST_PIN 7
#define OLED_CS_PORT GPIO_PORTH_BASE        //!< Chip Select SPI, SPI2
#define OLED_CS_PIN 2
#define OLED_DC_PORT GPIO_PORTM_BASE        //!< D/C (A0) select command/data, SPI2
#define OLED_DC_PIN 3
#define OLED_SSI_BASE SSI2_BASE             //!< Configure OLED to SPI 2
#endif

// Control pins for OLED Boosterpack 2
#if SSIM_3
#define OLED_RW_PORT GPIO_PORTD_BASE        //!< RW Select, SPI3
#define OLED_RW_PIN 2
#define OLED_RST_PORT GPIO_PORTP_BASE       //!< Reset, SPI3
#define OLED_RST_PIN 4
#define OLED_CS_PORT GPIO_PORTP_BASE        //!< Chip Select, SPI3
#define OLED_CS_PIN 5
#define OLED_DC_PORT GPIO_PORTM_BASE        //!< D/C (A0) select command/data, SPI3
#define OLED_DC_PIN 7
#define OLED_SSI_BASE SSI3_BASE             //!< Configure OLED to SPI 3
#endif

// Datasheet Serial clock cycle min 200ns -> 5MHz
#define SSI_FREQUENCY 5000000                   //!< SSi Frequency is 5MHz
#define OLED_SSI_MODE SPI_POL1_PHA1             //!< SSI Data Transfer Mode Polarity 1 /

/* LED definitions */
#define LED_01_PORT GPIO_PORTN_BASE
#define LED_01_PIN 1
#define LED_02_PORT GPIO_PORTN_BASE
#define LED_02_PIN 0
#define LED_03_PORT GPIO_PORTF_BASE
#define LED_03_PIN 4
#define LED_04_PORT GPIO_PORTF_BASE
#define LED_04_PIN 0

//! \brief Set Bit function sets a specific bit on a given position to the given value
#define SETBIT(PinAddress, bit) (GPIOPinWrite(PinAddress.port, (1 << PinAddress.pin), (bit << PinAddress.pin)))

// ----------------------------------------------------------------------------- globals---
extern const color24 whiteColor;
extern const color24 blackColor;
extern const color24 redColor;
extern const color24 greenColor;
extern const color24 blueColor;
// -------------------------------------------------------------------------- functions ---

/*!
 * \brief draw a given char onto the OLED display
 * The char get printed one by one, and printed onto the screen. Font size may be choose between 3 different sizes.
 * \param c char, the character get printed onto the screen (char in ascii value)
 * \param fontSize uint8_t, the fonts size in 3 different sizes, 1 small, 2 middle, 3 large
 * \param fontColor uint32_t, the color of the printed char in classic 24Bit RGB (no alpha channel)
 * \param bgColor uint32_t, background color for the char, because no alpha channel is supported
 * \param origin point, the lower left corner of the char in the screen coordinates
 */
void drawChar(char c, fontContainer *font, color24 fontColor, color24 bgColor, point origin);
extern void createBackgroundFromImage(image screenimage);
extern void createBackgroundFromColor(color24 rgbColor);
extern void OLED_power_on(void);
extern void OLED_power_off(void);
extern void toggleUpScroll(bool enable);

#endif /* OLED_HAL_H_ */
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
