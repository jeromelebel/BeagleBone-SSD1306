#include "ssd1306.h"
#include <stdio.h>
#include <stdlib.h>

SSD1306_Color colorWithString(const char *string, SSD1306_Color defaultValue = SSD1306_WhiteColor)
{
    SSD1306_Color result = defaultValue;
    
    if (strcasecmp(string, "white") == 0) {
        result = SSD1306_WhiteColor;
    } else if (strcasecmp(string, "black") == 0) {
        result = SSD1306_BlackColor;
    } else if (strcasecmp(string, "transparent") == 0) {
        result = SSD1306_TransparentColor;
    } else if (strcasecmp(string, "inverse") == 0) {
        result = SSD1306_InverseColor;
    }
    return result;
}

int main(int argc, const char **argv)
{
    SSD1306 display(0x3D, SSD1306_128_64);
    Font *currentFont = &font1;
    SSD1306_Pixel x = 0, y = 0;
    SSD1306_Color color = SSD1306_WhiteColor, backgroundColor = SSD1306_BlackColor;
    int vsize = 1, hsize = 1;
    
    if (display.openDevice("/dev/i2c-3") != 0) {
        printf("Problem to open device\n");
        return 1;
    }
    if (display.initDevice() != 0) {
        printf("Problem to init device\n");
        return 1;
    }
    
    display.clearDisplay();
    for (int ii = 1; ii < argc; ii++) {
        if (strcmp(argv[ii], "--clear") == 0) {
            display.clearDisplay();
        } else if (strcmp(argv[ii], "--color") == 0) {
            ii++;
            if (ii < argc) {
                color = colorWithString(argv[ii], SSD1306_WhiteColor);
            }
        } else if (strcmp(argv[ii], "--backgroundcolor") == 0) {
            ii++;
            if (ii < argc) {
                backgroundColor = colorWithString(argv[ii], SSD1306_BlackColor);
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
                SSD1306_Pixel x2, y2;
                
                x2 = atoi(argv[ii]);
                ii++;
                y2 = atoi(argv[ii]);
                display.drawLine(x, y, x2, y2, color);
                x = x2;
                y = y2;
            }
        } else if (strcmp(argv[ii], "--horizontalmirror") == 0) {
            display.setMirror((SSD1306_Mirror)(display.getMirror() ^ SSD1306_HorizontalMirror));
        } else if (strcmp(argv[ii], "--verticalmirror") == 0) {
            display.setMirror((SSD1306_Mirror)(display.getMirror() ^ SSD1306_VerticalMirror));
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
            display.setOrientation(SSD1306_RightOrientation);
        } else if (strcmp(argv[ii], "--left") == 0) {
            display.setOrientation(SSD1306_LeftOrientation);
        } else if (strcmp(argv[ii], "--upsidedown") == 0) {
            display.setOrientation(SSD1306_UpSideDownOrientation);
        } else {
            display.printString(x, y, argv[ii], color, backgroundColor, currentFont, hsize, vsize);
            y += currentFont->getHeight() * vsize;
            x = 0;
        }
    }
    display.pushDisplay();
    return 0;
}
