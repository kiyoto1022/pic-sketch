#include <xc.h>
#include <stdio.h>
#include <string.h> // memcpy
#include <stdlib.h>

#define _XTAL_FREQ 4000000
#define RF_DATA_SIZE 13
#define TRANSMIT_REQUEST 0x10 // frame type

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
void send(unsigned char);
unsigned char checksum(unsigned char*);

typedef struct api_frame {
    unsigned char start_delimiter;
    unsigned char length_msb;
    unsigned char length_lsb;
    unsigned char frame_type;
    unsigned char frame_id;
    unsigned char dest_address64[8];
    unsigned char dest_address16[2];
    unsigned char broadcast_radius;
    unsigned char options;
    unsigned char rf_data[RF_DATA_SIZE];
    unsigned char checksum;
} api_frame;

void main(void) {
    CMCON = 0x07; // Comprator Mode bits
    TRISA = 0x00;
    TRISB = 0b00000110; // Configures RB1/RX/DT and RB2/TX/CK pins as serial port pins when bits TRISB<2:1> are set
    PORTA = 0x00;
    PORTB = 0x00;

    init_uart();

    api_frame api_frame;
    api_frame.start_delimiter = 0x7E;
    // length = api_frame - (start_delimiter + length_msb + length_lsb + checksum)
    api_frame.length_msb = (sizeof(api_frame) -4) & 0xFF00;
    api_frame.length_lsb = (sizeof(api_frame) -4) & 0x00FF;
    api_frame.frame_type = TRANSMIT_REQUEST;
    api_frame.frame_id = 0x01;
    unsigned char address64[8] = { 0x00, 0x13, 0xA2, 0x00, 0x41, 0x7D, 0x55, 0x55 };
    memcpy(api_frame.dest_address64, address64, 8);
    unsigned char address16[2] = { 0xFF, 0xFE };
    memcpy(api_frame.dest_address16, address16, 2);  
    api_frame.broadcast_radius = 0x00;
    api_frame.options = 0x00;
    memcpy(api_frame.rf_data, "HELLO 16F628A", RF_DATA_SIZE);
    
    unsigned char *frame_data = (unsigned char *)&api_frame;

    api_frame.checksum = checksum(frame_data);
    
    while(1){
        for (int i = 0; i < sizeof(api_frame); i++) {
           send(frame_data[i]);
        }
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

void send(unsigned char data){
    while(!TXIF){
        continue;
    }
    TXREG = data;
}

unsigned char checksum(unsigned char *frame_data) {
    const int FRAME_TYPE_POSITION = 3;
    const int CHECK_SUM_SIZE = 1;
    
    int sum = 0;
    for (int i = FRAME_TYPE_POSITION; i < sizeof(api_frame) - CHECK_SUM_SIZE; i++) {
        sum += frame_data[i];
    }
    return 0xFF - (sum & 0x00FF);
}
