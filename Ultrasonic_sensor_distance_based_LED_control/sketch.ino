#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#define delay _delay_ms

// Pin definitions:
#define LED1  PB0   //(8)
#define LED2  PB1   //(9)
#define TRIG1 PD2   //(2)
#define ECHO1 PD3   //(3)
#define TRIG2 PD4   //(4)
#define ECHO2 PD5   //(5)

int main(void) {
// Setting LED pins pthways as output:
    DDRB |= (1 << LED1) | (1 << LED2);
    
    // Setting trigger pins pathways as outputs, no need to set echo pins as they are required to be low and by default they are
    DDRD |= (1 << TRIG1) | (1 << TRIG2);    // Trigger pins as outputs
  
    
    // Initialize Timer1 in normal mode with a prescaler of 8:
    TCCR1A = 0x00;         // Normal mode, no PWM
    TCCR1B = (1 << CS11);  // Prescaler = 8; 0.5 µs per count at 16MHz
    TIMSK1 = 0x00;         // Disable all Timer1 interrupts

    while(true) {
        float dist1, dist2;
        uint16_t counts;
        float pulse_us;
//Sensor 1
        PORTD |= (1 << TRIG1);
        _delay_us(10);
        PORTD &= ~(1 << TRIG1);
        
        // Wait for echo to go HIGH:
        while (!(PIND & (1 << ECHO1)));
        
        // Reset Timer1:
        TCNT1 = 0;
        
        // Wait for echo to go LOW:
        while (PIND & (1 << ECHO1));
        
        // Read timer count:
        counts = TCNT1;
        pulse_us = counts * 0.5f;     // Each tick is 0.5 µs
        dist1 = (pulse_us * 0.0343f) / 2.0f;  // Distance in cm
        
       
// Sensor2 
       
        PORTD |= (1 << TRIG2);
        _delay_us(10);
        PORTD &= ~(1 << TRIG2);
        //repeat the same high , reset timer and wait for it to go low
        while (!(PIND & (1 << ECHO2)));
        TCNT1 = 0;
        while (PIND & (1 << ECHO2));
        
        counts = TCNT1;
        pulse_us = counts * 0.5f;
        dist2 = (pulse_us * 0.0343f) / 2.0f;
        
        
// Compare distances & set LEDs:
        
        float minimum = 1.0f;  // 1 cm minimum difference
        if ((dist1 - dist2) > minimum) {       // Sensor1 > Sensor2
            PORTB |= (1 << LED1);          // Turn on LED1
            PORTB &= ~(1 << LED2);         // Turn off LED2
        } else if ((dist2 - dist1) > minimum) {  // Sensor2 > Sensor1
            PORTB |= (1 << LED2);          // Turn on LED2
            PORTB &= ~(1 << LED1);         // Turn off LED1
        } else {
            PORTB &= ~((1 << LED1) | (1 << LED2)); // Turn off both LEDs
        }
        
        _delay_ms(200); // Wait a bit before next measurement
    }
    
    return 0;
}
