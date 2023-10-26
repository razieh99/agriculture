/*
 * lcd_touch.h
 *
 * This library implements touch screen capabilities of TFTLCD,
 * based on lcd.h driver. To change the type of ILIxxxx driver
 * of your TFTLCD, refer to lcd.h.
 *
 * 29 Oct 2016 by Danylo Ulianych
 */

#ifndef __LCD_TOUCH_H
#define __LCD_TOUCH_H

#include "tft.h"
//#include "stm32f1xx_hal.h"


// ------------------- Initialization and setup -------------------

/**
 * Saves ADC handles references to measure touch screen positions.
 */
void ADC_init();
unsigned int read_adc(unsigned char adc_input);


// ------------------- Reading a touch -------------------
/*
 * Reads raw touch x- and y-positions and, if successful,
 * stores them in the LCD_TouchPoint point.
 */
 int LCD_Touch_Read();

long map(long x, long in_min, long in_max, long out_min, long out_max);



#endif /* __LCD_TOUCH_H */
