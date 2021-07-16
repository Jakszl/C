#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

//zmnienne globalne
volatile int tempC;
volatile int dec,ones;

int readADC(char channel)
{
	ADMUX = (1 << REFS1)|(1<<ADLAR)|(channel << MUX0);		// VREF=2,56V channel 0 jest na PC0
	_delay_us(100);											// czas na uregulowanie multiplexera
	ADCSRA |= (1<<ADSC);									// Uruchomienie konwersji
	while (ADCSRA & (1<<ADSC));								// Oczekiwanie na koniec konwersji
	return ADCH;											// 8 bitowy wynik ponieważ używamy ADLAR
}

void led_display(int x)
{
	switch (x)
	{
		case 0: PORTD=0XC0;break;
		case 1: PORTD=0XF9;break;
		case 2: PORTD=0XA4;break;
		case 3: PORTD=0XB0;break;
		case 4: PORTD=0X99;break;
		case 5: PORTD=0X92;break;
		case 6: PORTD=0X82;break;
		case 7: PORTD=0XF8;break;
		case 8: PORTD=0X80;break;
		case 9: PORTD=0X90;break;
		default: break;
	}
}


ISR(TIMER0_OVF_vect) {
	// wyświetlanie wyniku
	dec=(tempC/10);    //dzielenie w celu uzyskania liczby dziesiątek
	ones=(tempC%10);	//reszta z dzielenia przez 10 w celu uzyskania liczby jedności
	led_display(dec);
	PORTB|=(1<<PB1);
	PORTB&=~(1<<PB0);
	_delay_us(0);
	PORTB&=~(1<<PB1);
	PORTB&=~(1<<PB0);
	led_display(ones);
	PORTB&=~(1<<PB1);
	PORTB|=(1<<PB0);
	_delay_us(0);
	PORTB&=~(1<<PB1);
	PORTB&=~(1<<PB0);
}

void syst_init()
{
	cli();					//Dezaktywacja przerwań na czas inicializacji
	DDRB =0xFF;				//Ustawianie wyjść
	DDRD =0xFF;
	
	ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<< ADPS2)|(1<<ADIF);        // Włączenie ADC, div128
	
	TIMSK0=(1<<TOIE0) ;		// uruchomienie Timer0 overflow interrupt
	TCNT0=0x00;				// ustawienie licznika timer0 na wartość 0
	TCCR0B = (1<<CS11);		// start timer0 z 8 prescaler
	sei();					// włączenie przerwań
}

int main(void)
{
	syst_init();
	while (1) {
		tempC = readADC(0);			// 10mV na C.  
		tempC = tempC-38;			// Wyregulowanie wyniku
	}
	return 0;
}
