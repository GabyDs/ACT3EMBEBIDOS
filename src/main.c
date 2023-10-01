#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000L // Frecuencia del atmega328p

#define ALARMA PORTD7

#define tiempo_rebote 6 // ms

#define OCR0A_display 199
#define OCR0B_alarma 31250

#define Q1 PORTC0
#define Q2 PORTC1
#define Q3 PORTC2

volatile uint8_t habilitado = -1; // 1 -> habilitado; -1 -> deshabilitado
volatile uint16_t umbral = 50;    // umbral -> 50-400
volatile uint16_t contador = 125;
uint16_t cuenta = 48;
volatile uint8_t contador_timer1 = 0;

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

ISR(TIMER1_COMPA_vect)
{
    contador_timer1++;
    if (contador_timer1 >= 5)
    {
        PORTD &= ~(1 << ALARMA);               // Apaga alarma
        TCCR1B &= ~(1 << CS12) & ~(1 << CS10); // Detiene conteo
        contador_timer1 = 0;
    }
}

int main(void)
{
    // Timer0
    TCCR0A = (1 << WGM01);              // CTC mode
    OCR0A = OCR0A_display;              // TOP -> aprox 15ms
    TCCR0B = (1 << CS02) | (1 << CS00); // clk_I/O /1024 (from prescaler)
    TIMSK0 = (1 << OCIE0A);             // habilita la interrupcion por comparacion

    // Timer 1
    TCCR1B = (1 << WGM12); // Modo CTC
    OCR1A = OCR0B_alarma;
    TIMSK1 = (1 << OCIE1A); // Interrupción por igualación TCNT1 == OCR1A

    sei();

    // Entradas
    // DDRD = (1 << ALARMA);
    PORTD = (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4);

    // Salidas
    DDRB = 0xFF; // display

    DDRC = 0xFF;
    PORTC = 0xFF;

    while (1)
    {

        estadoPD2 = (PIND & (1 << PIND2));
        if (estadoPD2 != ultimo_estadoPD2) // es decir que cambio el estado
        {
            if (!estadoPD2)
            {
                _delay_ms(tiempo_rebote);
                if (!(PIND & (1 << PIND2)))
                {
                    umbral++; // Incrementa el umbral
                    contador = 0;

                    if (umbral > 400)
                        umbral = 50;
                }
            }
        }
        ultimo_estadoPD2 = estadoPD2;

        estadoPD3 = (PIND & (1 << PIND3));
        if (estadoPD3 != ultimo_estadoPD3)
        {
            if (!estadoPD3)
            {
                _delay_ms(tiempo_rebote);
                if (!(PIND & (1 << PIND3)))
                    habilitado *= -1; // Habilitar el conteo
            }
        }
        ultimo_estadoPD3 = estadoPD3;

        estadoPD4 = (PIND & (1 << PIND4));
        if (estadoPD4 != ultimo_estadoPD4)
        {
            if (!estado_display)
            {
                _delay_ms(tiempo_rebote);
                if (!(PIND & (1 << PIND4)))
                {
                    if (habilitado) // Incrementar el contador si esta habilitado
                    {
                        contador++;
                        cuenta++;
                    }
                    if (contador > 999)
                        contador = 0;
                }
            }
        }
        ultimo_estadoPD4 = estadoPD4;

        if ((cuenta >= umbral) && !(PINC & (1 << ALARMA)))
        {
            cuenta = 0;
            // si contador por encima de umbral y alarma no está encendida
            // encender alarma por 10 segundos
            PORTD |= (1 << ALARMA);
            TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024. Inicia el conteo
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