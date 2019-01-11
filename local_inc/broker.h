/*
 * \file broker.h
 * \brief Header file for the broker functionality a intermediate between UART, input and output Module
 * \date Jan 9, 2019
 * \author Valentin Platzgummer ic17b096
 */

#ifndef BROKER_H_
#define BROKER_H_

// ----------------------------------------------------------------------------- includes ---
#include "common.h"

//! \addtogroup group_comm
//! @{
// ----------------------------------------------------------------------------- typedefs ---

// ------------------------------------------------------------------------------ globals ---
//! \brief semaphore for IPC communication between Broker and UART
Semaphore_Handle uart_sem;
//! \brief semaphore for IPC communication between Broker and OLED
Semaphore_Handle output_sem;
//! \brief char Container for UART
char uartChar;
char oledChar;
//! \brief shared memory pulse comes from heartrate module
uint8_t pulse;
//! \brief shared memory pulsechar to be displayed in oled module
char pulseChar[4];

// ---------------------------------------------------------------------------- functions ---
extern void setup_Broker_task(xdc_String name, uint8_t priority);
extern void Broker_task(void);
#endif /* BROKER_H_ */
// Ende ot Doxygen group
//! @}
