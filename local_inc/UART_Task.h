/*!
 * \file UART_Task.h
 * \brief UART Task detects keystroke and sends the detected chars to the OLED Task for displaying
 * \author Valentin Platzgummer ic17b096
 * \date Jan, 02 2019
 */

#ifndef UART_TASK_H_
#define UART_TASK_H_

// ----------------------------------------------------------------------------- includes ---
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <driverlib/sysctl.h>
#include <ti/drivers/UART.h>
#include <ti/sysbios/knl/Semaphore.h>
/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>

/*Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

// ------------------------------------------------------------------------------ globals ---

Semaphore_Handle sem;
char charContainer;
// ------------------------------------------------------------------------------ defines ---
/// \def UART_BAUD_RATE used baudrate for the UART connection
#define UART_BAUD_RATE 9600

// ---------------------------------------------------------------------------- functions ---
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
