#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>  
#include <stdio.h>

void initialize_uart(unsigned int baud) {
    unsigned int ubrr = F_CPU / 16 / baud - 1;
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);  // Enable TX and RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data
}

void uart_tx(char data) {
    while (!(UCSR0A & (1 << UDRE0)));  // holding off until udr free flag set
    UDR0 = data;
}

char uuart_rx(void) {
    while (!(UCSR0A & (1 << RXC0)));  // holding off until receive over flag
    return UDR0;
}

void uart_print(const char *str) {  // slicing and transmitting till null character
    while (*str != '\0') {
        uart_tx(*str++);
    }
}

// Reading float from UART
float get_from_UART() {
    char buffer[10];
    uint8_t index = 0;
    char received;

    // Read characters until newline or buffer full
    while (index < 9) {
        received = uuart_rx();
        if (received == '\r' || received == '\n') break;
        buffer[index++] = received;
        uart_tx(received);  // Echo back
    }
    buffer[index] = '\0';  // terminating string at null

    return atof(buffer);  // Converting to float
}

// GCD function (without swap check)
float gcd(float a, float b) {
    while (b > 0.0001) {  // thresholding
        float remainder = fmod(a, b);
        a = b;
        b = remainder;
    }
    return a;
}

int main(void) {
    // Initialize UART with 9600 baud rate
    initialize_uart(9600);

    uart_print("Enter first float:\r\n");
    float num1 = get_from_UART();

    uart_print("\nEnter second float:\r\n");
    float num2 = get_from_UART();

    // Compute GCD
    float result = gcd(num1, num2);

    // Convert result to string (workaround for missing printf float support)
    char resultStr[10];
    dtostrf(result, 4, 2, resultStr);  // Convert float to string

    // Print result
    uart_print("\nGCD: ");
    uart_print(resultStr);

    while (true);  // Infinite loop
}
