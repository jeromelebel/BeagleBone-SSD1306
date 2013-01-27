#include "display.h"

#ifndef SSD1306_H
#define SSD1306_H

typedef enum {
    SSD1306_128_64 = 1,
    SSD1306_128_32 = 2,
    SSD1306_96_16 = 3
} SSD1306_Type;

class SSD1306
{
private:
    unsigned char _address;
    int _i2cFileHandler;
    SSD1306_Type _type;
    Display *_display;
    
    void sendCommand(unsigned char command);
    
public:
    SSD1306(unsigned char address, SSD1306_Type type);
    ~SSD1306();
    
    int openDevice(const char *bus);
    void closeDevice(void);
    int initDevice(void);
    
    PixelCoordonate getWidth();
    PixelCoordonate getHeight();
    
    Display *getDisplay(void) { return _display; };
    void pushDisplay(void);
};

#endif /* SSD1306 */
