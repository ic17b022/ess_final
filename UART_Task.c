/*!
 * \file UART_Task.c
 * \author: Valentin Platzgummer ic17b096
 * \date: Jan, 02 2019
 */
#include "local_inc/UART_Task.h"

/*! \fn UARTFxn
 * \brief UART Task receives keystrokes from an attached Terminal via UART
 * The keystroke get tested, and if the comply with the valid chars the char get appended to
 * the global char buffer. And a semaphore get posted
 */
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

    //create Mailbox with max 6 chars
    sem = Semaphore_create(0, &params, &er);
    if (sem == NULL) {
        System_abort("Error creating the Semaphore");
    }
    /* Loop forever echoing */
    while (1) {
        char input;
        UART_read(uart, &input, 1);
        // Keystroke in the valid region, send it to the oled_display.c
        if (input >= 0x20 && input <= 0x7E) {
            charContainer= input;
            Semaphore_post(sem);
        }
        UART_write(uart, &input, 1); // Remove this line to stop echoing!
    }
}
/*! \fn setup_UART_Task
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
