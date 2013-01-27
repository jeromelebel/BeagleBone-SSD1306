#include "ssd1306.h"
#include "font.h"
#include <stdio.h>
#include <stdlib.h>

DisplayColor colorWithString(const char *string, DisplayColor defaultValue = DisplayWhiteColor)
{
    DisplayColor result = defaultValue;
    
    if (strcasecmp(string, "white") == 0) {
        result = DisplayWhiteColor;
    } else if (strcasecmp(string, "black") == 0) {
        result = DisplayBlackColor;
    } else if (strcasecmp(string, "transparent") == 0) {
        result = DisplayTransparentColor;
    } else if (strcasecmp(string, "inverse") == 0) {
        result = DisplayInverseColor;
    }
    return result;
}

int main(int argc, const char **argv)
{
    SSD1306 device(0x3D, SSD1306_128_64);
    Font *currentFont = &font1;
    PixelCoordonate x = 0, y = 0;
    DisplayColor color = DisplayWhiteColor, backgroundColor = DisplayBlackColor;
    int vsize = 1, hsize = 1;
    
    if (device.openDevice("/dev/i2c-3") != 0) {
        printf("Problem to open device\n");
        return 1;
    }
    if (device.initDevice() != 0) {
        printf("Problem to init device\n");
        return 1;
    }
    
    device.getDisplay()->clearDisplay();
    for (int ii = 1; ii < argc; ii++) {
        if (strcmp(argv[ii], "--clear") == 0) {
            device.getDisplay()->clearDisplay();
        } else if (strcmp(argv[ii], "--color") == 0) {
            ii++;
            if (ii < argc) {
                color = colorWithString(argv[ii], DisplayWhiteColor);
            }
        } else if (strcmp(argv[ii], "--backgroundcolor") == 0) {
            ii++;
            if (ii < argc) {
                backgroundColor = colorWithString(argv[ii], DisplayBlackColor);
            }
        } else if (strcmp(argv[ii], "--x") == 0) {
            ii++;
            if (ii < argc) {
                x = atoi(argv[ii]);
            }
        } else if (strcmp(argv[ii], "--y") == 0) {
            ii++;
            if (ii < argc) {
                y = atoi(argv[ii]);
            }
        } else if (strcmp(argv[ii], "--lineto") == 0) {
            ii++;
            if (ii + 1 < argc) {
                PixelCoordonate x2, y2;
                
                x2 = atoi(argv[ii]);
                ii++;
                y2 = atoi(argv[ii]);
                device.getDisplay()->drawLine(x, y, x2, y2, color);
                x = x2;
                y = y2;
            }
        } else if (strcmp(argv[ii], "--horizontalmirror") == 0) {
            device.getDisplay()->setMirror((DisplayMirror)(device.getDisplay()->getMirror() ^ DisplayHorizontalMirror));
        } else if (strcmp(argv[ii], "--verticalmirror") == 0) {
            device.getDisplay()->setMirror((DisplayMirror)(device.getDisplay()->getMirror() ^ DisplayVerticalMirror));
        } else if (strcmp(argv[ii], "--font1") == 0) {
            currentFont = &font1;
        } else if (strcmp(argv[ii], "--font2") == 0) {
            currentFont = &font2;
        } else if (strcmp(argv[ii], "--size1") == 0) {
            hsize = vsize = 1;
        } else if (strcmp(argv[ii], "--size2") == 0) {
            hsize = vsize = 2;
        } else if (strcmp(argv[ii], "--vsize1") == 0) {
            vsize = 1;
        } else if (strcmp(argv[ii], "--vsize2") == 0) {
            vsize = 2;
        } else if (strcmp(argv[ii], "--hsize1") == 0) {
            hsize = 1;
        } else if (strcmp(argv[ii], "--hsize2") == 0) {
            hsize = 2;
        } else if (strcmp(argv[ii], "--right") == 0) {
            device.getDisplay()->setOrientation(DisplayRightOrientation);
        } else if (strcmp(argv[ii], "--left") == 0) {
            device.getDisplay()->setOrientation(DisplayLeftOrientation);
        } else if (strcmp(argv[ii], "--upsidedown") == 0) {
            device.getDisplay()->setOrientation(DisplayUpSideDownOrientation);
        } else {
            device.getDisplay()->printString(x, y, argv[ii], color, backgroundColor, currentFont, hsize, vsize);
            y += currentFont->getHeight() * vsize;
            x = 0;
        }
    }
    device.pushDisplay();
    return 0;
}
