#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define RED_LED_PIN PB5  //13
#define GREEN_LED_PIN PD7 // (7)
#define delay _delay_ms

#define Number_of_mappings (sizeof(mappings)-1)

void initialize_uart() {
    UBRR0H = 0;        
    UBRR0L = 103;       // assigning baudrate 9600 for 16MHz
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);   //enabling RX and TX bit flags
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Setting max bits to be sent as 8
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));   //holds on until UDR is free (USART Data Register)
    UDR0 = data;
}

void uart_print(const char *str) {
    while (*str != '\0') {               //transmitting except for null character
        uart_transmit(*str++);
    }
}

int uart_read_ip(char *buffer, int max_length){     //takes up ASCII value from UDR processes it to str
    int index = 0;
    char received;
    while (index < max_length - 1) {
        while (!(UCSR0A & (1 << RXC0)));     //holding until recived flag is set to notify previous receiving is finished
        received = UDR0;
        buffer[index++] = received;
        if (received == '\n' || received == '\r')
            break;        
    }
    buffer[index] = '\0';
}
// Mapping array: characters from A-Z and digits 0-9.
const char mappings[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 

// Morse code array: corresponding Morse strings.
const char *morse[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", // A-H
    "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.",    // I-P
    "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-",   // Q-X
    "-.--", "--..",                                           // Y-Z
    "-----", ".----", "..---", "...--", "....-", ".....",      // 0-5
    "-....", "--...", "---..", "----."                        // 6-9
};
// '.' -> red LED on (PB5) for 10ms,
// '-' -> green LED on (PD7) for 30ms.
char *out_morse(char ch){   // gets morse output for provided input by comparing with morse array and retrieve the same index from mappings array
    ch=toupper(ch);
    for(int i=0; i < Number_of_mappings; i++){
        if(mappings[i]==ch){
            return morse[i];
        }
    }
    return "";
}

void led_morse(const char *code){ //switches respective led on for respective morse sequence
    for(int i = 0; code[i] != '\0'; i++){
        if(code[i] == '.'){
            PORTB = 1 << 5;
            delay(100);         //inorder to see proper output in simulation delay increased from 10 to 100
            PORTB = 0;
        }
        else if(code[i] == '-'){
            PORTD = 1 << 7;
            delay(300);        //inorder to see proper output in simulation delay increased from 30 to 300
            PORTD = 0;
        }
    delay(50);   //general delay btw symbols
    }
}

int main(void){
    DDRB = 1 << RED_LED_PIN;
    DDRD = 1 << GREEN_LED_PIN;
    initialize_uart();
    uart_print("UART Initialized. Send your sentence");
    char input[100];   //initialized "input" variable to process serial monitor input
    while(true){
        uart_read_ip(input, 100);
        uart_print("\n morse output:");
        for(int i=0; input[i] != '\0'; i++){   // if space is inputed both leds light up to show it
            if(input[i] == ' '){
                PORTB = 1 << RED_LED_PIN;
                PORTD = 1 << GREEN_LED_PIN;
                delay(500);    //inorder to see proper output in simulation delay increased from 50 to 500
                PORTB = 0;
                PORTD = 0;
            }
            else{
                char *code = out_morse(input[i]);
                led_morse(code);
                delay(500);   //inorder to see proper output in simulation delay increased from 50 to 500
            } 
        }
    uart_print("\n");
    }
return 0;
}


