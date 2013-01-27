#include "ssd1306.h"
#include <stdio.h>

int main(int argc, const char **argv)
{
    SSD1306 display(0x3D, SSD1306_128_64);
    Font *currentFont = &font1;
    SSD1306_Pixel x = 0, y = 0;
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
        if (strcmp(argv[ii], "--font1") == 0) {
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
            x = 0;
            display.printString(x, y, argv[ii], SSD1306_WhiteColor, SSD1306_BlackColor, currentFont, hsize, vsize);
            y += currentFont->getHeight() * vsize;
        }
    }
    display.pushDisplay();
    return 0;
}
