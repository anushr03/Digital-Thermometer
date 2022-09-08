/*****************************************************************************
* Thermometer.c - This program mearsures the temperature of the surrounding using
* TMP36 sensor and outputs the tempearture on a 7segment led screen. A push button,
* when pressed, displays the temperature from Farenheit to degree Celcius.

* Date          Author          Revision
* 03/27/2022    Anush Rathod    Initial write of code
* 03/28/2022	Anush Rathod	Making version 1 of the assignment. Displaying the temperature to a serial monitor
* 03/29/2022	Anush Rathod	Setting up the led screen and get temp value on the screen
* 03/30/2022    Anush Rathod    Adding comments and debugging the code
* 04/01/2022    Anush Rathod    Making finals edits to the code

* REFERENCES
* ECE 231 Spring 22, Lecture slides 11, 12, 13, 14, Author - David McLaughlin
******************************************************************************/


#include <avr/io.h>
#include<util/delay.h>
#include<stdlib.h>
#include<string.h>
#define persistance 5
#define counttime 200

void adc_init(void);
unsigned int get_adc();

int main(void){

    double digitalValue, Cel2Far, Celcius; 


    adc_init();
    DDRD = 0xFF; // all 7 segments
    DDRB = 0XFF; // Digit enable pins
    PORTB = 0x0F; // Disabling the digits initially
    PORTC = 1<<PORTC2; // Initializing pinC2 as internal Pullup Resistor

    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; // digits 0 to 9 in hex values
    unsigned char ledDigWithDec[] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0XFD, 0X87, 0XFF, 0XE7}; // digits 0 to 9 with decimal point in hex values
    unsigned char DIG1, DIG2, DIG3, DIG4;
    unsigned int i = 0;
    unsigned char unit[] = {0x71, 0x39}; // hex values for 'F' and 'C' respectively


    while (1){

        digitalValue = get_adc(); // getting the ADC value from the TMP36 sensor. The value is in mV

        Celcius = (digitalValue-750)/10 + 25; // converting the digitalValue to C

        if  ((PINC & (1<<PINC2)) == 0){ // checking if the button is pressed 
            
            i = Celcius * 10;

            // extracting each digit of the temperature value
            DIG1 = (i/100)%10; // value at 100s place of temperature
            DIG2 = (i/10) % 10; // value at 10s place of temperature
            DIG3 = i%10; // value at 1s place of temperature
            DIG4 = 1; // having the correct unit sign for DIG 4.

            for (int j=0; j<counttime/persistance/4; j++){
                
                PORTD = unit[DIG4];
                PORTB =~ (1<<1);
                _delay_ms(persistance);

                PORTD = ledDigits[DIG3];
                PORTB =~ (1<<2);
                _delay_ms(persistance);

                PORTD = ledDigWithDec[DIG2];
                PORTB =~ (1<<3);
                _delay_ms(persistance);

                PORTD = ledDigits[DIG1];
                PORTB =~ (1<<4);
                _delay_ms(persistance);

                PORTB = 0xFF;
            }


        }
        else{ // if the button is not pressed, the serial monitor will show temp in Farenheit

            Cel2Far = Celcius*9.0/5.0+32.0; // converting Celcius to Farenheit
            i = Cel2Far *10;

            DIG4 = 0;
            DIG3 = i%10;
            DIG2 = (i/10) % 10; 
            DIG1 = (i/100)%10;

            for (int j=0; j<counttime/persistance/4; j++){
                
                PORTD = unit[DIG4];
                PORTB =~ (1<<1);
                _delay_ms(persistance);

                PORTD = ledDigits[DIG3];
                PORTB =~ (1<<2);
                _delay_ms(persistance);

                PORTD = ledDigWithDec[DIG2];
                PORTB =~ (1<<3);
                _delay_ms(persistance);

                PORTD = ledDigits[DIG1];
                PORTB =~ (1<<4);
                _delay_ms(persistance);

                PORTB = 0xFF;
            }
        }

    }
    return 0;

}
 // initializing the ADC ports
void adc_init(void){
    DDRC = 0x00;
    ADMUX = 0xC0;
    ADCSRA = 0x87;
}
// getting the value from the TMP36 sensor and converting to Celcius and sending it back
unsigned int get_adc(){
    ADCSRA |= (1<<ADSC);
    while((ADCSRA & (1 << ADIF)) == 0);
    return ADCL | (ADCH << 8);
}
