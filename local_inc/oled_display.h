/*! \file oled_display.h
 *  \date Dec 27, 2018
 *  \author Valentin Platzgummer
 *  \brief header file for the OLED Driver
 */

#ifndef OLED_DISPLAY_H_
#define OLED_DISPLAY_H_

// ------------------------------------ includes ---
#include "common.h"

// ------------------------------------ functions ---
extern void initSPI(uint32_t systemFrequency);
extern void setup_OLED_task(xdc_String name, uint8_t priority);

#endif /* OLED_DISPLAY_H_ */
