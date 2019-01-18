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
static void initializeSemaphore(void);
static void convertDataToChar(uint8_t inValue, char *outchar);

// ----------------------------------------------------------------------- implementation ---
/*!
 * \brief create a new Broker Task and initialize it with the necessary parameters.
 * \param name xdc_String, identifying name of the task
 * \param priority uint8_t initial priority of the task (1-15) 15 is highest priority
 */
extern void setup_Broker_task(xdc_String name, uint8_t priority) {
    Task_Params taskLedParams;
    Task_Handle taskLed;
    Error_Block eb;
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
    initializeSemaphore();

    while (1) {
        // wait for input, no matter from where
        Semaphore_pend(input_sem, BIOS_WAIT_FOREVER);
        // Testcase 0 is normal mode input module get routed to output module
        if (getTestcase() == 0) {
            inputChar = 71;
            convertDataToChar(inputChar, &oledChar);
        }
        // Testcase 1 is test input in which form whatsoever
        else if (getTestcase() == 1) {
            System_printf("Do the input test");
            System_flush();
        }
        // Testcase 2 routes the UART to the output, User can write to OLED
        else if (getTestcase() == 2) {
            oledChar[0] = inputChar;
            oledChar[1] = '\0';
        }
        // Testcase 3 swich the oled off
        else if (getTestcase() == 3) {
            OLED_toggle_Display_on_off();
        }
        else if (getTestcase() == 4) {
            System_printf("Draw diagram");
            System_flush();
        }
        //
        // post the semaphore for the OLED Task
        Semaphore_post(output_sem);
    }
}
/*!
 * \brief initialize both used semaphores
 * output_sem sempahore used to synchronize between broker and oled module
 * uart_sem semaphore used to synchronize between broker and uart
 */
static void initializeSemaphore(void) {
    Error_Block er;
    Semaphore_Params output_sem_parms;
    Semaphore_Params_init(&output_sem_parms);

    output_sem = Semaphore_create(0, &output_sem_parms, &er);
    if (output_sem == NULL) {
        System_printf("Failed creating output Semaphore\n");
        System_flush();
    }
    Semaphore_Params input_sem_parms;
    Semaphore_Params_init(&input_sem_parms);

    input_sem =  Semaphore_create(0, &input_sem_parms, &er);
    if (input_sem == NULL) {
        System_printf("Failed creating output Semaphore\n");
        System_flush();
    }
}
/*!
 * \brief convert ingoing integer to char with equivalent ascii
 * \param inValue integer to be converted. Note max 3 digits get used (uint8_t)
 */

// Compiler prints a waring because snprintf is not declared in c89 but in c99
// see https://e2e.ti.com/support/tools/ccs/f/81/t/123841?tisearch=e2e-sitesearch&keymatch=snprintf%20header
static void convertDataToChar(uint8_t inValue, char *outchar) {
    uint8_t result = snprintf(outchar, 4, "%u\0", inValue);
    if (result == 0) {
        System_printf("Not written any pulse value.\n");
        System_flush();
    }
}

//! @}
