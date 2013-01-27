#include "font.h"
#include <string.h>

#ifndef SSD1306_H
#define SSD1306_H

typedef enum {
    SSD1306_128_64 = 1,
    SSD1306_128_32 = 2,
    SSD1306_96_16 = 3
} SSD1306_Type;

typedef enum {
    SSD1306_NormalOrientation = 0,
    SSD1306_RightOrientation = 1,
    SSD1306_LeftOrientation = 3,
    SSD1306_UpSideDownOrientation = 2,
} SSD1306_Orientation;

typedef enum {
    SSD1306_BlackColor,
    SSD1306_WhiteColor,
    SSD1306_InverseColor,
    SSD1306_TransparentColor
} SSD1306_Color;

typedef enum {
    SSD1306_NoMirror = 0,
    SSD1306_HorizontalMirror = 1,
    SSD1306_VerticalMirror = 2,
} SSD1306_Mirror;

class SSD1306
{
private:
    unsigned char _address;
    unsigned char *_buffer;
    int _i2cFileHandler;
    SSD1306_Type _type;
    SSD1306_Pixel _realWidth;
    SSD1306_Pixel _realHeight;
    SSD1306_Orientation _orientation;
    SSD1306_Mirror _mirror;
    
    void sendCommand(unsigned char command);
    
public:
    SSD1306(unsigned char address, SSD1306_Type type);
    int openDevice(const char *bus);
    void closeDevice(void);
    int initDevice(void);
    
    unsigned char sideOrientation(void) { return (_orientation & 1) != 0; };
    SSD1306_Pixel getWidth(void) { return this->sideOrientation() ? _realHeight : _realWidth; };
    SSD1306_Pixel getHeight(void) { return this->sideOrientation() ? _realWidth : _realHeight; };
    SSD1306_Orientation getOrientation(void) { return _orientation; };
    void setOrientation(SSD1306_Orientation orientation) { _orientation = orientation; };
    SSD1306_Mirror getMirror(void) { return _mirror; };
    void setMirror(SSD1306_Mirror mirror) { _mirror = mirror; };
    
    void inverseDisplay(void);
    void printString(SSD1306_Pixel &x, SSD1306_Pixel &y, const char *string, SSD1306_Color color, SSD1306_Color backgroundColor, Font *font, unsigned char horizontalSize = 1, unsigned char verticalSize = 1);
    void pushDisplay(void);
    void clearDisplay(void) { memset(_buffer, 0, (_realWidth * _realHeight / 8)); };
    
    void drawPixel(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Color color);
    void drawLine(SSD1306_Pixel x0, SSD1306_Pixel y0, SSD1306_Pixel x1, SSD1306_Pixel y1, SSD1306_Color color);
    int drawChar(SSD1306_Pixel x, SSD1306_Pixel y, unsigned char c, SSD1306_Color color, SSD1306_Color backgroundColor, Font *font, unsigned char horizontalSize = 1, unsigned char verticalSize = 1);
    void drawFastVLine(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Pixel h, SSD1306_Color color);
    void drawFastHLine(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Pixel h, SSD1306_Color color);
    void fillRect(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Pixel w, SSD1306_Pixel h, SSD1306_Color color);
};

#endif /* SSD1306 */
