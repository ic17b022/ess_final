/*! \file oled_display.c
 *  \date Dec 27, 2018
 *  \author Valentin Platzgummer
 *  \brief all necessary drivers for the OLED 96x96 and the task to use it in the main thread
 */

// ------------------------------------ includes ---
#include "local_inc/oled_display.h"
#include "local_inc/UART_Task.h"
#include "local_inc/oled_hal.h"

// -------------------------------------- defines ---
#define LEFT_MARGIN 20
#define RIGHT_MARGIN 20
// -------------------------------------- globals ---
static point startpointFirstRow;
static point startpointSecondRow;
static point currentPosition;
// ------------------------------------ functions ---
static void OLED_Fxn(void);
static bool interpretChar (char c);

/* \fn setup_OLED_task
 * \brief create a new OLED Task and initialize it with the necessary parameters.
 * \param name xdc_String, identifying name of the task
 * \param priority uitn8_t initial priority of the task (1-15) 15 is highest priority
 */
extern void setup_OLED_task(xdc_String name, uint8_t priority) {
    Task_Params taskLedParams;
    Task_Handle taskLed;
    Error_Block eb;
    /* Create OLED startup task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.instance->name = name;
    taskLedParams.stackSize = 1024; /* stack in bytes */
    taskLedParams.priority = priority; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskLed = Task_create((Task_FuncPtr)OLED_Fxn, &taskLedParams, &eb);
    if (taskLed == NULL) {
        System_abort("TaskLed create failed");
    }
}
static void OLED_Fxn(void) {
    // power on OLED
    OLED_power_on();
    createBackgroundFromImage(logo_image);
    createBackgroundFromColor(0x00FF00);
    calculateCoordinates();
    bool sem_timeout;

    while (1) {
        sem_timeout = Semaphore_pend(sem, BIOS_WAIT_FOREVER);
        char c = charContainer;
        if (!sem_timeout) {
            System_printf("Semaphore has time out.\n");
            System_flush();
        }
        if (interpretChar(c)) {
            drawChar(c, 0xFF0000, 0x00FF00, currentPosition);
            currentPosition.x += FONT_SPACING; // Note text is drawing backwards
        }
    }
}
static calculateCoordinates(void) {
    uint8_t margin = (OLED_DISPLAY_Y_MAX - (2* FONT_HEIGHT + FONT_HEIGHT / 4));
    startpointSecondRow.y = margin;
    startpointSecondRow.x = LEFT_MARGIN;
    startpointFirstRow.y = OLED_DISPLAY_Y_MAX - (margin + FONT_HEIGHT);
    startpointFirstRow.x = LEFT_MARGIN;
    currentPosition.x = startpointFirstRow.x;
    currentPosition.y = startpointFirstRow.y;
}
static void updateCurrentPosition(void) {
    if ((currentPosition.x + FONT_WIDTH) > (OLED_DISPLAY_Y_MAX - RIGHT_MARGIN)) {
        currentPosition.x = LEFT_MARGIN;
    }
}
static bool interpretChar (char c) {
    updateCurrentPosition();
    if (c > 19) {
        return true;
    }
    //
    switch (c) {
    // insert line break if code is '\n'
    case 10:
    case 13:
        currentPosition.x = LEFT_MARGIN;
        currentPosition.y = (currentPosition.y == startpointFirstRow.y)? startpointSecondRow.y : startpointFirstRow.y;
        break;
        // switch back 1 char and draw space in case 'del'
    case 8:
    case 127:
        currentPosition.x -= FONT_WIDTH;
        c = 0x20;
        return true;
        break;
    }
    return false;

}
