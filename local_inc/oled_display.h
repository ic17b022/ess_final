/*
 * oled_display.h
 *
 *  Created on: Dec 27, 2018
 *      Author: marcaurel
 */


#ifndef OLED_DISPLAY_H_
#define OLED_DISPLAY_H_

#include "common.h"

// switch between ssi2 port and ssi3 port in case of necessary
#define SSIM_2 1
#define SSIM_3 0

typedef struct PinAddress {
    uint32_t port;
    uint8_t pin;
} PinAddress;

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


// Control pins for OLED Boosterpack 1
#if SSIM_2
    #define OLED_RW_PORT GPIO_PORTE_BASE        // RW Select
    #define OLED_RW_PIN 4
    #define OLED_RST_PORT GPIO_PORTC_BASE       // Reset
    #define OLED_RST_PIN 7
    #define OLED_CS_PORT GPIO_PORTH_BASE        // Chip Select
    #define OLED_CS_PIN 2
    #define OLED_DC_PORT GPIO_PORTM_BASE        // D/C (A0) select command/data
    #define OLED_DC_PIN 3
    #define OLED_SSI_BASE SSI2_BASE             // Configure OLED to SPI 2
#endif

// Control pins for OLED Boosterpack 2
#if SSIM_3
    #define OLED_RW_PORT GPIO_PORTD_BASE        // RW Select
    #define OLED_RW_PIN 2
    #define OLED_RST_PORT GPIO_PORTP_BASE       // Reset
    #define OLED_RST_PIN 4
    #define OLED_CS_PORT GPIO_PORTP_BASE        // Chip Select
    #define OLED_CS_PIN 5
    #define OLED_DC_PORT GPIO_PORTM_BASE        // D/C (A0) select command/data
    #define OLED_DC_PIN 7
    #define OLED_SSI_BASE SSI3_BASE             // Configure OLED to SPI 2
#endif

// Datasheet Serial clock cycle min 200ns -> 5MHz
#define SSI_FREQUENCY 5000000                   // SSi Frequency is 5MHz
#define OLED_SSI_MODE SPI_POL1_PHA1             // SSI Data Transfer Mode Polarity 1 /

/* LED definitions */
#define LED_01_PORT GPIO_PORTN_BASE
#define LED_01_PIN 1
#define LED_02_PORT GPIO_PORTN_BASE
#define LED_02_PIN 0
#define LED_03_PORT GPIO_PORTF_BASE
#define LED_03_PIN 4
#define LED_04_PORT GPIO_PORTF_BASE
#define LED_04_PIN 0

/**
 * Set Bit function sets a specific bit on a given position wit the correct value
 */
#define SETBIT(PinAddress, bit) (GPIOPinWrite(PinAddress.port, (1 << PinAddress.pin), (bit << PinAddress.pin)))


/* ******* FORWARD DECLARATIONS  ********** */
extern void initSPI(uint32_t systemFrequency);
extern void setup_power_on_task(xdc_String name);

#endif /* OLED_DISPLAY_H_ */
