#include "ssd1306.h"

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

inline void swapPixel(SSD1306_Pixel &a, SSD1306_Pixel &b)
{
    SSD1306_Pixel x;
    
    x = a;
    a = b;
    b = x;
}

#define SSD1306_I2C_ADDRESS   0x3D	// 011110+SA0+RW

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDRESS 0x21
#define SSD1306_PAGEADDRESS 0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

void swap(int &x1, int &x2)
{
    int x;
    
    x = x1;
    x1 = x2;
    x2 = x;
}

SSD1306::SSD1306(unsigned char address, SSD1306_Type type)
{
    _address = address;
    _orientation = SSD1306_NormalOrientation;
    _type = type;
    _mirror = SSD1306_NoMirror;
    _i2cFileHandler = 0;
    switch(_type) {
        case SSD1306_128_64:
            _realWidth = 128;
            _realHeight = 64;
            break;
        case SSD1306_128_32:
            _realWidth = 128;
            _realHeight = 32;
            break;
        case SSD1306_96_16:
            _realWidth = 96;
            _realHeight = 16;
            break;
    }
    _buffer = (unsigned char *)calloc(_realWidth * _realHeight / 8, 1);
}

int SSD1306::openDevice(const char *bus)
{
	_i2cFileHandler = open(bus, O_RDWR);
	if (_i2cFileHandler <= 0) {
	    printf("problem to open the bus\n");
	    return 1;
	}
	
	if (ioctl(_i2cFileHandler, I2C_SLAVE, _address) != 0) {
	    printf("problem to set the slave address\n");
	    return 1;
	}
	return 0;
}

void SSD1306::closeDevice(void)
{
    close(_i2cFileHandler);
}

void SSD1306::sendCommand(unsigned char command)
{
    unsigned char bufferCommand[2] = { 0x00, command };
    
    if (write(_i2cFileHandler, bufferCommand, sizeof(bufferCommand)) != sizeof(bufferCommand)) {
        printf("problem with %d\n", command);
    }
}

int SSD1306::initDevice(void)
{
    int vccstate = SSD1306_SWITCHCAPVCC;
    
    // Init sequence for 128x64 OLED module
    this->sendCommand(SSD1306_DISPLAYOFF);                    // 0xAE
    this->sendCommand(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    this->sendCommand(0x80);                                  // the suggested ratio 0x80
    this->sendCommand(SSD1306_SETMULTIPLEX);                  // 0xA8
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x3F);
            break;
        case SSD1306_128_32:
            this->sendCommand(0x1F);
            break;
        case SSD1306_96_16:
            this->sendCommand(0x0F);
            break;
    }
    this->sendCommand(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    this->sendCommand(0x0);                                   // no offset
    if (_type == SSD1306_96_16) {
        this->sendCommand(SSD1306_SETSTARTLINE | 0x08);            // line #0
    } else {
        this->sendCommand(SSD1306_SETSTARTLINE | 0x0);            // line #0
    }
    this->sendCommand(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) {
        this->sendCommand(0x10);
    } else {
        this->sendCommand(0x14);
    }
    this->sendCommand(SSD1306_MEMORYMODE);                    // 0x20
    this->sendCommand(0x00);                                  // 0x0 act like ks0108
    this->sendCommand(SSD1306_COLUMNADDRESS);                 // 0x21
    this->sendCommand(0x00);                                  // start column
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x7F);                                  // end column 127
            break;
        case SSD1306_128_32:
            this->sendCommand(0x7F);                                  // end column 127
            break;
        case SSD1306_96_16:
            this->sendCommand(95);                                  // end column 95
            break;
    }
    this->sendCommand(SSD1306_PAGEADDRESS);                   // 0x22
    this->sendCommand(0x00);                                  // start page
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x07);                                  // end page 7
            break;
        case SSD1306_128_32:
            this->sendCommand(0x07);                                  // end page 7
            break;
        case SSD1306_96_16:
            this->sendCommand(0x01);                                  // end page 1
            break;
    }
    this->sendCommand(SSD1306_SEGREMAP | 0x1);
    this->sendCommand(SSD1306_COMSCANDEC);
    this->sendCommand(SSD1306_SETCOMPINS);                    // 0xDA
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x12);
            break;
        case SSD1306_128_32:
            this->sendCommand(0x02);
            break;
        case SSD1306_96_16:
            this->sendCommand(0x02);
            break;
    }
    this->sendCommand(SSD1306_SETCONTRAST);                   // 0x81
    switch (_type) {
        case SSD1306_128_64:
            if (vccstate == SSD1306_EXTERNALVCC) {
                this->sendCommand(0x9F);
            } else {
                this->sendCommand(0xCF);
            }
            break;
        case SSD1306_128_32:
            this->sendCommand(0x8F);
            break;
        case SSD1306_96_16:
            if (vccstate == SSD1306_EXTERNALVCC) {
                this->sendCommand(0x10);
            } else {
                this->sendCommand(0xAF);
            }
            break;
    }
    this->sendCommand(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) {
        this->sendCommand(0x22);
    } else {
        this->sendCommand(0xF1);
    }
    this->sendCommand(SSD1306_SETVCOMDETECT);                 // 0xDB
    this->sendCommand(0x40);
    this->sendCommand(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    this->sendCommand(SSD1306_NORMALDISPLAY);                 // 0xA6
    this->sendCommand(SSD1306_DISPLAYON);                     // 0xAF
    return 0;
}

void SSD1306::pushDisplay(void)
{
    unsigned char page[17] = { 0x40 };
    unsigned int x, ii;
    
    this->sendCommand(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
    this->sendCommand(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
    this->sendCommand(SSD1306_SETSTARTLINE | 0x0); // line #0
    for (ii = 0; ii < _realWidth * _realHeight / 8; ii++) {
        // send a bunch of data in one xmission
        for (x = 1; x <= 16; x++) {
            page[x] = _buffer[ii];
            ii++;
        }
        if (write(_i2cFileHandler, page, sizeof(page)) != sizeof(page)) {
            printf("problem with page %d\n", ii);
        }
        ii--;
    }
}

void SSD1306::inverseDisplay(void)
{
    int ii;
    
    for (ii = 0; ii < _realWidth * _realHeight / 8; ii++) {
        _buffer[ii] ^= 0xff;
    }
}

void SSD1306::printString(SSD1306_Pixel &x, SSD1306_Pixel &y, const char *string, SSD1306_Color color, SSD1306_Color backgroundColor, Font *font, unsigned char horizontalSize, unsigned char verticalSize)
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

int SSD1306::drawChar(SSD1306_Pixel x, SSD1306_Pixel y, unsigned char c, SSD1306_Color color, SSD1306_Color backgroundColor, Font *font, unsigned char horizontalSize, unsigned char verticalSize)
{
    for (SSD1306_Pixel cursorX = 0; cursorX <= font->getWidth(); cursorX++ ) {
        unsigned char line;
        
        if (cursorX == font->getWidth()) {
            line = 0x0;
        } else {
            line = font->pixelsForChar(c, cursorX);
        }
        for (SSD1306_Pixel cursorY = 0; cursorY < font->getHeight(); cursorY++) {
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

void SSD1306::drawPixel(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Color color)
{
    SSD1306_Pixel swap;
    
    // check rotation, move pixel around if necessary
    switch (this->getOrientation()) {
        case SSD1306_NormalOrientation:
            break;
        case SSD1306_RightOrientation:
            swapPixel(x, y);
            x = _realWidth - x - 1;
            break;
        case SSD1306_UpSideDownOrientation:
            x = _realWidth - x - 1;
            y = _realHeight - y - 1;
            break;
        case SSD1306_LeftOrientation:
            swapPixel(x, y);
            y = _realHeight - y - 1;
            break;
    }
    if (_mirror & SSD1306_VerticalMirror) {
        if (this->sideOrientation()) {
            x = _realWidth - x - 1;
        } else {
            y = _realHeight - y - 1;
        }
    }
    if (_mirror & SSD1306_HorizontalMirror) {
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
            case SSD1306_WhiteColor:
                _buffer[x + (y/8) * _realWidth] |= _BV((y%8));
                break;
            case SSD1306_BlackColor:
                _buffer[x + (y/8) * _realWidth] &= ~_BV((y%8));
                break;
            case SSD1306_InverseColor:
                _buffer[x + (y/8) * _realWidth] = _buffer[x + (y/8) * _realWidth] ^ _BV((y%8));
                break;
            case SSD1306_TransparentColor:
                break;
        }
    }
}


void SSD1306::drawLine(SSD1306_Pixel x0, SSD1306_Pixel y0, SSD1306_Pixel x1, SSD1306_Pixel y1, SSD1306_Color color)
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

void SSD1306::drawFastVLine(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Pixel h, SSD1306_Color color)
{
    if (this->sideOrientation()) {
        // stupidest version - update in subclasses if desired!
        drawLine(x, y, x, y+h-1, color);
    } else {
        drawLine(x, y, x, y+h-1, color);
    }
}

void SSD1306::drawFastHLine(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Pixel h, SSD1306_Color color)
{
    if (this->sideOrientation()) {
        drawLine(x, y, x + h - 1, y, color);
    } else {
        // stupidest version - update in subclasses if desired!
        drawLine(x, y, x + h - 1, y, color);
    }
}

void SSD1306::fillRect(SSD1306_Pixel x, SSD1306_Pixel y, SSD1306_Pixel w, SSD1306_Pixel h, SSD1306_Color color)
{
    SSD1306_Pixel ii;
    
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
