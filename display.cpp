#include "display.h"
#include "font.h"
#include <stdlib.h>

inline void swapPixel(PixelCoordonate &a, PixelCoordonate &b)
{
    PixelCoordonate x;
    
    x = a;
    a = b;
    b = x;
}

Display::Display(PixelCoordonate width, PixelCoordonate height)
{
    _realWidth = width;
    _realHeight = height;
    _orientation = DisplayNormalOrientation;
    _mirror = DisplayNoMirror;
    _buffer = (unsigned char *)calloc(_realWidth * _realHeight / 8, 1);
}

Display::~Display()
{
    free(_buffer);
}

void Display::inverseDisplay(void)
{
    int ii;
    
    for (ii = 0; ii < _realWidth * _realHeight / 8; ii++) {
        _buffer[ii] ^= 0xff;
    }
}

void Display::printString(PixelCoordonate &x, PixelCoordonate &y, const char *string, DisplayColor color, DisplayColor backgroundColor, Font *font, unsigned char horizontalSize, unsigned char verticalSize)
{
    while(string[0] != 0) {
        if (string[0] == '\n') {
            x = 0;
            y = y + font->getHeight() * verticalSize;
        } else {
            x += this->drawChar(x, y, string[0], color, backgroundColor, font, horizontalSize, verticalSize);
        }
        if (x + (font->getWidth() * horizontalSize) > this->getWidth()) {
            y = y + (font->getHeight() * verticalSize);
            x = 0;
        }
        if (y + (font->getHeight() * verticalSize) > this->getHeight()) {
            return;
        }
        string = string + 1;
    }
}

int Display::drawChar(PixelCoordonate x, PixelCoordonate y, unsigned char c, DisplayColor color, DisplayColor backgroundColor, Font *font, unsigned char horizontalSize, unsigned char verticalSize)
{
    for (PixelCoordonate cursorX = 0; cursorX <= font->getWidth(); cursorX++ ) {
        unsigned char line;
        
        if (cursorX == font->getWidth()) {
            line = 0x0;
        } else {
            line = font->pixelsForChar(c, cursorX);
        }
        for (PixelCoordonate cursorY = 0; cursorY < font->getHeight(); cursorY++) {
            if (line & 0x1) {
                if (horizontalSize == 1 && verticalSize == 1) {
                    this->drawPixel(x + cursorX, y + cursorY, color);
                } else {  // big size
                    this->fillRect(x + (cursorX * horizontalSize), y + (cursorY * verticalSize), horizontalSize, verticalSize, color);
                } 
            } else {
                if (horizontalSize == 1 && verticalSize == 1) {
                    this->drawPixel(x + cursorX, y + cursorY, backgroundColor);
                } else {  // big size
                    this->fillRect(x + (cursorX * horizontalSize), y + (cursorY * verticalSize), horizontalSize, verticalSize, backgroundColor);
                } 	
            }
            line >>= 1;
        }
    }
    return (font->getWidth() + 1) * horizontalSize;
}

#define _BV(bit) (1 << (bit)) 

void Display::drawPixel(PixelCoordonate x, PixelCoordonate y, DisplayColor color)
{
    PixelCoordonate swap;
    
    // check rotation, move pixel around if necessary
    switch (this->getOrientation()) {
        case DisplayNormalOrientation:
            break;
        case DisplayRightOrientation:
            swapPixel(x, y);
            x = _realWidth - x - 1;
            break;
        case DisplayUpSideDownOrientation:
            x = _realWidth - x - 1;
            y = _realHeight - y - 1;
            break;
        case DisplayLeftOrientation:
            swapPixel(x, y);
            y = _realHeight - y - 1;
            break;
    }
    if (_mirror & DisplayVerticalMirror) {
        if (this->sideOrientation()) {
            x = _realWidth - x - 1;
        } else {
            y = _realHeight - y - 1;
        }
    }
    if (_mirror & DisplayHorizontalMirror) {
        if (this->sideOrientation()) {
            y = _realHeight - y - 1;
        } else {
            x = _realWidth - x - 1;
        }
    }
    if ((x >= 0)
        && (x < _realWidth)
        && (y >= 0)
        && (y < _realHeight)) {
        switch (color) {
            case DisplayWhiteColor:
                _buffer[x + (y/8) * _realWidth] |= _BV((y%8));
                break;
            case DisplayBlackColor:
                _buffer[x + (y/8) * _realWidth] &= ~_BV((y%8));
                break;
            case DisplayInverseColor:
                _buffer[x + (y/8) * _realWidth] = _buffer[x + (y/8) * _realWidth] ^ _BV((y%8));
                break;
            case DisplayTransparentColor:
                break;
        }
    }
}


void Display::drawLine(PixelCoordonate x0, PixelCoordonate y0, PixelCoordonate x1, PixelCoordonate y1, DisplayColor color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swapPixel(x0, y0);
        swapPixel(x1, y1);
    }
    
    if (x0 > x1) {
        swapPixel(x0, x1);
        swapPixel(y0, y1);
    }
    
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    
    int16_t err = dx / 2;
    int16_t ystep;
    
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }
    
    for (; x0<=x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void Display::drawFastVLine(PixelCoordonate x, PixelCoordonate y, PixelCoordonate h, DisplayColor color)
{
    if (this->sideOrientation()) {
        // stupidest version - update in subclasses if desired!
        drawLine(x, y, x, y+h-1, color);
    } else {
        drawLine(x, y, x, y+h-1, color);
    }
}

void Display::drawFastHLine(PixelCoordonate x, PixelCoordonate y, PixelCoordonate h, DisplayColor color)
{
    if (this->sideOrientation()) {
        drawLine(x, y, x + h - 1, y, color);
    } else {
        // stupidest version - update in subclasses if desired!
        drawLine(x, y, x + h - 1, y, color);
    }
}

void Display::fillRect(PixelCoordonate x, PixelCoordonate y, PixelCoordonate w, PixelCoordonate h, DisplayColor color)
{
    PixelCoordonate ii;
    
    if (this->sideOrientation()) {
        for (ii = y; ii < y + h; ii++) {
            drawFastHLine(x, ii, w, color); 
        }
    } else {
        for (ii = x; ii < x + w; ii++) {
            drawFastVLine(ii, y, h, color); 
        }
    }
}
