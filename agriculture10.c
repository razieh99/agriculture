/*******************************************************
This program was created by the
CodeWizardAVR V3.14 Advanced
Automatic Program Generator
� Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 6/23/2021
Author  : 
Company : 
Comments: 


Chip type               : ATmega128A
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 2048
*******************************************************/

#include <mega128a.h>

#include <delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Graphic Display functions
#include <glcd.h>
#include "DHT22.h"
// Font used for displaying text
// on the graphic display
#include <font5x7.h>

// Declare your global variables here
#include "glcd_libs\glcdsia.h"
#include "glcd_libs\lcd_touch.h"
#include "glcd_fonts\arial_narrow_bold.h"
//glcd icons
#include "glcd_icons\setting.h"
#include "glcd_icons\sun.h"
#include "glcd_icons\temp.h"
#include "glcd_icons\hum.h"
#include "glcd_icons\soil.h"
#include "glcd_icons\back.h"
#include "glcd_icons\ph.h"
#include "glcd_icons\ec.h"
#include "glcd_icons\logo.h"
#include "glcd_icons\pH30_30.h"
//
// Definition of wifi states
#define BUSY 0
#define READY 1
//
#define DISCONNECTED 0
#define CONNECTED 1
//
eeprom char wifi_ssid[48]="notdef";
eeprom char wifi_pass[48]="notdef";
 char wifi_ssid_ram[48];
 char wifi_pass_ram[48];
 char nodeID_ram[48]; 
 
 char sendDate[32];    
 
eeprom char nodeId[32]="notdef";
char text[48]; 
char recieved=0;

char back,set,first;

int light , warning=0;
float soil,temperature,humidity,EC,volt,pH;
char httpHeader[200];
char sendcommand[40];
char lcdBuffer[100];
char phstring[100];
char wifiModuleState = 0;
char networkState=DISCONNECTED;
bit wifiRecieved = false;
char sendOK = 0,serverResponse =0;
unsigned int seconds=0,periode=0;
unsigned int minutes=0;
unsigned char tic=0;
unsigned int ticm=0;
void initialScreen();
int sensors();
void setting ();



void sendToServer();
char wifiState=READY;
void wifi_reconnect();

void strcpytoeep(eeprom char *dest,char *src)  // copies string src from RAM to dest in eeprom 
{
char i=0;
 while(src[i] != '\0')
 {
  dest[i]=src[i];
  i++;
 }
 dest[i]='\0';
}

void strcpyfromeep(char *dest,eeprom char *src)  // copies string src from eeprom to dest in RAM 
{
char i=0;
 while(src[i] != '\0')
 {
  dest[i]=src[i];
  i++;
 }
 dest[i]='\0';
}

void extractElementstr(char str[],flash char element[],char *value) //searchs for the parameter element in string str and returns the value between""                                                                    //eg. "#abcd ssid="ssidvalue" dummy**"  function extracts ssidvalue
{
 char ch_pos=0,i=0;
 char digit; 
 ch_pos = strstrf(str,element) - str ;// strstrf returns a pointer to the character in str1 where str2 begins
                                     //then, to calculate index(position) the pointer should be substracted from pointer representing beginig of the str 
 ch_pos =  ch_pos+strlenf(element) + 2;   // ex. "nodeID="hihello" +2 is for '="' sign
 
 while( (digit = str[ch_pos+i]) != '"' && i<strlen(str))
 { 
  value[i] = digit; 
  i++;
 }
 value[i] = '\0';
}

#define DATA_REGISTER_EMPTY (1<<UDRE0)
#define RX_COMPLETE (1<<RXC0)
#define FRAMING_ERROR (1<<FE0)
#define PARITY_ERROR (1<<UPE0)
#define DATA_OVERRUN (1<<DOR0)

// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 128
char rx_buffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0 <= 256
unsigned char rx_wr_index0=0,rx_rd_index0=0;
#else
unsigned int rx_wr_index0=0,rx_rd_index0=0;
#endif

#if RX_BUFFER_SIZE0 < 256
unsigned char rx_counter0=0;
#else
unsigned int rx_counter0=0;
#endif

// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow0;

// USART0 Receiver interrupt service routine
interrupt [USART0_RXC] void usart0_rx_isr(void)
{
char status,data;
status=UCSR0A;
data=UDR0;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
    if (rx_wr_index0 == RX_BUFFER_SIZE0 || data == '#' ){
    rx_wr_index0=0;  
    }
    rx_buffer0[rx_wr_index0]=data;   
    rx_wr_index0++ ;
   }   
if (rx_buffer0[rx_wr_index0-1] == '*' && rx_buffer0[0] =='#' && set==1)// wifiState == READY) //complete command has been recieved
   {  
    extractElementstr(rx_buffer0,"ssid",text);
    strcpytoeep(wifi_ssid,text);
    extractElementstr(rx_buffer0,"pass",text);
    strcpytoeep(wifi_pass,text);
    extractElementstr(rx_buffer0,"nodeId",text); 
    strcpytoeep(nodeId,text);
    recieved = 1;
    delay_ms(20);
    wifi_reconnect();
   }  
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART0 Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter0==0);
data=rx_buffer0[rx_rd_index0++];
#if RX_BUFFER_SIZE0 != 256
if (rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
#endif
#asm("cli")
--rx_counter0;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART1 Receiver buffer
#define RX_BUFFER_SIZE1 128
char rx_buffer1[RX_BUFFER_SIZE1];

#if RX_BUFFER_SIZE1 <= 256
unsigned char rx_wr_index1=0,rx_rd_index1=0;
#else
unsigned int rx_wr_index1=0,rx_rd_index1=0;
#endif

#if RX_BUFFER_SIZE1 < 256
unsigned char rx_counter1=0;
#else
unsigned int rx_counter1=0;
#endif

// This flag is set on USART1 Receiver buffer overflow
bit rx_buffer_overflow1;

// USART1 Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rx_isr(void)
{
char status,data;
status=UCSR1A;
data=UDR1;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
    if (rx_wr_index1 == RX_BUFFER_SIZE1 ){
    rx_wr_index1=0;  
    }
    rx_buffer1[rx_wr_index1]=data;   
    rx_wr_index1++ ; 
     //glcdsia_outtextxy(50,160,rx_buffer1);
   }   
if (rx_buffer1[rx_wr_index1-2] == '\r' && rx_buffer1[rx_wr_index1-1] =='\n') //complete command has been recieved
   { 
    rx_buffer1[rx_wr_index1] = '\0';
    rx_wr_index1=0;
    
    wifiRecieved = true;
    
    if (rx_buffer1[0]=='+')  
    {
         if(strstrf(rx_buffer1,"200")) 
        {
          serverResponse = true;
        } 
        else 
         serverResponse = false;
    }
       
    if (strstrf(rx_buffer1,"OK\r\n") )
    {  
     wifiState = READY; // ready to recieve new commands
    } 
//    if (strstrf(rx_buffer1,"ready\r\n") )
//    {
//     wifiModuleState = 1;// wifi module started up ( after restart or turn on)
//    }
    if (strstrf(rx_buffer1,"CONNECTED\r\n"))
    {
    //strcpyf(networkState,"CONNECTED      ");
      networkState=CONNECTED ;
    } 
    if (strstrf(rx_buffer1,"WIFI DISCONNECT\r\n"))
    {
    networkState= DISCONNECTED;
    }
    if(strstrf(rx_buffer1,"FAIL\r\n")) 
    {
      networkState= DISCONNECTED;
      wifiState = READY;
    }
    if(strstrf(rx_buffer1,"ERROR\r\n")) 
    {
      wifiState = READY;
    }
    else if(strstrf(rx_buffer1,"SEND OK\r\n")) 
    {
      wifiState = READY;
      sendOK= true;
    }
    // = strstrf(rx_buffer1,"Date");  
    //index = strstrf(,"Date") ;
//    if ( strstrf(rx_buffer1,"Date"))
//    {   
//    // strlcpy(sendDate,rx_buffer1,25);
//    }
   } 
}

// Get a character from the USART1 Receiver buffer
#pragma used+
char getchar1(void)
{
char data;
while (rx_counter1==0);
data=rx_buffer1[rx_rd_index1++];
#if RX_BUFFER_SIZE1 != 256
if (rx_rd_index1 == RX_BUFFER_SIZE1) rx_rd_index1=0;
#endif
#asm("cli")
--rx_counter1;
#asm("sei")
return data;
}
#pragma used-
// Write a character to the USART1 Transmitter
#pragma used+
void putchar1(char c)
{
while ((UCSR1A & DATA_REGISTER_EMPTY)==0);
UDR1=c;
}
#pragma used-


void puts1(char *st){
int n;
    for(n=0;n<strlen(st);n++){
        putchar1(st[n]);
    }
}
// Standard Input/Output functions
#include <stdio.h>

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Reinitialize Timer 0 value
TCNT0=0xB2;
tic++;
ticm++;
if(tic >= 100)
{
 seconds++;
 periode++;
 tic=0;    
 //puts("ok");
          if (warning == 0){ 
          if ( periode == 2)  
          {
          PORTB.7 = 1;    // GREEN ON / OFF
          PORTB.5 = 0;
          PORTB.6 = 0;
          } 
          if ( periode == 3)  
          {
          PORTB.7 = 0;    // GREEN ON / OFF
          PORTB.6 = 0;
          PORTB.5 = 0; 
          periode = 0;
          }
          }
          if (warning == 1)  {
           if ( periode == 2)  
          {
          PORTB.6 = 1;    // RED ON / OFF
          PORTB.5 = 0;
          PORTB.7 = 0;
          } 
          if ( periode == 3)  
          {
          PORTB.7 = 0;    // RED ON / OFF
          PORTB.6 = 0;
          PORTB.5 = 0; 
          periode = 0;
          }
          }
}
if (ticm >= 1500)
{
 minutes++;  
 ticm = 0;
 //seconds = 0;
}
//if (minutes == 1)
//{
// sendToServer();
//}

}

// Timer2 overflow interrupt service routine
interrupt [TIM2_OVF] void timer2_ovf_isr(void)
{
// Reinitialize Timer2 value
TCNT2=0x38;
// Place your code here

}

// Timer3 overflow interrupt service routine
interrupt [TIM3_OVF] void timer3_ovf_isr(void)
{
// Reinitialize Timer3 value
TCNT3H=0x9E58 >> 8;
TCNT3L=0x9E58 & 0xff;
// Place your code here

}

// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=(1<<ADSC);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);
return ADCW;
}

void main(void)
{
// Declare your local variables here
// back is used for touch 
// set is used for setting menu
                 
//char ssid[32],pass[32],node[32];
// Variable used to store graphic display
// controller initialization data
GLCDINIT_t glcd_init_data;
unsigned int yposInformation=10;
// Reset Source checking
if (MCUCSR & (1<<PORF))
   {
   // Power-on Reset
   MCUCSR&=~((1<<JTRF) | (1<<WDRF) | (1<<BORF) | (1<<EXTRF) | (1<<PORF));
   // Place your code here

   }
else if (MCUCSR & (1<<EXTRF))
   {
   // External Reset
   MCUCSR&=~((1<<JTRF) | (1<<WDRF) | (1<<BORF) | (1<<EXTRF) | (1<<PORF));
   // Place your code here

   }
else if (MCUCSR & (1<<BORF))
   {
   // Brown-Out Reset
   MCUCSR&=~((1<<JTRF) | (1<<WDRF) | (1<<BORF) | (1<<EXTRF) | (1<<PORF));
   // Place your code here

   }
else if (MCUCSR & (1<<WDRF))
   {
   // Watchdog Reset
   MCUCSR&=~((1<<JTRF) | (1<<WDRF) | (1<<BORF) | (1<<EXTRF) | (1<<PORF));
   // Place your code here

   }
else if (MCUCSR & (1<<JTRF))
   {
   // JTAG Reset
   MCUCSR&=~((1<<JTRF) | (1<<WDRF) | (1<<BORF) | (1<<EXTRF) | (1<<PORF));
   // Place your code here

   }

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Port E initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRE=(0<<DDE7) | (0<<DDE6) | (0<<DDE5) | (0<<DDE4) | (0<<DDE3) | (0<<DDE2) | (0<<DDE1) | (0<<DDE0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTE=(0<<PORTE7) | (0<<PORTE6) | (0<<PORTE5) | (0<<PORTE4) | (0<<PORTE3) | (0<<PORTE2) | (0<<PORTE1) | (0<<PORTE0);

// Port F initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRF=(0<<DDF7) | (0<<DDF6) | (0<<DDF5) | (0<<DDF4) | (0<<DDF3) | (0<<DDF2) | (0<<DDF1) | (0<<DDF0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTF=(0<<PORTF7) | (0<<PORTF6) | (0<<PORTF5) | (0<<PORTF4) | (0<<PORTF3) | (0<<PORTF2) | (0<<PORTF1) | (0<<PORTF0);

// Port G initialization
// Function: Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRG=(0<<DDG4) | (0<<DDG3) | (0<<DDG2) | (0<<DDG1) | (0<<DDG0);
// State: Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTG=(0<<PORTG4) | (0<<PORTG3) | (0<<PORTG2) | (0<<PORTG1) | (0<<PORTG0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 7.813 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
// Timer Period: 9.984 ms
ASSR=0<<AS0;
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (1<<CS02) | (1<<CS01) | (1<<CS00);
TCNT0=0xB2;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// OC1C output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;


// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
TCCR2=(0<<WGM20) | (0<<COM21) | (0<<COM20) | (0<<WGM21) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x38;
OCR2=0x00;

// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: 125.000 kHz
// Mode: Normal top=0xFFFF
// OC3A output: Disconnected
// OC3B output: Disconnected
// OC3C output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 0.2 s
// Timer3 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=(0<<COM3A1) | (0<<COM3A0) | (0<<COM3B1) | (0<<COM3B0) | (0<<COM3C1) | (0<<COM3C0) | (0<<WGM31) | (0<<WGM30);
TCCR3B=(0<<ICNC3) | (0<<ICES3) | (0<<WGM33) | (0<<WGM32) | (0<<CS32) | (1<<CS31) | (1<<CS30);
TCNT3H=0x9E;
TCNT3L=0x58;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;
OCR3CH=0x00;
OCR3CL=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (1<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);
ETIMSK=(0<<TICIE3) | (0<<OCIE3A) | (0<<OCIE3B) | (1<<TOIE3) | (0<<OCIE3C) | (0<<OCIE1C);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: Off
// INT5: Off
// INT6: Off
// INT7: Off
EICRA=(0<<ISC31) | (0<<ISC30) | (0<<ISC21) | (0<<ISC20) | (0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
EICRB=(0<<ISC71) | (0<<ISC70) | (0<<ISC61) | (0<<ISC60) | (0<<ISC51) | (0<<ISC50) | (0<<ISC41) | (0<<ISC40);
EIMSK=(0<<INT7) | (0<<INT6) | (0<<INT5) | (0<<INT4) | (0<<INT3) | (0<<INT2) | (0<<INT1) | (0<<INT0);

// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud Rate: 9600
UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
UCSR0B=(1<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
UCSR0C=(0<<UMSEL0) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
UBRR0H=0x00;
UBRR0L=0x33;

// USART1 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART1 Receiver: On
// USART1 Transmitter: On
// USART1 Mode: Asynchronous
// USART1 Baud Rate: 38400
UCSR1A=(0<<RXC1) | (0<<TXC1) | (0<<UDRE1) | (0<<FE1) | (0<<DOR1) | (0<<UPE1) | (0<<U2X1) | (0<<MPCM1);
UCSR1B=(1<<RXCIE1) | (0<<TXCIE1) | (0<<UDRIE1) | (1<<RXEN1) | (1<<TXEN1) | (0<<UCSZ12) | (0<<RXB81) | (0<<TXB81);
UCSR1C=(0<<UMSEL1) | (0<<UPM11) | (0<<UPM10) | (0<<USBS1) | (1<<UCSZ11) | (1<<UCSZ10) | (0<<UCPOL1);
UBRR1H=0x00;
UBRR1L=0x0C;

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);

// ADC initialization
// ADC Clock frequency: 125.000 kHz
// ADC Voltage Reference: AVCC pin
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADFR) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);
SFIOR=(0<<ACME);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Graphic Display Controller initialization
// The ILI9341 connections are specified in the
// Project|Configure|C Compiler|Libraries|Graphic Display menu:
// DB8 - PORTA Bit 3
// DB9 - PORTA Bit 4
// DB10 - PORTE Bit 2
// DB11 - PORTE Bit 3
// DB12 - PORTE Bit 4
// DB13 - PORTE Bit 5
// DB14 - PORTE Bit 6
// DB15 - PORTE Bit 7
// RS - PORTF Bit 6
// /CS - PORTF Bit 5
// /RD - PORTA Bit 7
// /WR - PORTA Bit 6
// /RST - PORTA Bit 5

// Specify the current font for displaying text
glcd_init_data.font=font5x7;
// No function is used for reading
// image data from external memory
glcd_init_data.readxmem=NULL;
// No function is used for writing
// image data to external memory
glcd_init_data.writexmem=NULL;
// Horizontal display not reversed
glcd_init_data.reverse_x=1;
// Vertical display not reversed
glcd_init_data.reverse_y=0;
// Color bit writing order: BGR
glcd_init_data.cl_bits_order=ILI9341_CL_BITS_BGR;
// Use default value for pump ratio
glcd_init_data.pump_ratio=ILI9341_DEFAULT_PUMP_RATIO;
// Use default value for DDVDH enhanced mode
glcd_init_data.ddvdh_enh_mode=ILI9341_DEFAULT_DDVDH_ENH_MODE;
// Use default value for CR timing
glcd_init_data.cr_timing=ILI9341_DEFAULT_CR_TIMING;
// Use default value for EQ timing
glcd_init_data.cr_timing=ILI9341_DEFAULT_EQ_TIMING;
// Use default value for precharge_timing
glcd_init_data.precharge_timing=ILI9341_DEFAULT_PRECHARGE_TIMING;
// Use default value for VRH
glcd_init_data.vrh=ILI9341_DEFAULT_VRH;
// Use default value for VCOML
glcd_init_data.vcoml=ILI9341_DEFAULT_VCOML;
// Use default value for VCOMH
glcd_init_data.vcomh=ILI9341_DEFAULT_VCOMH;
// Use default value for VCOM offset
glcd_init_data.vcom_offset=ILI9341_DEFAULT_VCOM_OFFSET;
// LCD type normally white
glcd_init_data.lcd_type=ILI9341_LCD_TYPE_WHITE;
// LCD scan mode: interlaced
glcd_init_data.scan_mode=ILI9341_SCAN_INTERLACED;
// Frame rate: 70Hz
glcd_init_data.frame_rate=ILI9341_FRAME_RATE_70;

PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) ;

glcd_init(&glcd_init_data);
glcdsia_init(GLCD_CL_BLACK,GLCD_CL_WHITE,font5x7,false);
// Global enable interrupts
#asm("sei")
//strcpytoeep(nodeId,"lklklklklk");
//strcpytoeep(wifi_ssid,"BlackBerry");
//strcpytoeep(wifi_pass,"10203040");

puts1("AT\r\n");
glcd_display(0);
glcdsia_setbackclr(GLCD_CL_WHITE);
glcdsia_clear();
glcdsia_PutJPG(60,80,agrice_logo1);
glcd_display(1);
puts1("AT+RST\r\n");
wifiState = BUSY;
delay_ms(5000);
initialScreen();
//puts1("AT\r\n");
//wifiState = BUSY;
//while( wifiState==BUSY);
//delay_ms(1000);
set = 0;
back = 0;
DDRB.5 =1;
while (1)
      {  
      PORTB.5 = 0;  
      
      back = LCD_Touch_Read(); 
       while (set == 1){  
           if (recieved == 1 || first == 1) {   
           glcdsia_setforeclr(0x4007);  
           strcpyfromeep(wifi_ssid_ram,wifi_ssid);
           sprintf(lcdBuffer,"%25s  ",wifi_ssid_ram) ;
           glcdsia_outtextxy(45,60,lcdBuffer); 
           strcpyfromeep(wifi_pass_ram,wifi_pass); 
           sprintf(lcdBuffer,"%24s  ",wifi_pass_ram);       // don't have 32 space 
           glcdsia_outtextxy(80,90,lcdBuffer); 
           strcpyfromeep(nodeID_ram,nodeId);
           sprintf(lcdBuffer,"%26s ",nodeID_ram); 
           glcdsia_outtextxy(5,150,lcdBuffer);
    //       glcdsia_outtextxy(98,190,rx_buffer1);
           delay_ms(10);
           first = 0;
           recieved = 0;
           }
           if(wifiState == READY)
           { 
               if (networkState == CONNECTED)
                {
                   glcdsia_outtextxy(80,190,"CONNECTED       ");
                }
                else {
                   glcdsia_outtextxy(80,190,"NOT CONNECTED");    
                }
           }
       else {
        glcdsia_outtextxy(80,190,"Please wait...       ");
        //glcdsia_outtextxy(80,190,rx_buffer1);
       }
       // setting for bluetooth
       back = LCD_Touch_Read();
        if (back  == 2){
        initialScreen();
        back = 0;
        set = 0;
        } 
        else back = 3;  
        delay_ms(10);
       } 
       //default :no touch
       if (back  == 0) {
       if(seconds>= 2)  
        {  
          warning = sensors();  
          //sprintf(lcdBuffer,"%3d",seconds);
         // glcdsia_outtextxy(80,190,lcdBuffer);
          seconds =0; 
          }  
//        sprintf(lcdBuffer,"%3d  %2d",minutes,networkState);
//        glcdsia_outtextxy(80,190,lcdBuffer);
       if(minutes >= 2 && networkState == CONNECTED)
        {
         strcpyfromeep(nodeID_ram,nodeId);
         puts1("AT+CIPSTART=\"TCP\",\"148.251.224.230\",1010\r\n");    
         delay_ms(2000);
         puts(rx_buffer1);                             

         sprintf(httpHeader,"GET /api/v1/nodes/addDataToNode/%s/%0.1f/%0.1f/%0.1f/%0.1f/0/%d HTTP/1.1\r\nHost: 148.251.224.230:1010\r\n\r\n",nodeID_ram,humidity,temperature,soil,EC,light,phstring);
         sprintf(sendcommand,"AT+CIPSEND=%d\r\n",strlen(httpHeader));//
         puts1(sendcommand); 
         delay_ms(800);
         puts1(httpHeader);  
         seconds = 0;   
         minutes=0;
//            sprintf(lcdBuffer,"%3d  %2d",sendOK,networkState);
//           glcdsia_outtextxy(80,190,lcdBuffer);
         glcdsia_setfont(font5x7);
         
         if (serverResponse)
          {
           glcdsia_setforeclr(0xffff); 
           glcdsia_drawlineH2(10,yposInformation,180,8);
           glcdsia_setforeclr(0x0210);  
           glcdsia_outtextxy(10,yposInformation,"Data sent successfully. ");
           //glcdsia_outtext(sendDate);
          } 
          else { 
           glcdsia_setforeclr(0xffff); 
           glcdsia_drawlineH2(10,yposInformation,180,8);
           glcdsia_setforeclr(0x0210);
           glcdsia_outtextxy(10,yposInformation,"Failed to send data...         ");
          }  
          glcdsia_setforeclr(0x0210); 
          glcdsia_setfont(arial_narrow_bold);
         sendOK= false; 
         back=0;
        } 
       //glcdsia_outtextxy(65,230,rx_buffer1);
       }  
       else if ( networkState == DISCONNECTED )
       {
        glcdsia_setforeclr(0xffff); 
        glcdsia_drawlineH2(10,yposInformation,180,8);
        glcdsia_setforeclr(0x0210);  
        glcdsia_outtextxy(10,yposInformation,"WiFi NOT connected ");
          // glcdsia_outtext(sendDate);
       }
        // setting touched
        if (back  == 1){
        setting();
        set = 1;
        first = 1; 
        }
       
           
      }
}
void initialScreen()
{
 glcdsia_setbackclr(GLCD_CL_WHITE);
glcdsia_clear();//fill lcd using background color
glcdsia_setfont(arial_narrow_bold);
 glcd_display(0);
 glcdsia_setforeclr(0x0210);
 glcdsia_PutJPG(200,10,settings) ; 
 glcdsia_PutJPG(5,30,suns); 
 glcdsia_outtextxy(50,30,"Light:"); 
 glcdsia_outtextxy(50,30,"Light:");  
 glcdsia_PutJPG(10,80,hums1) ;
 glcdsia_outtextxy(50,80,"Humidity:"); 
 glcdsia_PutJPG(10,130,temps) ;
 glcdsia_outtextxy(50,130,"Temperature:"); 
 glcdsia_PutJPG(7,180,soils) ;
 glcdsia_outtextxy(50,180,"Soil moisture:"); 
 glcdsia_PutJPG(7,230,ecs) ;
 glcdsia_outtextxy(50,230,"EC (uS/cm):");        
 glcdsia_PutJPG(7,270,ph30_30) ;
 glcdsia_outtextxy(50,273,"pH:");     
 glcd_display(1);  
}
int sensors(){
float volt;//soil,temperature,humidity,EC,warning are defined in Global variables
char tempstring[10],humstring[10],soilstring[17],lightstring[10],ECstring[10];
glcdsia_setforeclr(0x4007);
 
       warning = 0; // green rgb is on 

       light = read_adc(3); 
       sprintf(lightstring,"%4d   ",light);
       glcdsia_outtextxy(150,30,lightstring);
       //if (light < 200  || light > 900 )warning = 1;  
       
      dht22_read(&temperature,&humidity);
//      ftoa(temperature,1,temp);
//      ftoa(humidity,1,hum);   
      sprintf(tempstring,"%0.1fC  ",temperature);
       sprintf(humstring,"%0.1f%%  ",humidity);  
       glcdsia_outtextxy(150,80,humstring); 
       glcdsia_outtextxy(150,130,tempstring);
       if (temperature < 20  || temperature > 50 )warning = 1;
       if (humidity < 20  || humidity > 80 )warning = 1;

      soil = (float)read_adc(0) ;
      volt = (float)soil /1023.0;
      volt = volt *5.0;
    //  soil = ( (soil) / (1023.0-10.0) )*100 ;   //pure water returns 1.05 volt  (205.0 adc)       
      //y = (ymax - ymin) * (x- xmin) / (xmax-xmin) + ymin    //xmin 
       soil = (100.0-0.0) *(soil-300.0) / (990.0-300.0)+0;// 4.9 volt in 0%humidity
     if (soil > 100) soil =  100 ; 
      else if(soil<0)  soil = 0;
      soil = 100.0-soil;
       
       pH = (7.0-4.8) *((float)read_adc(0)-210.0) / (305-210.0)-4.8;// 4.9 volt in 0%humidity   
       if (6<pH <8){
       pH =1;
      strcpy( phstring,"Neutral");
      } 
       else  {
       pH = 0;
       strcpy( phstring,"Not Neutral");
       }
    // if (pH > 14) pH =  14.0 ; 
    //  else if(pH<0)  pH = 0.0;
      
//      if (soil < 75)
//      {
//       pH=7.0;
//      }
      
       sprintf(soilstring,"%0.1f%%    ",soil );
       glcdsia_outtextxy(150,180,soilstring);
       if (soil < 10) warning = 1;
         
        EC = 5 - volt ;
      EC = EC *1375;
      EC = EC /(10* volt); 
        sprintf(ECstring,"%0.1f     ",EC);  
       glcdsia_outtextxy(145,230,ECstring); //
                                                    
       if (pH == 1)glcdsia_outtextxy(145,273,"Neutral");  
       else glcdsia_outtextxy(145,273,"Not Neutral"); 
       //sprintf(ECstring,"%0.3f  ",phstring);//pH  
       //glcdsia_outtextxy(145,273,ECstring); //
      // if (EC < 34  || EC > 550 )warning = 1; 
       return warning;
}


void setting (){
          
        glcdsia_setbackclr(GLCD_CL_WHITE);
        glcdsia_clear();//fill lcd using background color
        glcdsia_setfont(arial_narrow_bold);
        glcdsia_setforeclr(0x0210);
        glcd_display(0);
        glcdsia_outtextxy(10,20,"Network Settings:");  
        glcdsia_outtextxy(10,60,"SSID:"); 
        glcdsia_outtextxy(10,90,"Password: ");
        glcdsia_outtextxy(10,120,"NodeID : ");
        glcdsia_outtextxy(10,190,"Status :");
        glcdsia_outtextxy(10,220,"Bluetooth name : AGRI01"); 
        glcdsia_outtextxy(10,250,"Bluetooth pass : 1234");
        glcdsia_PutJPG(10,280,backs) ;
        glcd_display(1);
}

void wifi_reconnect()
{
//char wifiModemPropstr[
delay_ms(20);
strcpyfromeep(wifi_ssid_ram,wifi_ssid);
strcpyfromeep(wifi_pass_ram,wifi_pass); 
sprintf(lcdBuffer,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_ssid_ram,wifi_pass_ram);
//glcdsia_outtext("gsgdh");
//glcdsia_outtext(wifi_ssid_ram);
puts1(lcdBuffer);
//glcdsia_outtext( lcdBuffer);
wifiState = BUSY;
rx_wr_index1=0;
}

void sendToServer()
{
//  puts("AT+CIPSTART=\"TCP\",\"148.251.224.230\",1010\r\n");
//  seconds = 0; 
//  wifiState = BUSY; 
//  while(  wifiState ==  BUSY && seconds<20);
//  
//  sprintf(httpHeader,"GET /api/v1/nodes/addDataToNode/60d42081638da98da9f49340/55/15/55/5/5/11 HTTP/1.1\r\nHost: 148.251.224.230:1010\r\n\r\n");
//  sprintf(sendcommand,"AT+CIPSEND=%d\r\n",strlen(httpHeader));//+1 baraye hazfe \n ast chon khode puts \n ra mifreste
//  puts1(sendcommand); 
}