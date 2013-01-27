#include <string.h>

#ifndef DISPLAY_H
#define DISPLAY_H

typedef unsigned char PixelCoordonate;

typedef enum {
    DisplayNormalOrientation = 0,
    DisplayRightOrientation = 1,
    DisplayLeftOrientation = 3,
    DisplayUpSideDownOrientation = 2,
} DisplayOrientation;

typedef enum {
    DisplayBlackColor,
    DisplayWhiteColor,
    DisplayInverseColor,
    DisplayTransparentColor
} DisplayColor;

typedef enum {
    DisplayNoMirror = 0,
    DisplayHorizontalMirror = 1,
    DisplayVerticalMirror = 2,
} DisplayMirror;

class Font;

class Display
{
private:
    unsigned char *_buffer;
    PixelCoordonate _realWidth;
    PixelCoordonate _realHeight;
    DisplayOrientation _orientation;
    DisplayMirror _mirror;

public:
    Display(PixelCoordonate width, PixelCoordonate height);
    ~Display();
    
    unsigned char sideOrientation(void) { return (_orientation & 1) != 0; };
    PixelCoordonate getWidth(void) { return this->sideOrientation() ? _realHeight : _realWidth; };
    PixelCoordonate getHeight(void) { return this->sideOrientation() ? _realWidth : _realHeight; };
    DisplayOrientation getOrientation(void) { return _orientation; };
    void setOrientation(DisplayOrientation orientation) { _orientation = orientation; };
    DisplayMirror getMirror(void) { return _mirror; };
    void setMirror(DisplayMirror mirror) { _mirror = mirror; };
    unsigned char *getBuffer(void) { return _buffer; };
    
    void clearDisplay(void) { memset(_buffer, 0, (_realWidth * _realHeight / 8)); };
    void inverseDisplay(void);
    void printString(PixelCoordonate &x, PixelCoordonate &y, const char *string, DisplayColor color, DisplayColor backgroundColor, Font *font, unsigned char horizontalSize = 1, unsigned char verticalSize = 1);

    void drawPixel(PixelCoordonate x, PixelCoordonate y, DisplayColor color);
    void drawLine(PixelCoordonate x0, PixelCoordonate y0, PixelCoordonate x1, PixelCoordonate y1, DisplayColor color);
    int drawChar(PixelCoordonate x, PixelCoordonate y, unsigned char c, DisplayColor color, DisplayColor backgroundColor, Font *font, unsigned char horizontalSize = 1, unsigned char verticalSize = 1);
    void drawFastVLine(PixelCoordonate x, PixelCoordonate y, PixelCoordonate h, DisplayColor color);
    void drawFastHLine(PixelCoordonate x, PixelCoordonate y, PixelCoordonate h, DisplayColor color);
    void fillRect(PixelCoordonate x, PixelCoordonate y, PixelCoordonate w, PixelCoordonate h, DisplayColor color);
};

#endif
