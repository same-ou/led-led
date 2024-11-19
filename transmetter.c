#include <avr/io.h>
#include <util/delay.h>
#include "../common/usb.h"

#define F_CPU 16000000UL

// define led pin
#define LED_PIN PD1
#define INTERVAL 900

// marcos for turning on and off led
#define out_low(port,pin)   (port) &= ~(1 << (pin))
#define out_high(port,pin)  (port) |= (1 << (pin))

/*
    function prototypes
    1. init_led: initialize led pin as output
    2. sendByte: send a byte to the receiver
    3. sendString: send a string to the receiver
*/
void init_led();
void sendByte(char byte);
void sendString(char *string);


int main() {
    init_led();
    usb_init();
    // the led is turned on by default
    out_high(PORTD, LED_PIN);
    _delay_ms(30000);

    while (1)
    {
        sendString("Hello World!");
        _delay_ms(30000);
    }
    return 0;
}


void init_led() {
    // set the led pin as output
    DDRD |= (1 << LED_PIN);
}

void sendByte(char byte) {
    // inform the receiver that a byte is coming
    // BOF 
    out_low(PORTD, LED_PIN);
    _delay_ms(INTERVAL);

    // send Data
    usb_print("Transmitting byte...\n\r");
    for (int j = 7; j >= 0; j--) {
        if(byte & (1 << j)) {
            out_high(PORTD, LED_PIN);
            usb_tx_char('1');
        } else {
            out_low(PORTD, LED_PIN);
            usb_tx_char('0');
        }
        _delay_ms(INTERVAL);
    }
    usb_print("\n\r");
    // inform the receiver that the byte is over
    out_high(PORTD, LED_PIN);
    _delay_ms(INTERVAL);
}

void sendString(char *string) {
    for (int i = 0; string[i] != '\0'; i++) {
        sendByte(string[i]);
    }
}
