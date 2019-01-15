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
#define LEFT_MARGIN 4
//! \brief right margin of text window
#define RIGHT_MARGIN 4
//! \brief top margin of text window
#define UPPER_MARGIN 4
//! \brief bottom margin of text window
#define LOWER_MARGIN 4
// ------------------------------------------------------------------------------ globals ---
//! \brief contains the actual position of the cursor in window
static volatile point currentPosition;
//! \brief wheter sreensaver scrolling is enabled or disabled
static volatile bool isScrolling;
//! \brief used fontsize for next char (1-3)
static volatile uint8_t fontsize;
//! \brief used font needed for calculation purposes
static fontContainer font;
static color24 charCol;
static color24 bgcol;

// ---------------------------------------------------------------------------- functions ---
static void OLED_Fxn(void);
static void putValueFromInput(char *inputChar, char *title, char *status);
static bool isPrintableChar (char c);
static void initializeCurrentPoint(void);
static void updateCurrentPosition(void);
static void switchRow(void);
static void setCursor(void);
static void deleteCharAtCurrentPoint();
static bool isPointUpperLeft(point current);
static bool isPointPrelastRow (point current);
static void scrollRow (point current);

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
    fontsize = 1;
    bgcol = blueColor;
    charCol = whiteColor;
    createBackgroundFromColor(bgcol);
    bool sem_timeout;

    initializeFont(&font, fontsize);
    initializeCurrentPoint();

    while (1) {

        // here code for calculating cursor position and initialize the scrolling functionality.
        scrollRow(currentPosition);
        // sem_timeout = Semaphore_pend(sem, BIOS_WAIT_FOREVER);
        //        char c = charContainer;
        sem_timeout = Semaphore_pend(output_sem, BIOS_WAIT_FOREVER);
        char c = oledChar;
        if (!sem_timeout) {
            System_printf("Semaphore has time out.\n");
            System_flush();
        }
        uint8_t testcase = getTestcase();
        bool isChanged = getChanged();
        // if testcase change occcured, clear screen
        if (isChanged == true) {
            createBackgroundFromColor(bgcol);
            initializeCurrentPoint();
            setChanged(false);
        }
        if (testcase == 0) {
            putValueFromInput("128\0", "\3 Rate\0", "Stat: OK\0");
        } else if (testcase == 2) {
            if (isPrintableChar(c)) {
                // here code for calculating cursor position and initialize the scrolling functionality.
                // scrollRow(currentPosition);
                drawChar(c, &font, charCol, bgcol, currentPosition);
                currentPosition.x += font.fontSpacing; // Note text is drawing backwards
                setCursor();
            }
        }
    }
}
/*!
 * \brief set a cursor of type '_' at the current position to give user a visual feedback
 */
static void setCursor(void) {
    updateCurrentPosition();
    drawChar('_', &font, charCol, bgcol, currentPosition);
}
/*!
 * \brief output the incoming value in a formatted form.
 * \param inputChar value (given as 0-terminated c-String) heartrate
 * \param title header for the formatted output. (c-String 0-terminated)
 * \param status a fedback to the user about the status of the measurement (0-terminated)
 */
static void putValueFromInput(char *inputChar, char *title, char *status) {
    // draw header
    initializeFont(&font, 1);
    currentPosition.x = font.fontWidth + 4;
    currentPosition.y = 4;
    uint8_t i = 0;
    while (title[i] != 0) {
        drawChar(title[i++], &font, charCol, bgcol, currentPosition);
        currentPosition.x += font.fontSpacing; // Note text is drawing backwards
    }
    currentPosition.y += font.fontHeight;

    // draw input value
    initializeFont(&font, 2);
    i= 0;
    currentPosition.x = font.fontWidth + 4;
    while (inputChar[i] != 0) {
        drawChar(inputChar[i++], &font, charCol, bgcol, currentPosition);
        currentPosition.x += font.fontSpacing;

    }
    //draw status
    initializeFont(&font, 0);
    i= 0;
    currentPosition.x = font.fontWidth + 4;
    currentPosition.y = OLED_DISPLAY_Y_MAX - font.fontHeight;
    while (status[i] != 0) {
        drawChar(status[i++], &font, charCol, bgcol, currentPosition);
        currentPosition.x += font.fontSpacing;
    }
}
/*!
 *  \brief set the initial starting point to the upper left corner
 *  \todo find out where is the starting point of the chars lower right, (lower left?)
 */
static void initializeCurrentPoint(void) {
    currentPosition.x = font.fontWidth + LEFT_MARGIN;
    currentPosition.y = UPPER_MARGIN;
}
/*!
 * \brief calculate the line break.
 * Line break will be done if a next char will not fit into the row.
 */
static void updateCurrentPosition(void) {
    if (currentPosition.x > OLED_DISPLAY_X_MAX ) {
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
static bool isPrintableChar (char c) {
    if (c > 19) {
        updateCurrentPosition();  // Check position only if real char to draw
        return true;
    }
    // delete cursor, because char is not a printable one
    drawChar(0x20, &font, bgcol, bgcol, currentPosition);

    // switch upon incoming control code. more control codes are possible.
    switch (c) {
    // move cursor back by 1 char + char spacing and draw space in case '\b'
    case 8:
        // if on upper left, stay there
        if (isPointUpperLeft(currentPosition))
            initializeCurrentPoint();
        else
            deleteCharAtCurrentPoint();
        break;
        // increases font size
    case 9:
        fontsize = ++fontsize % 3;
        initializeFont(&font, fontsize);
        // clear screen
        createBackgroundFromColor(bgcol);
        break;
        // insert line break if code is '\n'
    case 10:
    case 13:
        switchRow();
        break;
    }
    // now set cursor for all.
    setCursor();
    return false;
}

/*!
 * \brief delete the character at the current point and step on character space back
 * Deleting is implemented to work over multiple lines, the coordinates get calculated accordingly
 */
static void deleteCharAtCurrentPoint() {
    // delete cursor
    drawChar(0x20, &font, bgcol, bgcol, currentPosition);
    // is cursor at begin of display?
    if ((currentPosition.x - font.fontSpacing) <  LEFT_MARGIN) {
        currentPosition.y -= font.fontHeading; // jump 1 row back
        // set cursor at last position of this row
        currentPosition.x = OLED_DISPLAY_X_MAX - ((OLED_DISPLAY_X_MAX - LEFT_MARGIN) % font.fontSpacing) - (font.fontSpacing - font.fontWidth);
        System_printf("current_x: %u\n", currentPosition.x);
        System_flush();
        drawChar(0x20, &font, bgcol, bgcol, currentPosition);  // draw space without char feed
    } else {
        currentPosition.x -= font.fontSpacing; // Spacing is font width + extra space for the next char
        drawChar(0x20, &font, bgcol, bgcol, currentPosition);  // draw space without char feed
    }
    setCursor();
}
/*!
 * \brief switch the current working next row to the following
 */
static void switchRow(void) {
    currentPosition.x = font.fontWidth + LEFT_MARGIN;
    // is end of last line? jump to upper left.
    if (currentPosition.y + 2*font.fontHeading > OLED_DISPLAY_Y_MAX) {
        currentPosition.y = UPPER_MARGIN;
    } else {
        currentPosition.y += font.fontHeading;
    }
}
/*! \brief evaluate if current point is 1 character space before first point in display.
 * Take the given margins in account
 * \param current current point to evaluate
 * \return true if point is first point, false in all other cases
 */
static bool isPointUpperLeft(point current) {
    if ((current.x - font.fontSpacing <= LEFT_MARGIN) && (current.y <= UPPER_MARGIN))
        return true;
    return false;
}
// this function is useful to detect end of display in order to scroll down, while typing is still ongoing.
static bool isPointPrelastRow (point current) {
    if (current.y + 3 * font.fontSpacing >= OLED_DISPLAY_Y_MAX)
        return true;
    return false;
}
static void scrollRow (point current) {
    if (isPointPrelastRow(currentPosition)) {
        isScrolling = true;
    }
    else
        isScrolling = false;
    System_printf("cursor at x: %u, y: %u\n", currentPosition.x, currentPosition.y);
    toggleUpScroll(isScrolling);
}
// Close Doxygen group
//! @}
