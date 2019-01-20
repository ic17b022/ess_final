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
static void initializeMailboxes(void);
// ---------------------------------------------------------------------------- globals -----
static uint8_t testcase;
static bool isChanged;
// ----------------------------------------------------------------------- implementation ---
/*!
 * \brief create a new Broker Task and initialize it with the necessary parameters.
 * \param name xdc_String, identifying name of the task
 * \param priority uint8_t initial priority of the task (1-15) 15 is highest priority
 */
extern void setup_Broker_task(xdc_String name, uint8_t priority)
{
    Task_Params taskLedParams;
    Task_Handle taskLed;
    Error_Block eb;
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.instance->name = name;
    taskLedParams.stackSize = 1024; /* stack in bytes */
    taskLedParams.priority = priority; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskLed = Task_create((Task_FuncPtr) Broker_task, &taskLedParams, &eb);
    if (taskLed == NULL)
    {
        System_abort("Brokertask create failed");
    }
}
/*!
 * \brief
 * broker task receives input from interprocess signal (semaphore) from UART or from
 * the input module converts the input to ascii format and sends it to the OLED function
 * to display.
 */
extern void Broker_task(void)
{
    initializeMailboxes();
    uint8_t UART_read;
    uint8_t temp;

    while (1)
    {
        if(Mailbox_pend(brokerRead, &UART_read, 1))
        {
            System_printf("gelesen1: %c\n", UART_read);
            System_flush();

            if (UART_read == '#')
            {
                Mailbox_pend(brokerRead, &UART_read, BIOS_WAIT_FOREVER);
                System_printf("gelesen2: %c\n", UART_read);
                System_flush();
                if (UART_read >= '0' && UART_read <= '4')
                {
                    testcase = UART_read - '0';
                    isChanged = true;
                    outputTestcaseChange(testcase);
                }
            } // Testcase 2 routes the UART to the output, User can write to OLED
            else if (testcase == 2)
            {
                Mailbox_post(oledMailbox, &UART_read, BIOS_WAIT_FOREVER);
            }
            // Testcase 3 swich the oled off
            else if (testcase == 3)
            {
                OLED_toggle_Display_on_off();
            }
        }

        // Testcase 0 is normal mode input module get routed to output module
        if (testcase == 0)
        {
            Mailbox_pend(heartrateMailbox, &temp, BIOS_WAIT_FOREVER);
            Mailbox_post(oledMailbox, &temp, BIOS_NO_WAIT);
        }
        // Testcase 1 is test input in which form whatsoever
        else if (testcase == 1)
        {
            char heartrateString[4];

            Mailbox_pend(heartrateMailbox, &temp, BIOS_WAIT_FOREVER);
            sprintf(heartrateString, "%03u", temp);
            Mailbox_post(brokerWrite, &heartrateString[0], BIOS_WAIT_FOREVER);
            Mailbox_post(brokerWrite, &heartrateString[1], BIOS_WAIT_FOREVER);
            Mailbox_post(brokerWrite, &heartrateString[2], BIOS_WAIT_FOREVER);
        }
        else if (testcase == 4)
        {
            System_printf("Draw diagram");
            System_flush();
        }
    }
}

/*!
 * \brief initialize all used mailboxes
 *
 *
 */
static void initializeMailboxes(void){
    Mailbox_Params params;
    Error_Block eb;

    Mailbox_Params_init(&params);
    Error_init(&eb);

    heartrateMailbox = Mailbox_create(sizeof(uint8_t), 1, &params, &eb);
    oledMailbox = Mailbox_create(sizeof(uint8_t), 1, &params, &eb);
    brokerWrite = Mailbox_create(sizeof(uint8_t), 1, &params, &eb);
    brokerRead = Mailbox_create(sizeof(uint8_t), 1, &params, &eb);
}

/*!
 * \brief convert ingoing integer to char with equivalent ascii
 * \param inValue integer to be converted. Note max 3 digits get used (uint8_t)
 */

/*!
 * \brief get the actual testcase value
 * 0 ... normal mode
 * 1 ... testing input module
 * 2 ... testing output module
 * 3 ... display off/ on
 */
uint8_t getTestcase(void)
{
    return testcase;
}
/*!
 * \brief reset the static variable isChanged from outside.
 * Necessary because oled_display.c needs to reset the value
 */
void resetChanged(void)
{
    isChanged = false;
}
/*!
 * \ get the actual value of the changing testcase status
 */
bool getChanged(void)
{
    return isChanged;
}

//! @}
