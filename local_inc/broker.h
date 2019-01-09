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
//! \brief struct defnition for the 3 shared values from input
typedef struct data {
    uint8_t temp_full;          //!< integer part for the temperature value
    uint8_t temp_fraction;      //!< fraction part for the temperature value
    uint8_t pulse;              //!< heard pulse value
    uint8_t oxygen;             //!< oxygen value of blooth
} data;

// ------------------------------------------------------------------------------ globals ---
//! \brief semaphore for IPC communication between Broker and UART
Semaphore_Handle uart_sem;

//! \brief semaphore for IPC communication between Broker and OLED
Semaphore_Handle output_sem;
//! \brief char Container for UART
char uartChar;
char oledChar;
data shared;

// ---------------------------------------------------------------------------- functions ---
extern void setup_Broker_task(xdc_String name, uint8_t priority);
extern void Broker_task(void);
#endif /* BROKER_H_ */
// Ende ot Doxygen group
//! @}
