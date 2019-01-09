/*! \file common.h
 *  \brief including all shared headers files osed by this project, mainly RTOS and driverlib headers
 *  \date Jan 1, 2019
 *  \author Valentin Platzgummer
*/

#ifndef COMMON_H_
#define COMMON_H_
// define doxygen module groups, works like preprocessor defines
/*! \defgroup group_main Heart Rate 4  & OLED C
 * @{
 * \defgroup group_oled_res OLED Graphical Resources
 * \defgroup group_oled_hal OLED Hardware Abstraction Layer
 * \defgroup group_oled_app OLED Application Layer
 * \defgroup group_comm Communication Layer
 * @}
 */

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
#include <ti/sysbios/knl/Task.h>        // supplies the Task
#include <ti/sysbios/knl/Semaphore.h>   // supplies the Semaphore

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
