#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

void init_uart(void);

void putch(char data){
    while(!TXIF){
        continue;
    }
    TXREG = data;
}

void main(void) {
    CMCON = 0x07; // Comprator Mode bits
    TRISA = 0x00;
    TRISB = 0b00000110; // Configures RB1/RX/DT and RB2/TX/CK pins as serial port pins when bits TRISB<2:1> are set
    PORTA = 0x00;
    PORTB = 0x00;

    init_uart();

    while(1){
        printf("HELLO WORLD\n");
        __delay_ms(1000);
    }
}

void init_uart(){
    // bit7 SPEN: Serial Port Enable bit
    RCSTA = 0b10000000;
    // bit5 TXEN: Transmit Enable bit
    // bit4 SYNC: USART Mode Select bit 0(SYNC: USART Mode Select bit)
    // bit2 BRGH: High Baud Rate Select bit 1(High speed)
    TXSTA = 0b00100100;
    // USART Baud Rate Generator ASYNCHRONOUS MODE (BRGH = 1) 9600bps 4MHz
    SPBRG = 25;
}
