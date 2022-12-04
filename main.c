/*
 * Main.c
 *
 *  Created on  : Nov 25, 2022
 *  Author      : Álex, Erick
 *  Description : Sistema de meterologia com dht22 e oled i2c
 */

/* Lib Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Header Files */
#include "I2C.h"
#include "SSD1306_OLED.h"
#include "example_app.h"

/* Oh Compiler-Please leave me as is */
volatile unsigned char flag = 0;

/* Alarm Signal Handler */
void ALARMhandler(int sig)
{
    /* Set flag */
    flag = 5;
}

int main()
{
    /* Initialize I2C bus and connect to the I2C Device */
    if(init_i2c_dev(I2C_DEV2_PATH, SSD1306_OLED_ADDR) == 0)
    {
        printf("(Main)i2c-2: Bus Connected to SSD1306\r\n");
    }
    else
    {
        printf("(Main)i2c-2: OOPS! Something Went Wrong\r\n");
        exit(1);
    }

    /* Register the Alarm Handler */
    signal(SIGALRM, ALARMhandler);

    /* Run SDD1306 Initialization Sequence */
    display_Init_seq();

    /* Clear display */
    clearDisplay();

    // text
    float temp = 4;
    float umi = 5;
    show_values(temp, umi);
    Display();
}