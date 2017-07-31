/*
 * File:   main.c
 * Author: drmoo
 *
 * Created on July 26, 2017, 1:58 PM
 */


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f527.h>
#include <stdint.h>  

 // CONFIG     
 #pragma config FOSC = INTRC_IO  // Oscillator Selection (INTRC with I/O function on OSC2/CLKOUT and 10 us startup time)     
 #pragma config WDTE = OFF       // Watchdog Timer Enable (WDT Disabled)     
 #pragma config CP = OFF          // Code Protection - User Program Memory (Code protection off)     
 #pragma config MCLRE = OFF      // Master Clear Enable (MCLR pin functions as I/O, MCLR internally tied to Vdd)     
 #pragma config IOSCFS = 8MHz    // Internal Oscillator Frequency Select (8 MHz INTOSC Speed)     
 #pragma config CPSW = ON        // Code Protection - Self Writable Memory (Code protection on)     
 #pragma config BOREN = ON       // Brown-out Reset Enable (BOR Enabled)     
 #pragma config DRTEN = ON      // Device Reset Timer Enable (DRT Enabled) 

 #ifndef _XTAL_FREQ
 #define _XTAL_FREQ 8000000 //8Mhz FRC internal osc
 #define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/8000000.0)))     
 #define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/8000.0)))
 #endif

void interrupt tc_int(void) {
//    TMR0bits.TMR0 = 0;
//    TRISC = 0xFF;
}

void timerInit(void) {
//    OPTION |= 0b00010111; //setting prescaler value to 1:256
//    OPTION &= 0b11000111; //prescaler assigned to tmr0
//                          //tmr0 clock source select bit to internal.
//                          //increments on high to low
    
    //possible try this:
    //asm(CLRWDT);
    asm("MOVLW 0x07");	    // LOAD W    
    asm("OPTION");          // LOAD OPTION 
    
    //enabling interrupts and overflows for TMR0
    INTCONbits.T0IF = 1;
    INTCONbits.GIE = 1;
}

void portInit(void) {       
    ADCON0 = 0xA0; // ADC clock source is INTOSC/4
                    // ADC not running
                    // ADC enabled
    PORTC = 0x00; // init bits to low
    
    TRISC = 0X00; // Set all PORT C bits to output
    TRISB = 0XFF; // Set all PORT B bits to input
    
    ANSEL = 0x00; //set all analog pins to digital mode
    ANSELbits.ANS0 = 0;
    ANSELbits.ANS1 = 0;
    ANSELbits.ANS2 = 0;
    ANSELbits.ANS3 = 0;
    ANSELbits.ANS4 = 0;
    ANSELbits.ANS5 = 0;
    ANSELbits.ANS6 = 0;
    ANSELbits.ANS7 = 0;
    
    //disable opamp
    OPACONbits.OPA1ON = 0;
    OPACONbits.OPA2ON = 0;
    
    //disable comparator
    CM2CON0bits.C2ON = 0;
    
//    PORTCbits.RC5 = 1; //raise C.5.
//    PORTCbits.RC1 = 1; //raise C.1.
}

int Count = 0; 

void sendByte(uint8_t byte2send) {
    int i = 0;
    while (i < 8) {  
        if (byte2send%2 == 0) {
            PORTCbits.RC0 = 0; //updating data line
        }
        if (byte2send%2 == 1) {
            PORTCbits.RC0 = 1; //updating data line
        }
        byte2send = byte2send >> 1;
        
        __delay_us(5);
        
        TRISC = 0x00; //setting clock low, setting as output
        PORTCbits.RC5 = 0;
        __delay_us(50);
        TRISC = 0xF0; //setting clock high, setting as input
        __delay_us(50);
        i += 1;
    }
    
}

void main(void) {
    portInit();
    timerInit();
  
    PORTCbits.RC5 = 0;
    TRISC = 0xF0;
    
    //code to send a byte
    while(1) {
        TRISC = 0x00; //setting clock low, setting as output
        PORTCbits.RC5 = 0;
        __delay_ms(1);
        //__delay_us(1500);
        TRISC = 0xF0; //setting clock high, setting as input
        __delay_ms(1);
        
        
        sendByte(0x36);
        __delay_ms(50);
    }
    
//    //code for testing large line pulldown
//    while(1) {
//        if (PORTBbits.RB6 == 0) {
//            TRISC = 0xFF; //setting clock high, setting as input
//
//        }
//        if (PORTBbits.RB6 == 1) {
//            TRISC = 0x00; //setting clock low, setting as output
//        }
//    }
    
    //code to toggle RC5 as RUI talked about
//    while(1) {
//        while(!TMR0bits.TMR0);
//        //PORTCbits.RC5 = 1;
//        TMR0bits.TMR0 = 0;
//        Count ++;
//        if (Count == 10000) {
//            TRISC = 0xFF;
//            //PORTCbits.RC5 = 0;
//        }
//        if (Count == 20000) {
//            TRISC = 0x00;
//            //PORTCbits.RC5 = 1;
//            Count = 0;
//        }
//    }
    
    
    //CODE FOR 2ms, 2Hz
//    while(1) {     
//        while(!TMR0bits.TMR0);
//        //PORTCbits.RC5 = 1;
//        TMR0bits.TMR0 = 0;
//        Count ++;
//        if (Count == 14) {
//            PORTCbits.RC5 = 1;
//        }
//        if (Count == 3650) {
//            PORTCbits.RC5 = 0;
//            Count = 0;
//        }
//    }
}