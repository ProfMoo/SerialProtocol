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
#include <stdbool.h>

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

//#define SCL PORTCbits.RC1

int Count = 0;
uint8_t received = 0;
uint8_t counter = 0;
uint8_t readin = 0;

void timerInit(void) {
//    OPTION |= 0b00010111; //setting prescaler value to 1:256
//    OPTION &= 0b11000111; //prescaler assigned to tmr0
//                          //tmr0 clock source select bit to internal.
//                          //increments on high to low
    
    asm("MOVLW 0x07");	    // LOAD W    
    asm("OPTION");          // LOAD TRIS PORTA 
    
    //enabling interrupts and overflows for TMR0
    INTCONbits.T0IF = 1;
    INTCONbits.GIE = 1;
}

void portInit(void) {       
    ADCON0 = 0xA0; // ADC clock source is INTOSC/4
                    // ADC not running
                    // ADC enabled
    PORTC = 0x00; // init bits to low
    
    TRISC = 0XF0; // Set 7-4 as input, 3-0 as output
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

uint8_t readByte(void) {
    uint8_t i = 0;
    uint8_t newbyte = 0;
    while (i < 8) {
        while (PORTCbits.RC5 == 1); //wait for the clock to go low
            
        readin = PORTBbits.RB6;
        
        //confirming a bits low or high
        if (readin == 1) {
            PORTCbits.RC1 = 1;
        }
        if (readin == 0) {
            PORTCbits.RC1 = 0;
        }
  
        newbyte = newbyte >> 1;
        if (readin == 1) {
            newbyte |= 0x80;
        }
//        newbyte = newbyte << 1; //leftshift
//        newbyte += readin; //add the bit to the unsigned int
        
        while (PORTCbits.RC5 == 0); //wait for the clock to go back high

        i += 1;
    }
    PORTCbits.RC1 = 0; //resetting data received line
    return newbyte;
}

bool read_clock(void) {
    if (PORTCbits.RC5 == 1) {
        return true;
    }
    if (PORTCbits.RC5 == 0) {
        return false;
    }
}

bool read_data(void) {
    if (PORTBbits.RB6 == 1) {
        return true;
    }
    if (PORTBbits.RB6 == 0) {
        return false;
    }
}

void start_read(void) {
    uint8_t counter = 0;
    uint8_t target = 0;
    uint8_t command = 0;
    uint8_t checksum = 0;
    
    counter = readByte();
    target = readByte();
    command = readByte();
    checksum = readByte();
//    if (counter == 0xAF) {
//        PORTCbits.RC3 = 1;
//    }
//    if (target == 0x02) {
//        PORTCbits.RC3 = 1;
//    }
//    if (command == 0xAA) {
//        PORTCbits.RC3 = 1;
//    }
    if (checksum == 0x45) {
        PORTCbits.RC3 = 1;
    }
}

void main(void) {
    portInit();
    timerInit();
    
    uint8_t testerino;
    uint8_t test2;

    while (1) {
        //check for start
        while (read_data() == false);
        if (read_clock() == false) {
//            testerino = readByte();
//            test2 = readByte();
//            if (test2 == 0x03) {
//                PORTCbits.RC3 = 1;
//            }
            start_read();
        }
        else {
            continue;
        }
    }
}