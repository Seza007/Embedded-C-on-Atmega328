#include <avr/io.h>
#include <util/delay.h>


void initialize_uart(unsigned int baud) {
    unsigned int ubrr = F_CPU / 16 / baud - 1;  // Calculate baud rate register value
    UBRR0H = (unsigned char)(ubrr >> 8);       
    UBRR0L = (unsigned char)ubrr;              //set baud rate
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);      // Enable TX and RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    // 8 data bits 1 stop bit
}

void uart_tx(char data) {
    while (!(UCSR0A & (1 << UDRE0)));  // holding off until udr free flag set
    UDR0 = data;  
}

char uart_rx(void) {
    while (!(UCSR0A & (1 << RXC0)));  // holding off until receive over flag
    return UDR0; 
}

void uart_print(const char *str) {
    while (*str != '\0') {  // Keep slicing and transmitting characters until we hit null
        uart_tx(*str++);
    }
}

int main(void) {
    initialize_uart(9600); 
    uart_print("UART Initialized. Send me something!\r\n");  

    char received_char;
    while (1) {  /
        received_char = uart_rx();  // Grab the incoming char
        uart_tx(received_char);     // tx it back

        if (received_char == '\n') {  // newline stopping
            uart_print(" - Echo Received! Type again.\r\n");  
        }
    }
}
