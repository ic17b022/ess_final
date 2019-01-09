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
#include <string.h>

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

/* Driver Headers */
#include <ti/drivers/SPI.h>     // supplies definitions for spi interface
/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>      // Delivers the pin map defines
#include <driverlib/ssi.h>      // Supplies the defines for SSI setup
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h> // Supplies GPIO_PORTx_BASE
#include <inc/hw_gpio.h>
#include <inc/hw_types.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

#endif /* COMMON_H_ */
