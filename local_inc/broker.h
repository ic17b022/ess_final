/*
 * \file broker.h
 * \brief Header file for the broker functionality a intermediate between UART, input and output Module
 * \date Jan 9, 2019
 * \author Valentin Platzgummer ic17b096
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "common.h"
//! \addtogroup group_comm
//! @{

typedef struct data {
    uint8_t temp_full;
    uint8_t temp_fraction;
    uint8_t pulse;
    uint8_t oxygen;
} data;
//! \brief semaphore for IPC Broker and OLED
Semaphore_Handle output_sem;
//! \brief char Container fro UART
char uartChar;
data shared;

#endif /* BROKER_H_ */
// Ende ot Doxygen group
//! @}
