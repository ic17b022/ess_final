/*!
 * \file UART_Task.c
 * \brief UART Task detects keystroke and sends the detected chars to the OLED Task for displaying
 * \author Valentin Platzgummer ic17b096
 * \date Jan, 02 2019
 */
// ----------------------------------------------------------------------------- includes ---
#include "local_inc/common.h"
#include "local_inc/UART_Task.h"
#include "local_inc/broker.h"
//! \addtogroup group_comm
//! @{
// ------------------------------------------------------------------------------ globals ---
//! \brief stores the test variante selected by menu
static uint8_t testcase;
static bool isChanged;

// ---------------------------------------------------------------------------- functions ---
static void outputMenu(void);
/*!
 * \brief UART Task receives keystrokes from an attached Terminal via UART
 * The keystroke get tested, and if the comply with the valid chars the char get appended to
 * the global char buffer. And a semaphore get posted
 */

// ----------------------------------------------------------------------- implementation ---
void UARTFxn(UArg arg0, UArg arg1)
{
    UART_Handle uart;
    UART_Params uartParams;
    const char echoPrompt[] = "\fEchoing characters:\r\n";

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    Error_Block er;
    Semaphore_Params params;
    Semaphore_Params_init(&params);

    // create a Semaphore for each individual char
    sem = Semaphore_create(0, &params, &er);
    if (sem == NULL) {
        System_abort("Error creating the Semaphore");
    }
    // initialize Testcase with 0 i.e. 'Normal mode'
    testcase = 0;
    outputMenu();
    /* Loop forever echoing */
    while (1) {
        char input, followChar;
        UART_read(uart, &input, 1);
        UART_write(uart, &input, 1); // Remove this line to stop echoing!
        // if input is '#' menu is selected. use next char to determine which selection is taken
        if (input == '#') {
            UART_read(uart, &followChar, 1);

            switch (followChar) {
            case '0':
            case '1':
            case '2':
            case '3':
                testcase = followChar - '0';
                isChanged = true;
                System_printf("New Testcase: %u\n", testcase);
                System_flush();
                break;
            default:
                // pipe the input to standard out
                break;
            }
        }
        // Keystroke in the valid region, send it to the oled_display.c
        if (input >= 0x08 && input <= 0x7F) {
            // inputChar is the shared memory in broker to wait for any input
            inputChar = input;
            Semaphore_post(input_sem); // Semaphore get posted to broker
        }
        UART_write(uart, &input, 1); // Remove this line to stop echoing!
    }

}
/*!
 * \brief create a new UART Task and initialize it with the necessary parameters.
 * \param name xdc_String, identifying name of the task
 * \param priority uitn8_t initial priority of the task (1-15) 15 is highest priority
 */
void setup_UART_Task(xdc_String name, uint8_t priority)
{
    Task_Params taskUARTParams;
    Task_Handle taskUART;
    Error_Block eb;
    /* Enable and configure the peripherals used by the UART0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UART_init();

    /* Setup PortN LED1 activity signaling */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    Error_init(&eb);
    Task_Params_init(&taskUARTParams);
    taskUARTParams.instance->name = name;   // Tasks name, easier to debug
    taskUARTParams.stackSize = 1024; /* stack in bytes */
    taskUARTParams.priority = priority; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    //taskUARTParams.arg0 = (UArg)event_descr;
    taskUART = Task_create((Task_FuncPtr)UARTFxn, &taskUARTParams, &eb);
    if (taskUART == NULL) {
        System_abort("TaskUART create failed");
    }
}
static void outputMenu(void) {
    System_printf("Menu list:\n");
    System_printf("\n#0 Heartrate (Input) In -> OLED C (Output) out\n");
    System_printf("#1 Heartrate (Input) In -> UART out\n");
    System_printf("#2 UART In -> OLED C (Output) out\n");
    System_printf("Select needed by providing leading '#' before number.\n");
    System_flush();
}
uint8_t getTestcase(void) {
    return testcase;
}
void setChanged(bool _isChanged) {
    isChanged = _isChanged;
}
bool getChanged(void) {
    return isChanged;
}
// End Doxygen group
//! @}
