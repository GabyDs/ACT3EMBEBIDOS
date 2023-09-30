#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000L // Frecuencia del atmega328p

#define ALARMA PORTB0

#define tiempo_rebote 6 // ms

#define VPC0 100

#define OCR0A_display 199

enum UMBRAL
{
    U1 = 50,
    U2 = 400
};

enum DISPLAY
{
    Q1 = PORTC0,
    Q2 = PORTC1,
    Q3 = PORT2
};

enum BCD
{
    B0 = PORTB5,
    B1 = PORTB4,
    B2 = PORTB3,
    B3 = PORTB2
};

volatile int habilitado = -1; // 1 -> habilitado; -1 -> deshabilitado
volatile int umbral = U1;
volatile int contador = 125;

int estado_display = Q1;
int unidad, decena, centena = 0;

void cambiar_umbral();
void ciclar_display(int);

ISR(INT0_vect)
{
    _delay_ms(tiempo_rebote);
    if (!(PIND & (1 << PIND2)))
    {
        contador = 0;
        cambiar_umbral();
    }
}

ISR(INT1_vect)
{
    _delay_ms(tiempo_rebote);
    if (!(PIND & (1 << PIND3)))
    {
        habilitado *= -1;
    }
}

ISR(TIMER0_COMPA_vect)
{
    if (habilitado == 1)
        ciclar_display(contador);
    else
        ciclar_display(umbral);
}

int main(void)
{
    // Interrupciones

    // Externas
    EICRA |= (1 << ISC01) | (1 << ISC11);
    EIMSK |= (1 << INT0) | (1 << INT1);

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
        // Incrementar el contador si esta habilitado
        if (!(PIND & (1 << PIND4)))
        {
            _delay_ms(tiempo_rebote);
            if (!(PIND & (1 << PIND4)))
            {
                if (habilitado == 1)
                    contador += 1;
            }
        }
    }

    return 0;
}

void cambiar_umbral()
{
    switch (umbral)
    {
    case U1:
        umbral = U2;
        contador = 0;
        break;
    case U2:
        umbral = U1;
        contador = 0;
        break;
    }
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