#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000
#define INTERRUPTS_COUNT_PER_SECOND 15
#define ONES_DIGIT_PIN RA0
#define TENS_DIGIT_PIN RA1

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

void init_tmr0();
void count_up();

// 1 2 4 6 7 9 10 dummy
// output pattern of 7-segment LED
unsigned char OUTPUT_PATTERN[10] = {
    0b00000011,   // 0
    0b11001111,   // 1
    0b00100101,   // 2
    0b10000101,   // 3
    0b11001001,   // 4
    0b10010001,   // 5
    0b00010001,   // 6
    0b11000011,   // 7
    0b00000001,   // 8
    0b11000001    // 9
};

int tmr0_counter = INTERRUPTS_COUNT_PER_SECOND;
int counter = 0;

void count_up(void)
{
    counter++;
    if (counter >= 60) {
        counter = 0;
    } 
}

void __interrupt() irt(void)
{
    T0IF = 0;
    tmr0_counter--;
    if (tmr0_counter == 0) {
        tmr0_counter = INTERRUPTS_COUNT_PER_SECOND;
        count_up();
    }
}

void init_tmr0()
{
    OPTION_REG = 0b10000111; // Prescaler Rate Select bits 111 -> 1 : 256
    TMR0 = 0x00;
    INTCON = 0b10100000; //GIE(Global Interrupt Enable bit):1, T0IE(TMR0 Overflow Interrupt Enable bit):1
}

void main(void){
    CMCON = 0x07; // Comparator Mode bits
    TRISA = 0x00;
    TRISB = 0x00; 
    
    init_tmr0();   
    
    while(1) {
        int ones_digit = counter % 10;
        int tens_digit = counter / 10;

        ONES_DIGIT_PIN = 1;
        TENS_DIGIT_PIN = 0;
        PORTB = OUTPUT_PATTERN[ones_digit];
        __delay_ms(8);
       
        ONES_DIGIT_PIN = 0;
        TENS_DIGIT_PIN = 1;
        PORTB = OUTPUT_PATTERN[tens_digit];
        __delay_ms(8);
    }
}
