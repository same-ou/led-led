#define F_CPU 16000000UL
#include <avr/io.h>
#include "../common/usb.h"
#include <util/delay.h>

// Constants
#define ledAnode PD1 // LED-A to D2 through 210R series resistor
#define ledCathode PF7 // LED-K to A0
#define threshold 800 // maximun measured time for discharging the led when it exposes to light

/** Set a specific pin of a port to a low state (logic 0). */
#define out_low(port, pin)  (port) &= ~(1 << (pin))
/** Set a specific pin of a port to a high state (logic 1). */
#define out_high(port, pin) (port) |= (1 << (pin))

// Macro to set a pin as an input
#define SET_PIN_INPUT(port, pin)    ((port) &= ~(1 << (pin)))
// Macro to set a pin as an output
#define SET_PIN_OUTPUT(port, pin)   ((port) |= (1 << (pin)))

/*
  function prototypes
  1. init_adc - initializes the adc
  2. adc_read - reads the adc value from the given channel
  3. get_led - returns true if the ldr is above the threshold, otherwise false
  4. get_byte - returns the byte from the bits decoded by get_ldr
  */
void init_adc();
uint16_t adc_read(uint8_t ch);
int get_led();
char get_byte();

int main(void){
  init_adc();
  usb_init();
  // init pins as output
  SET_PIN_OUTPUT(DDRD,ledAnode);
  SET_PIN_OUTPUT(DDRF,ledCathode);
  
  while (1)
  { 
    // read the byte
    char byte = get_byte();
    usb_tx_char(byte);
  }
  return 0;
}

void init_adc(void) {
  // SET AVCC as reference
  ADMUX |= (1<<REFS0);
  // SET prescaler to 128
  ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

  // turn on ADC
  ADCSRA |= (1<<ADEN);
  ADCSRA |= (1<<ADSC);
}

uint16_t adc_read(uint8_t ch) {
  // select channel
  ADMUX &= 0xF0;
  ADMUX |= ch;

  // start conversion
  ADCSRA |= (1<<ADSC);

  // wait for conversion to complete
  while(!(ADCSRA & (1<<ADIF)));

  // clear ADIF by writing 1 to it
  ADCSRA |= (1<<ADIF);

  return (ADC);
}

char get_byte() {
  // Decode a byte of data
  // Wait for the transmission to begin
  // If the state of the LED is changed from HIGH TO LOW, then start the conversion
  while (get_led());

  char collectedByte = 0;
  for (int i = 7; i >= 0; i--) {
    // Read the bit from the LED
    collectedByte |= get_led() << i;
  }
  return collectedByte;
}

int get_led(){
  // Get the state of the LED
  // This function will be used to establish synchronization between the two LEDs
  // First of all, let's put the LED in reverse bias mode

  // Reverse Bias the LED
  int result = 1;
  out_low(PORTD,ledAnode);
  out_high(PORTF,ledCathode);
  _delay_ms(100);

  SET_PIN_INPUT(DDRF,ledCathode);

  // Wait for the time expected to discharge the LED
  _delay_ms(threshold);
  

  if (adc_read(ledCathode) != 0) {
    // Discharge in 800 ms
    result =  0;
  }
  SET_PIN_OUTPUT(DDRF, ledCathode);
  return result;
}