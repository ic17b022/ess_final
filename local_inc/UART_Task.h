/*!
 * \file UART_Task.h
 * \brief UART Task detects keystroke and sends the detected chars to the OLED Task for displaying
 * \author Valentin Platzgummer ic17b096
 * \date Jan, 02 2019
 */

#ifndef UART_TASK_H_
#define UART_TASK_H_

// ----------------------------------------------------------------------------- includes ---
//
#include <ti/drivers/UART.h>
//! \addtogroup group_comm
//! @{
// ------------------------------------------------------------------------------ globals ---

Semaphore_Handle sem;
char charContainer;

// ------------------------------------------------------------------------------ defines ---
/// \def UART_BAUD_RATE used baudrate for the UART connection
#define UART_BAUD_RATE 9600

// ---------------------------------------------------------------------------- functions ---
extern uint8_t getTestcase();
extern bool getChanged(void);
extern void resetChanged(void);
/*!
 *  \brief Execute UART Task
 *  \param arg0 void
 *  s\param arg1 void
 *
*/
void UARTFxn(UArg arg0, UArg arg1);

/*!
 *  \brief Setup UART task
 *
 *  Setup UART task
 *  Task has highest priority and receives 1kB of stack
 *
 *  \return nothing. In case of error the system halts.
*/
void setup_UART_Task(xdc_String name, uint8_t priority);

#endif

// End Doxygen Group
//! @}
