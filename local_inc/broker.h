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
#include "UART_Task.h"
#include "oled_display.h"

//! \addtogroup group_comm
//! @{
// ----------------------------------------------------------------------------- typedefs ---

// ------------------------------------------------------------------------------ globals ---
//! \brief semaphore for IPC communication between Broker and input, whether heartrate module or UART
Mailbox_Handle heartrateMailbox;
//! \brief semaphore for IPC communication between Broker and OLED
Mailbox_Handle oledMailbox;
//! \brief semaphore for writing from broker to UART;
Mailbox_Handle brokerWrite;
//! \brief semaphore for reading from UART to broker
Mailbox_Handle brokerRead;

// ---------------------------------------------------------------------------- functions ---
extern void setup_Broker_task(xdc_String name, uint8_t priority);
extern void Broker_task(void);
extern uint8_t getTestcase();
extern bool getChanged(void);
extern void resetChanged(void);
#endif /* BROKER_H_ */
// Ende ot Doxygen group
//! @}
