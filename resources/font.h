/*! \file font.h
 * \brief contains definition for Nimbus-Mono-Regular 7x13 Bitmap Font
 * \author Valentin Platzgummer - ic17b096
 * \date Jan 1, 2019
 */

// source: https://stackoverflow.com/questions/2156572/c-header-file-with-bitmapped-fonts/2156872#2156872
// Nimbus-Mono-Regular 8Bit deeps
#ifndef FONT_H_
#define FONT_H_
#include "common.h"
//! \addtogroup OLED Graphical Resources
//! @{

//! brief fontContainer struct contains all data for the used font
typedef struct fontContainer {
    uint8_t charArrayLength;    //!< array length array for each individual character
    uint8_t fontWidth;          //!< width of each individual characters
    uint8_t fontHeight;         //!< height of each individual characters
    uint8_t fontDepthByte;      //!< Bit depth of the characters in Bits 8, 16 or 24
    uint8_t fontSpacing;        //!< Character length plus spacing between two characters
    uint8_t fontHeading;        //!< Headroom + fontheight for the given font, total height
    unsigned char *font;        //!< Pointer to font array, font is given in 1dim char array, coded bitmap
} fontContainer;

extern void initializeFont(fontContainer *fc, uint8_t size);
#endif
// Close the Doxygen group.
//! @}
