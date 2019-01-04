/*
 *  ======== StartBIOS.c ========
 */

// include all common headers
#include "local_inc/common.h"
#include "local_inc/oled_display.h"
#include "local_inc/UART_Task.h"

int main(void)
{
    uint32_t ui32SysClock;
    /* Call board init functions. */
    ui32SysClock = Board_initGeneral(120*1000*1000);
    (void)ui32SysClock; // We don't really need this (yet)

    Board_initI2C();
    Board_initSPI();

    // init the SPI with the actual system clock
    initSPI(ui32SysClock);
    // Starting the OLED Task: receiving char and displaying
    setup_OLED_task("Startup_Oled", 5);
    System_printf("Created Startup Oled Task\n");
    // Starting the UART Task: sending char and displaying
    setup_UART_Task("UART Task", 10);
    System_printf("Created Startup UART Task\n");
    System_flush();
    /* Start BIOS */
    BIOS_start();
}

/**** The code below is only for Instrumentation purposes! ****/

/* The redefinition of ti_uia_runtime_LogSnapshot_writeNameOfReference
 * is necessary due to bug UIA-23 fixed in UIA 2.00.06.52, cf.
 * http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/uia/2_00_06_52/exports/docs/uia_2_00_06_52_release_notes.html
 */
#undef ti_uia_runtime_LogSnapshot_writeNameOfReference
#define ti_uia_runtime_LogSnapshot_writeNameOfReference(refId, fmt, startAdrs, lengthInMAUs) \
        ( \
                (ti_uia_runtime_LogSnapshot_putMemoryRange(ti_uia_events_UIASnapshot_nameOfReference, Module__MID, \
                                                           (IArg)refId,(IArg)__FILE__,(IArg)__LINE__, \
                                                           (IArg)fmt, (IArg)startAdrs, (IArg)lengthInMAUs)) \
        )

/* Log the task name whenever a task is created.
 * This works around a limitation of UIA where tasks sharing a "main"
 * function do not show up separately in the execution analyzer, cf.
 * http://processors.wiki.ti.com/index.php/System_Analyzer_Tutorial_3A#Going_Further:_How_to_have_Analysis_tables_and_graphs_display_the_names_of_tasks_created_at_run_time
 */
#include <string.h>
Void tskCreateHook(Task_Handle hTask, Error_Block *eb) {
    String name = Task_Handle_name(hTask);
    LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
    ti_uia_runtime_LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
}
