/*
 * user_setting.h
 *
 *  Created on: 02-Jul-2019
 *      Author: poe
 */

#ifndef USER_SETTING_H_
#define USER_SETTING_H_


#include <mega128a.h>

#define RD_PORT &PORTA
#define RD_PIN  4
#define WR_PORT &PORTA
#define WR_PIN  3
#define CD_PORT &PORTA          // RS PORT
#define CD_PIN  2              // RS PIN
#define CS_PORT &PORTA
#define CS_PIN  1
#define RESET_PORT &PORTA
#define RESET_PIN  0

#define D0_PORT &PORTB
#define D0_PIN 0
#define D1_PORT &PORTB
#define D1_PIN 1
#define D2_PORT &PORTA
#define D2_PIN 15
#define D3_PORT &PORTB
#define D3_PIN 3
#define D4_PORT &PORTB
#define D4_PIN 4
#define D5_PORT &PORTB
#define D5_PIN 5
#define D6_PORT &PORTB
#define D6_PIN 6
#define D7_PORT &PORTA
#define D7_PIN 5



#define  WIDTH    ((uint16_t)240)
#define  HEIGHT   ((uint16_t)320)


/****************** delay in microseconds ***********************/
//extern TIM_HandleTypeDef htim1;
//void delay (int time)
//{
//	/* change your code here for the delay in microseconds */
//	//__HAL_TIM_SET_COUNTER(&htim1, 0);
//	//while ((__HAL_TIM_GET_COUNTER(&htim1))<time);
//}




// configure macros for the data pins.

/* First of all clear all the LCD_DATA pins i.e. LCD_D0 to LCD_D7
 * We do that by writing the HIGHER bits in BSRR Register
 *
 * For example :- To clear Pins B3, B4 , B8, B9, we have to write GPIOB->BSRR = 0b0000001100011000 <<16
 *
 *
 *
 * To write the data to the respective Pins, we have to write the lower bits of BSRR :-
 *
 * For example say the PIN LCD_D4 is connected to PB7, and LCD_D6 is connected to PB2
 *
 * GPIOB->BSRR = (data & (1<<4)) << 3.  Here first select 4th bit of data (LCD_D4), and than again shift left by 3 (Total 4+3 =7 i.e. PB7)
 *
 * GPIOB->BSRR = (data & (1<<6)) >> 4.  Here first select 6th bit of data (LCD_D6), and than again shift Right by 4 (Total 6-4 =2 i.e. PB2)
 *
 *
 */
 


/********************* For 180 MHz *****************************/
//#define WRITE_DELAY { WR_ACTIVE8; }
//#define READ_DELAY  { RD_ACTIVE16;}


/************************** For 72 MHZ ****************************/
//#define WRITE_DELAY { }
//#define READ_DELAY  { RD_ACTIVE;  }


/************************** For 100 MHZ ****************************/
//#define WRITE_DELAY { WR_ACTIVE2; }
//#define READ_DELAY  { RD_ACTIVE4; }


/************************** For 216 MHZ ****************************/
//#define WRITE_DELAY { WR_ACTIVE8; WR_ACTIVE8; } //216MHz
//#define IDLE_DELAY  { WR_IDLE4;WR_IDLE4; }
//#define READ_DELAY  { RD_ACTIVE16;RD_ACTIVE16;RD_ACTIVE16;}


/************************** For 48 MHZ ****************************/
#define WRITE_DELAY { }
#define READ_DELAY  { }


/*****************************  DEFINES FOR DIFFERENT TFTs   ****************************************************/

//#define SUPPORT_0139              //S6D0139 +280 bytes
//#define SUPPORT_0154              //S6D0154 +320 bytes
//#define SUPPORT_1289              //SSD1289,SSD1297 (ID=0x9797) +626 bytes, 0.03s
//#define SUPPORT_1580              //R61580 Untested
//#define SUPPORT_1963              //only works with 16BIT bus anyway
//#define SUPPORT_4532              //LGDP4532 +120 bytes.  thanks Leodino
//#define SUPPORT_4535              //LGDP4535 +180 bytes
//#define SUPPORT_68140             //RM68140 +52 bytes defaults to PIXFMT=0x55
//#define SUPPORT_7735
//#define SUPPORT_7781              //ST7781 +172 bytes
//#define SUPPORT_8230              //UC8230 +118 bytes
//#define SUPPORT_8347D             //HX8347-D, HX8347-G, HX8347-I, HX8367-A +520 bytes, 0.27s
//#define SUPPORT_8347A             //HX8347-A +500 bytes, 0.27s
//#define SUPPORT_8352A             //HX8352A +486 bytes, 0.27s
//#define SUPPORT_8352B             //HX8352B
//#define SUPPORT_8357D_GAMMA       //monster 34 byte
//#define SUPPORT_9163              //
//#define SUPPORT_9225              //ILI9225-B, ILI9225-G ID=0x9225, ID=0x9226, ID=0x6813 +380 bytes
#define SUPPORT_9326_5420         //ILI9326, SPFD5420 +246 bytes
//#define SUPPORT_9342              //costs +114 bytes
//#define SUPPORT_9806              //UNTESTED
//#define SUPPORT_9488_555          //costs +230 bytes, 0.03s / 0.19s
//#define SUPPORT_B509_7793         //R61509, ST7793 +244 bytes
//#define OFFSET_9327 32            //costs about 103 bytes, 0.08s




#endif /* USER_SETTING_H_ */
