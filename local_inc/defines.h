/*
 * defines.h
 *
 *  Created on: Dec 22, 2018
 *      Author: marcaurel
 */

#ifndef DEFINES_H_
#define DEFINES_H_

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

#define OLED_RW_PORT GPIO_PORTE_BASE        // RW Select
#define OLED_RW_PIN 4

#define OLED_RST_PORT GPIO_PORTC_BASE       // Reset
#define OLED_RST_PIN 7

#define OLED_CS_PORT GPIO_PORTH_BASE        // Chip Select
#define OLED_CS_PIN 2

#define OLED_DC_PORT GPIO_PORTM_BASE        // D/C (A0) select command/data
#define OLED_DC_PIN 3

/* Definition of the SSI2 Ports */
#define SSI2_MOSI_PORT GPIO_PORTD_BASE
#define SSI2_MOSI_PIN 1

#define SSI2_MISO_PORT GPIO_PORTD_BASE
#define SSI2_MISO_PIN 0

#define SSI2_SCK_PORT GPIO_PORTD_BASE
#define SSI2_SCK_PIN 3

/* LED definitions */
#define LED_01_PORT GPIO_PORTN_BASE
#define LED_01_PIN 1

#define LED_02_PORT GPIO_PORTN_BASE
#define LED_02_PIN 0

#define LED_03_PORT GPIO_PORTF_BASE
#define LED_03_PIN 4

#define LED_04_PORT GPIO_PORTF_BASE
#define LED_04_PIN 0

/* Constant Addresses of PINS, muxing with typedefs */
const PinAddress OLED_RST = {OLED_RST_PORT, OLED_RST_PIN};
const PinAddress OLED_DC = {OLED_DC_PORT, OLED_DC_PIN};
const PinAddress OLED_CS = {OLED_CS_PORT, OLED_CS_PIN};
const PinAddress OLED_RW = {OLED_RW_PORT, OLED_RW_PIN};

const PinAddress SSI_MOSI = {SSI2_MOSI_PORT, SSI2_MOSI_PIN};
const PinAddress SSI_MISO = {SSI2_MISO_PORT, SSI2_MISO_PIN};
const PinAddress SSI_SCK = {SSI2_SCK_PORT, SSI2_SCK_PIN};

const PinAddress LED01  = {LED_01_PORT, LED_01_PIN};
const PinAddress LED02  = {LED_02_PORT, LED_02_PIN};
const PinAddress LED03  = {LED_03_PORT, LED_03_PIN};
const PinAddress LED04  = {LED_04_PORT, LED_04_PIN};

/**
 * Set Bit function sets a specific bit on a given position wit the correct value
 */
#define SETBIT(PinAddress, bit) (GPIOPinWrite(PinAddress.port, (1 << PinAddress.pin), (bit << PinAddress.pin)))
#endif /* DEFINES_H_ */
