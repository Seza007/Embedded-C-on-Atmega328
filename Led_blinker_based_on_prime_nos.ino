#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define LED_PIN PB5 //13

void initialize_uart() {
    UBRR0H = 0;        
    UBRR0L = 103;       // assigning baudrate 9600 for 16MHz
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);   //enabling RX and TX bit flags
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Setting max bits to be sent as 8
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));   //hold until UDR is free (USART Data Register)
    UDR0 = data;
}

void uart_print(const char *str) {
    while (*str != '\0') {               //transmitting except for null character
        uart_transmit(*str++);
    }
}

int uart_read_int() {     //takes up ASCII value from UDR processes it to int
    char buffer[10];
    int index = 0;
    char received;
    while (true) {
        while (!(UCSR0A & (1 << RXC0)));    //holding until recived flag is set to notify previous receiving is finished
        received = UDR0;
        if (received == '\n' || received == '\r')
            break;
        buffer[index++] = received;
        if (index >= 9)
            break;
    }
    buffer[index] = '\0';
    return atoi(buffer);
}

bool isPrime(int n) {  //checks prime
    int cnt = 0;
    if (n <= 1)
        return false;
    for (int i = 1; i <= n; i++) {
        if (n % i == 0)
            cnt++;
    }
    if (cnt > 2)
        return false;
    else
        return true;
}

int main(void) {
    DDRB = (1 << LED_PIN);
    initialize_uart();
    uart_print("UART Initialized. Send a number.\n");

    while (true) {
        int inputNumber = uart_read_int();
        char msg[30];
        sprintf(msg, "Received: %d\n", inputNumber);    // writes input number to msg[] as string
        uart_print(msg);
        if (isPrime(inputNumber)) {
            uart_print("Prime number detected. Blinking LED.\n");
            for (int i = 0; i < 5; i++) {   //makes it blink 5 time
                PORTB = (1 << LED_PIN);  
                _delay_ms(500);
                PORTB = 0;               
                _delay_ms(500);
            }
        } else {
            uart_print("Not a prime number.\n");
        }
    }
    return 0;
}