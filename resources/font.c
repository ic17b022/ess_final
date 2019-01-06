/*! \file font.c
 * \brief contains implementation for Nimbus-Mono-Regular 7x13 Bitmap Font
 * \author Valentin Platzgummer - ic17b096
 * \date Jan 1, 2019
 */

#include "font.h"
#include "font_array.h"

void initializeFont(fontContainer *fc, int size) {
    switch (size) {
        case 1:
            fc->fontDepthByte = 1;
            fc->fontHeight = 14;
            fc->fontWidth = 7;              // 7 effective font width, nominal 8
            fc->fontSpacing = 10;
            fc->charArrayLength = 14;
            fc->font = font1;
            break;

        case 2:
            fc->fontDepthByte = 2;
            fc->fontHeight = 26;
            fc->fontWidth = 15;             // 15 effective font width, nominal 16
            fc->fontSpacing = 18;
            fc->charArrayLength = 52;
            fc->font = font2;
            break;

        case 3:
            fc->fontDepthByte = 3;
            fc->fontHeight = 36;
            fc->fontWidth = 23;             // 23 effective font width, nominal 22
            fc->fontSpacing = 25;
            fc->charArrayLength = 108;
            fc->font = font3;
            break;
    }
}