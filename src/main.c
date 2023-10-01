#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000L // Frecuencia del atmega328p

#define ALARMA PORTB0

#define tiempo_rebote 10 // ms

#define OCR0A_display 199

enum DISPLAY
{
    Q1 = PORTC0,
    Q2 = PORTC1,
    Q3 = PORT2
};

volatile int habilitado = -1; // 1 -> habilitado; -1 -> deshabilitado
volatile int umbral = 50;     // umbral -> 50-400
volatile int contador = 45;

uint8_t estado_display = Q1;
uint8_t unidad, decena, centena = 0;
uint8_t estadoPD2;
uint8_t ultimo_estadoPD2 = 1;
uint8_t estadoPD3;
uint8_t ultimo_estadoPD3 = 1;
uint8_t estadoPD4;
uint8_t ultimo_estadoPD4 = 1;

void ciclar_display(int);

ISR(TIMER0_COMPA_vect)
{
    if (habilitado == 1)
        ciclar_display(contador);
    else
        ciclar_display(umbral);
}

int main(void)
{
    // Timer
    TCCR0A = (1 << WGM01);              // CTC mode
    OCR0A = OCR0A_display;              // TOP -> aprox 15ms
    TCCR0B = (1 << CS02) | (1 << CS00); // clk_I/O /1024 (from prescaler)
    TIMSK0 = (1 << OCIE0A);             // habilita la interrupcion por comparacion

    sei();

    // Entradas
    PORTD = (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4);

    // Salidas
    DDRB = 0xFF; // display

    DDRC = 0xFF;
    PORTC = 0xFF;

    while (1)
    {
        estadoPD4 = (PIND & (1 << PIND4));
        if (estadoPD4 != ultimo_estadoPD4)
        {
            if (!estado_display)
            {
                _delay_ms(10 * tiempo_rebote);
                if (!(PIND & (1 << PIND4)))
                {
                    if (habilitado == 1) // Incrementar el contador si esta habilitado
                        contador += 1;
                }
            }
        }

        estadoPD3 = (PIND & (1 << PIND3));
        if (estadoPD3 != ultimo_estadoPD3)
        {
            if (!estadoPD3)
            {
                _delay_ms(10 * tiempo_rebote);
                if (!(PIND & (1 << PIND3)))
                    habilitado *= -1; // Habilitar el conteo
            }
        }

        estadoPD2 = (PIND & (1 << PIND2));
        if (estadoPD2 != ultimo_estadoPD2) // es decir que cambio el estado
        {
            if (!estadoPD2)
            {
                _delay_ms(10 * tiempo_rebote);
                if (!(PIND & (1 << PIND2)))
                {
                    umbral += 1; // Incrementa el umbral
                    contador = 0;

                    if (umbral > 400)
                        umbral = 50;
                }
            }
        }
    }
    return 0;
}

void ciclar_display(int num)
{
    PORTC = 0xFF;
    switch (estado_display)
    {
    case Q1:
        // Habilitamos el display
        PORTC &= ~(1 << estado_display);
        PORTB = num % 10;
        estado_display = Q2;
        break;
    case Q2:
        // Habilitamos el display
        PORTC &= ~(1 << estado_display);
        PORTB = (num % 100) / 10;
        estado_display = Q3;
        break;
    case Q3:
        // Habilitamos el display
        PORTC &= ~(1 << estado_display);
        PORTB = num / 100;
        estado_display = Q1;
        break;
    }
}