/*! \file oled_display.c
 *  \date Dec 27, 2018
 *  \author Valentin Platzgummer - ic17b096
 *  \brief all necessary drivers for the OLED 96x96 and the task to use it in the main thread
 */

// ----------------------------------------------------------------------------- includes ---
#include "local_inc/oled_display.h"
#include "local_inc/UART_Task.h"
#include "local_inc/oled_hal.h"

// ------------------------------------------------------------------------------ defines ---
#define LEFT_MARGIN 20
#define RIGHT_MARGIN 20
// ------------------------------------------------------------------------------ globals ---
static volatile point startpointFirstRow;
static volatile point startpointSecondRow;
static volatile point currentPosition;
static volatile bool isScrolling;
static fontContainer font;
// ---------------------------------------------------------------------------- functions ---
static void OLED_Fxn(void);
static bool isPrintableChar (char c, color24 bgcolor);
static calculateCoordinates(void);
static void updateCurrentPosition(void);
static void switchRow(void);

// ----------------------------------------------------------------------- implementation ---
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
    Task_sleep(5000);
    createBackgroundFromColor(blueColor);
    calculateCoordinates();
    bool sem_timeout;

    initializeFont(&font, 1);

    while (1) {
        sem_timeout = Semaphore_pend(sem, BIOS_WAIT_FOREVER);
        char c = charContainer;
        if (!sem_timeout) {
            System_printf("Semaphore has time out.\n");
            System_flush();
        }

        if (isPrintableChar(c, blueColor)) {
            drawChar(c, &font, whiteColor, blueColor, currentPosition);
            currentPosition.x += font.fontSpacing; // Note text is drawing backwards
        }
    }
}
/*! \fn calculateCoordinates
 *  \brief Calculates the upper and lower margin of the 2 rows centered
 *  Assuming always 2 text rows being always centered
 */
static calculateCoordinates(void) {
    uint8_t margin = (OLED_DISPLAY_Y_MAX - (2* font.fontHeight + font.fontHeight / 4)) / 2;
    startpointFirstRow.x = LEFT_MARGIN;
    startpointFirstRow.y = margin;
    startpointSecondRow.x = LEFT_MARGIN;
    startpointSecondRow.y = OLED_DISPLAY_Y_MAX - (margin + font.fontHeight);
    currentPosition.x = startpointFirstRow.x;
    currentPosition.y = startpointFirstRow.y;
    System_printf("upper margin: %u lower margin: %u\n",OLED_DISPLAY_Y_MAX - (margin + font.fontHeight), margin);
    System_flush();
}
/*! \fn updateCurrentPosition
 * \brief calculate the line break.
 * Line break will be done if a next char will not fit into the row.
 */
static void updateCurrentPosition(void) {
    if ((currentPosition.x + font.fontWidth) > (OLED_DISPLAY_Y_MAX)) {
        switchRow();
    }
}
/*! \fn isPrintableChar
 * \brief function checks chars if they are printable chars to put on screen or control codes
 * if char is a printable char, the line break will called. Depending on which control code is given,
 * different actions are taken. in case '\b' the cursor moves on step backwards and a plain background is drawn.
 * in chase '\n' switches the current row. in case '\t' down scrolling is switched between enable/disable
 * \param c char, the given char to examined
 * \param bgcolor color24, the given background color for '\b' operation
 * \return true, if char is printable character, false in other case.
 */
static bool isPrintableChar (char c, color24 bgcolor) {
    if (c > 19) {
        updateCurrentPosition();  // Check position only if real char to draw
        return true;
    }
    // switch upon incoming control code. more control codes are possible.
    switch (c) {
    // move cursor back by 1 char + char spacing and draw space in case '\b'
    case 8:
        currentPosition.x -= font.fontSpacing; // Spacing is font width + extra space for the next char
        drawChar(0x20, &font, bgcolor, bgcolor, currentPosition);  // draw space without char feed
        break;
     // enable/ disable screen saver scrolling by pressing '\t'
    case 9:
        isScrolling ^= 1;
        toggleDownScroll(isScrolling);
        break;
     // insert line break if code is '\n'
    case 10:
    case 13:
        switchRow();
        break;
    }
    return false;
}
/*! \fn switchRow
 * \brief switch the current working next row to the following
 */
static void switchRow(void) {
    currentPosition.x = LEFT_MARGIN;
    currentPosition.y = (currentPosition.y == startpointFirstRow.y)? startpointSecondRow.y : startpointFirstRow.y;
}
