/*
 * common.h
 *
 *  Created on: Dec 27, 2018
 *      Author: marcaurel
 */

#ifndef COMMON_H_
#define COMMON_H_

/* Standard bool and standard int header */
#include <stdbool.h>
#include <stdint.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Instrumentation headers */
#include <ti/uia/runtime/LogSnapshot.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin_map.h>      // Delivers the pin map defines
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h> // Supplies GPIO_PORTx_BASE
#include <driverlib/ssi.h>      // Supplies the difnes for SSI setup

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Peripheral Header files */


/* Application headers */



#endif /* COMMON_H_ */
