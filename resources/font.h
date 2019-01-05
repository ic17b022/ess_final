/*! \file font.h
 * \brief contains definition for Nimbus-Mono-Regular 7x13 Bitmap Font
 * \author Valentin Platzgummer - ic17b096
 * \date Jan 1, 2019
 */

// source: https://stackoverflow.com/questions/2156572/c-header-file-with-bitmapped-fonts/2156872#2156872
// Nimbus-Mono-Regular 8Bit deep
#ifndef FONT_H_
#define FONT_H_

#define FONT_WIDTH 7
#define FONT_HEIGHT 13
#define FONT_BIT_PER_CHAR 8
#define FONT_SPACING 10
#define FONT_STARTING_NUMBER 0x20 // 'Space' ASCII 32)

extern const unsigned char chars[95][13];

typedef const struct fontContainer {
    unsigned char fontWidth;
    unsigned char fontHeight;
    unsigned char fontDepth;
    unsigned char fontSpacing;
    unsigned char fontStartingNumber;
    unsigned char **font;
} fontContainer;

// extern const fontContainer nimbusMono7x13;
#endif
