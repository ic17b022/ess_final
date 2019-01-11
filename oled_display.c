/*! \file oled_display.c
 *  \date Dec 27, 2018
 *  \author Valentin Platzgummer - ic17b096
 *  \brief all necessary drivers for the OLED 96x96 and the task to use it in the main thread
 */

// ----------------------------------------------------------------------------- includes ---
#include "local_inc/oled_display.h"
#include "local_inc/UART_Task.h"
#include "local_inc/oled_hal.h"
#include "local_inc/broker.h"


//! \addtogroup group_oled_app
//! @{
// ------------------------------------------------------------------------------ defines ---
//! \brief left margin of text window
#define LEFT_MARGIN 20
//! \brief right margin of text window
#define RIGHT_MARGIN 20
//! \brief top margin of text window
#define UPPER_MARGIN 20
//! \brief bottom margin of text window
#define LOWER_MARGIN 20
// ------------------------------------------------------------------------------ globals ---
//! \brief contains the actual position of the cursor in window
static volatile point currentPosition;
//! \brief wheter sreensaver scrolling is enabled or disabled
static volatile bool isScrolling;
//! \brief used fontsize for next char (1-3)
static volatile uint8_t fontsize;
//! \brief used font needed for calculation purposes
static fontContainer font;
// ---------------------------------------------------------------------------- functions ---
static void OLED_Fxn(void);
static void putValueFromInput(char *inputChar, char *title, char *status);
static bool isPrintableChar (char c, color24 bgcolor);
static void initializeCurrentPoint(void);
static void updateCurrentPosition(void);
static void switchRow(void);

// ----------------------------------------------------------------------- implementation ---
/*!
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
/*!
 * \brief OLED function enables the OLED Display, creates a given background
 * In the working while loop displays all incoming char to the display
 */
static void OLED_Fxn(void) {
    // power on OLED
    OLED_power_on();
    createBackgroundFromImage(cool_image);
    Task_sleep(3000);
    fontsize = 2;
    createBackgroundFromColor(blueColor);
    initializeCurrentPoint();
    bool sem_timeout;

    initializeFont(&font, fontsize);

    while (1) {

        // sem_timeout = Semaphore_pend(sem, BIOS_WAIT_FOREVER);
        //        char c = charContainer;
        sem_timeout = Semaphore_pend(output_sem, BIOS_WAIT_FOREVER);
        char c = oledChar;
        if (!sem_timeout) {
            System_printf("Semaphore has time out.\n");
            System_flush();
        }
        uint8_t testcase = getTestcase();
        if (testcase == 0) {
            putValueFromInput("128\0", "Rate\0", "OK\0");
        } else if (testcase == 2) {
            if (isPrintableChar(c, blueColor)) {
                drawChar(c, &font, whiteColor, blueColor, currentPosition);
                currentPosition.x += font.fontSpacing; // Note text is drawing backwards
            }
        }
    }
}

static void putValueFromInput(char *inputChar, char *title, char *status) {
    // draw header
    initializeFont(&font, 2);
    currentPosition.x = font.fontWidth + 4;
    currentPosition.y = 4;
    uint8_t i = 0;
    while (title[i] != 0) {
        drawChar(title[i++], &font, whiteColor, blueColor, currentPosition);
        currentPosition.x += font.fontSpacing; // Note text is drawing backwards
    }
    currentPosition.y += font.fontHeight;

    // draw input value
    initializeFont(&font, 3);
    i= 0;
    currentPosition.x = font.fontWidth + 4;
    while (inputChar[i] != 0) {
        drawChar(inputChar[i++], &font, whiteColor, blueColor, currentPosition);
        currentPosition.x += font.fontSpacing;

    }
    //draw status
    initializeFont(&font, 1);
    i= 0;
    currentPosition.x = font.fontWidth + 4;
    currentPosition.y = OLED_DISPLAY_Y_MAX - font.fontHeight;
    while (status[i] != 0) {
        drawChar(status[i++], &font, whiteColor, blueColor, currentPosition);
        currentPosition.x += font.fontSpacing;
    }
}
/*!
 *  \brief set the initial starting point to the upper left corner
 *  \todo find out where is the starting point of the chars lower right, (lower left?)
 */
static void initializeCurrentPoint(void) {
    currentPosition.x = 0;
    currentPosition.y = 0;
}
/*!
 * \brief calculate the line break.
 * Line break will be done if a next char will not fit into the row.
 */
static void updateCurrentPosition(void) {
    if ((currentPosition.x + font.fontWidth) > (OLED_DISPLAY_X_MAX)) {
        switchRow();
    }
}
/*!
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
/*!
 * \brief switch the current working next row to the following
 */
static void switchRow(void) {
    currentPosition.x = LEFT_MARGIN;
    if (currentPosition.y + font.fontHeading > OLED_DISPLAY_Y_MAX) {
        currentPosition.y = UPPER_MARGIN;
    } else {
        currentPosition.y += font.fontHeading;
    }
}
// Close Doxygen group
//! @}
