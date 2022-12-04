
/* Lib Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include  <signal.h>
#include <unistd.h>
#include "SSD1306_OLED.h"
#include "example_app.h"

/* MACRO's */
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

/* Extern volatile */
extern volatile unsigned char flag;

/* Bit Map - Taken from Adafruit SSD1306 OLED Library  */
static const unsigned char logo16_glcd_bmp[] =
{
 0b00000000, 0b11000000,
 0b00000001, 0b11000000,
 0b00000001, 0b11000000,
 0b00000011, 0b11100000,
 0b11110011, 0b11100000,
 0b11111110, 0b11111000,
 0b01111110, 0b11111111,
 0b00110011, 0b10011111,
 0b00011111, 0b11111100,
 0b00001101, 0b01110000,
 0b00011011, 0b10100000,
 0b00111111, 0b11100000,
 0b00111111, 0b11110000,
 0b01111100, 0b11110000,
 0b01110000, 0b01110000,
 0b00000000, 0b00110000
};

/* draw many lines */
void testdrawline() 
{
    short i = 0;
    for (i=0; i<SSD1306_LCDWIDTH; i+=4)
    {
        drawLine(0, 0, i, SSD1306_LCDHEIGHT-1, WHITE);
        Display();
        usleep(1000);
    }
    for (i=0; i<SSD1306_LCDHEIGHT; i+=4)
    {
        drawLine(0, 0, SSD1306_LCDWIDTH-1, i, WHITE);
        Display();
        usleep(1000);
    }
    usleep(250000);

    clearDisplay();
    for (i=0; i<SSD1306_LCDWIDTH; i+=4)
    {
        drawLine(0, SSD1306_LCDHEIGHT-1, i, 0, WHITE);
        Display();
        usleep(1000);
    }
    for (i=SSD1306_LCDHEIGHT-1; i>=0; i-=4)
    {
        drawLine(0, SSD1306_LCDHEIGHT-1, SSD1306_LCDWIDTH-1, i, WHITE);
        Display();
        usleep(1000);
    }
    usleep(250000);

    clearDisplay();
    for (i=SSD1306_LCDWIDTH-1; i>=0; i-=4)
    {
        drawLine(SSD1306_LCDWIDTH-1, SSD1306_LCDHEIGHT-1, i, 0, WHITE);
        Display();
        usleep(1000);
    }
    for (i=SSD1306_LCDHEIGHT-1; i>=0; i-=4)
    {
        drawLine(SSD1306_LCDWIDTH-1, SSD1306_LCDHEIGHT-1, 0, i, WHITE);
        Display();
        usleep(1000);
    }
    usleep(250000);

    clearDisplay();
    for (i=0; i<SSD1306_LCDHEIGHT; i+=4)
    {
        drawLine(SSD1306_LCDWIDTH-1, 0, 0, i, WHITE);
        Display();
        usleep(1000);
    }
    for (i=0; i<SSD1306_LCDWIDTH; i+=4) {
        drawLine(SSD1306_LCDWIDTH-1, 0, i, SSD1306_LCDHEIGHT-1, WHITE);
        Display();
        usleep(1000);
    }
    usleep(250000);
}


/* Display a bunch of characters and emoticons */
void testdrawchar()
{
    unsigned char i = 0;
    setTextSize(1);
    setTextColor(WHITE);
    setCursor(0,0);

    for (i=0; i < 168; i++)
    {
        if (i == '\n')
            continue;
        oled_write(i);
        if ((i > 0) && (i % 21 == 0))
            println();
    }
    Display();
    usleep(1000);
}

/* Display "scroll" and scroll around */
void testscrolltext()
{
    setTextSize(2);
    setTextColor(WHITE);
    setCursor(10,0);
    print_str("scroll");
    println();
    Display();
    usleep(1000);
    startscrollright(0x00, 0x0F);
    usleep(2000000);
    stopscroll();
    usleep(1000000);
    startscrollleft(0x00, 0x0F);
    usleep(2000000);
    stopscroll();
    usleep(1000000);
    startscrolldiagright(0x00, 0x07);
    usleep(2000000);
    startscrolldiagleft(0x00, 0x07);
    usleep(2000000);
    stopscroll();
}

/* Display Texts */
void display_texts()
{
    setTextSize(1);
    setTextColor(WHITE);
    setCursor(10,0);
    print_str("HELLO FELLAS!");
    println();
    printFloat_ln(3.141592, 4); //Print 4 No's after the decimal Pt.
    printNumber_L_ln(-1234, DEC);
    printNumber_UC_ln(170, BIN);
    setTextSize(2);
    setTextColor(WHITE);
    print_str("0x");
    printNumber_UL_ln(0xDEADBEEF, HEX);
}

/* Display miniature bitmap */
void display_bitmap()
{
    drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
}

/* Invert Display and Normalize it */
void display_invert_normal()
{
    invertDisplay(SSD1306_INVERT_DISPLAY);
    usleep(1000000);
    invertDisplay(SSD1306_NORMALIZE_DISPLAY);
    usleep(1000000);
}

/* Draw a bitmap and 'animate' movement */
void testdrawbitmap(const unsigned char *bitmap, unsigned char w, unsigned char h)
{
    unsigned char icons[NUMFLAKES][3], f = 0;

    // initialize
    for (f=0; f< NUMFLAKES; f++)
    {
        icons[f][XPOS] = rand() % SSD1306_LCDWIDTH;
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = (rand() % 5) + 1;

        /* Looks kinna ugly to me - Un-Comment if you need it */
        //print_str("x: ");
        //printNumber_UC(icons[f][XPOS], DEC);
        //print_str("y: ");
        //printNumber_UC(icons[f][YPOS], DEC);
        //print_str("dy: ");
        //printNumber_UC(icons[f][DELTAY], DEC);
    }

    while (flag != 5)
    {
        // draw each icon
        for (f=0; f< NUMFLAKES; f++)
        {
            drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
        }
        Display();
        usleep(200000);

        // then erase it + move it
        for (f=0; f< NUMFLAKES; f++)
        {
            drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);

            // move it
            icons[f][YPOS] += icons[f][DELTAY];

            // if its gone, reinit
            if (icons[f][YPOS] > SSD1306_LCDHEIGHT)
            {
                icons[f][XPOS] = rand() % SSD1306_LCDWIDTH;
                icons[f][YPOS] = 0;
                icons[f][DELTAY] = (rand() % 5) + 1;
            }
        }
    }
}


/* Intro */
void show_values(float temperatura, float umidade)
{
    setTextSize(1);
    setTextColor(WHITE);
    setCursor(1,0);
    print_strln("temperatura %f", &temperatura);
    println();
    print_strln("umidade %f", &umidade);
    println();
    println();
}