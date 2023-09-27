/*********CONSIGNA*********

Simulador de maquina de conteo de paquetes.

P1: Cambia el umbral y resetea el contador (interrupcion)
P2: Habilita o dehabilita el conteo (interrupcion)
P3: Simulador del contador (solo si P2 habilita el conteo), (encuesta)
    Cada que se presione P3 el contador aumenta en 1

El umbral puede ser entre 50 y 400, por lo tanto se necesita un contador de 16 bits.
Cuando el conteo llega al umbral se prende la alarma.

Display: En estado de configuracion debe mostrar el umbral. Durante el conteo muestra la cantidad de packs
         Si supera el umbral sigue contando hasta 999

Timer: Cuando el timer desborda se multiplexa al siguiente display, entonces se muestra
D1: Centena; D2, Decena; D3, Unidad.

*/

/*********PINES*********

Entradas:
PD2 -> SELECCIONAR UMBRAL
PD3 -> HABILITAR / DESHABILITAR CONTEO
PD4 -> CONTADOR

Salidas:
PC0,1,2 -> Q1,2,3
PB0 -> ALARMA
PB2,3,4,5 -> DECODIFICADOR

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define F_CPU 16000000L

// Salidas
#define Q1 PORTC0
#define Q2 PORTC1
#define Q3 PORTC2
#define B0 PORTB5
#define B1 PORTB4
#define B2 PORTB3
#define B3 PORTB2
#define ALARMA PORTB0

#define tiempo_rebote 6 // ms

enum UMBRAL
{
    U1 = 50,
    U2 = 400
};

int habilitado = -1; // 1 -> habilitado; -1 -> deshabilitado
int umbral = U1;
int contador = 0;

void cambiar_umbral();
void bcd_al_display();

int main(void)
{

    // Entradas
    PORTD = (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4);

    // Salidas
    DDRB = 0xFF;
    PORTB = 0x00;

    DDRC = 0xFF;
    PORTC = 0x00;

    while (1)
    {

        // Habilitar o deshabilitar el conteo
        if (!(PIND & (1 << PIND3)))
        {
            _delay_ms(tiempo_rebote);
            if (!(PIND & (1 << PIND3)))
            {
                habilitado *= -1;
            }
        }

        // Cambiar el umbral
        if (!(PIND & (1 << PIND2)))
        {
            _delay_ms(tiempo_rebote);
            if (!(PIND & (1 << PIND2)))
            {
                cambiar_umbral();
            }
        }

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

void bcd_al_display()
{
}