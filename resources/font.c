/*! \file font.c
 * \brief initialization code for every font, all data are stored in the struct.
 *
 * \author Valentin Platzgummer - ic17b096
 * \date Jan 1, 2019
 */

#include "font.h"
#include "font_array.h"
//! \addtogroup group_oled_res
//! @{
/*!
 * \brief initializes the fontContainer struct with all needed data for any given font
 * \param fc fontContainer, pointer to a fontContainer struct to be initialized
 * \param size unsigned char, size f the font, 1-3 in bit depth steps 8bit, 16bit and 24 bit.
 */
void initializeFont(fontContainer *fc, uint8_t size) {
    switch (size) {
        // Font Bitsize 1Byte 7x14
        case 1:
            fc->fontDepthByte = 1;
            fc->fontHeight = 14;
            fc->fontWidth = 7;              // 7 effective font width, nominal 8
            fc->fontSpacing = 10;
            fc->fontHeading = 16;
            fc->charArrayLength = 14;
            fc->font = font1;
            break;
            //Font Bitsize 12Byte 14x26
        case 2:
            fc->fontDepthByte = 2;
            fc->fontHeight = 26;
            fc->fontWidth = 15;             // 15 effective font width, nominal 16
            fc->fontSpacing = 16;
            fc->fontHeading = 28;
            fc->charArrayLength = 52;
            fc->font = font2;
            break;
            // Font Bitsize 3Byte 24x36
        case 3:
            fc->fontDepthByte = 3;
            fc->fontHeight = 36;
            fc->fontWidth = 23;             // 23 effective font width, nominal 22
            fc->fontSpacing = 25;
            fc->fontHeading = 38;
            fc->charArrayLength = 108;
            fc->font = font3;
            break;
        default:
            System_abort("Illegal font size");
    }
}
// Close the Doxygen group.
//! @}
