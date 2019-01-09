/*!
 * \file broker.c
 * \brief Broker service between the input and output module
 * \date Jan 9, 2019
 * \author Valentin Platzgummer ic17b096
 */

// ----------------------------------------------------------------------------- includes ---
#include "local_inc/broker.h"

/* Function: Broker interacts direct with UART (bidirectional)
 * and takes from input from the Input-Module
 * converts it from Int (or float) to char
 * and sends it over to the Output Module
 * It should be implemented a testing menuto provide testing functionality
 * for each module individual
 */

//! \addtogroup group_comm
//! @{

// ---------------------------------------------------------------------------- functions ---
extern void Broker_task(void);

// ----------------------------------------------------------------------- implementation ---
/*!
 * \brief create a new Broker Task and initialize it with the necessary parameters.
 * \param name xdc_String, identifying name of the task
 * \param priority uitn8_t initial priority of the task (1-15) 15 is highest priority
 */
extern void setup_Broker_task(xdc_String name, uint8_t priority) {
    Task_Params taskLedParams;
    Task_Handle taskLed;
    Error_Block eb;
    /* Create OLED startup task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.instance->name = name;
    taskLedParams.stackSize = 1024; /* stack in bytes */
    taskLedParams.priority = priority; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskLed = Task_create((Task_FuncPtr)Broker_task, &taskLedParams, &eb);
    if (taskLed == NULL) {
        System_abort("TaskLed create failed");
    }
}
/*!
 * \brief
 * broker task receives input from interprocess signal (semaphore) from UART or from
 * the input module converts the input to ascii format and sends it to the OLED function
 * to display.
 */
extern void Broker_task(void) {
    Error_Block er;
    Semaphore_Params output_sem_parms;
    Semaphore_Params_init(&output_sem_parms);

    Semaphore_create(0, &output_sem_parms, &er);

    while (1) {
        // receive char from UART task
        // Semaphore_pend(sem, BIOS_WAIT_FOREVER);
        //System_printf("Char from UART: %c", uartChar);
        // Convert input ...


        // post the semaphore for the OLED Task
        Semaphore_post(output_sem);
    }
}

//! @}
