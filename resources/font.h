/*! \file font.h
 * \brief contains definition for Nimbus-Mono-Regular 7x13 Bitmap Font
 * \author Valentin Platzgummer - ic17b096
 * \date Jan 1, 2019
 */

// source: https://stackoverflow.com/questions/2156572/c-header-file-with-bitmapped-fonts/2156872#2156872
// Nimbus-Mono-Regular 8Bit deeps
#ifndef FONT_H_
#define FONT_H_
#include <stdint.h>

//! brief fontContainer struct contains all data for the used font
typedef struct fontContainer {
    uint8_t charArrayLength;    //!< length of the array of each character
    uint8_t fontWidth;          //!< width of each individual characters
    uint8_t fontHeight;         //!< height of each individual characters
    uint8_t fontDepthByte;      //!< Bitdepth of the charachters in Bits 8, 16 or 24
    uint8_t fontSpacing;        //!< Characterlength plus spacing between two characters
    unsigned char *font;
} fontContainer;

extern void initializeFont(fontContainer *fc, int size);
#endif
