/*!
 * \file UART_Task.c
 * \brief UART Task detects keystroke and sends the detected chars to the OLED Task for displaying
 * \author Valentin Platzgummer ic17b096
 * \date Jan, 02 2019
 */
// ----------------------------------------------------------------------------- includes ---
#include "local_inc/common.h"
#include "local_inc/UART_Task.h"
//! \addtogroup group_comm
//! @{
// ------------------------------------------------------------------------------ globals ---
Semaphore_Handle sem;
uint8_t input;
// ---------------------------------------------------------------------------- functions ---
static void outputMenu(void);
static void UARTreadCallback(UART_Handle, void *buf, size_t count);

// ----------------------------------------------------------------------- implementation ---

/*!
 * \brief UART Task receives keystrokes from an attached Terminal via UART
 * The keystroke get tested, and if the comply with the valid chars the char get appended to
 * the global char buffer. And a semaphore get posted
 */
void UARTFxn(UArg arg0, UArg arg1)
{
    Error_Block er;
    Semaphore_Params params;
    Semaphore_Params_init(&params);

    sem = Semaphore_create(0, &params, &er);

    UART_Handle uart;
    UART_Params uartParams;
    const char echoPrompt[] = "\fEchoing characters:\r\n";

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_ON;
    uartParams.baudRate = 9600;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = UARTreadCallback;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    outputMenu();

    UART_read(uart, &input, 1);
    /* Loop forever echoing */
    while (1) {

        if(Semaphore_pend(sem, 1))
        {
            // Keystroke in the valid region, send it to broker otherwise just ignore it
            if (input >= 0x08 && input <= 0x7F) {
                // inputChar is the shared memory in broker to wait for any input
                Mailbox_post(brokerRead, &input, BIOS_WAIT_FOREVER);

            }

            UART_read(uart, &input, 1);
        }



        if(Mailbox_pend(brokerWrite, &UARTwrite, 1))
            UART_write(uart, &UARTwrite, 1);
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
/*!
 * \brief print principal menu to screen
 */
static void outputMenu(void) {
    System_printf("Menu list:\n");
    System_printf("\n#0 Heart rate (Input) In -> OLED C (Output) out\n");
    System_printf("#1 Heart rate (Input) In -> UART out\n");
    System_printf("#2 UART In -> OLED C (Output) out\n");
    System_printf("#3 Toggle OLED- Display on/ off\n");
    System_printf("Select needed by providing leading '#' before number.\n");
    System_flush();
}

void outputTestcaseChange(uint8_t testcase){
    System_printf("New Testcase: %u\n", testcase);
    System_flush();
}

static void UARTreadCallback(UART_Handle uart, void *buf, size_t count){
    input = *((uint8_t*) buf);

    Semaphore_post(sem);
}

// End Doxygen group
//! @}
