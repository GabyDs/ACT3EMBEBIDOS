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

#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000L // Frecuencia del atmega328p

#define ALARMA PORTB0

#define tiempo_rebote 6 // ms

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

int habilitado = -1; // 1 -> habilitado; -1 -> deshabilitado
int umbral = U1;
int contador = 125;
int estado_display = Q1;
int unidad, decena, centena = 0;

void cambiar_umbral();
void encender_display(int);
void visualizar_display(int);
void ciclar_display();

int main(void)
{

    // Entradas
    PORTD = (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4);

    // Salidas
    DDRB = 0xFF;
    PORTB = 0x00;

    DDRC = 0xFF;
    PORTC = 0xFF;

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
                contador = 0;
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
                {
                    contador += 1;
                    _delay_ms(100);
                }
            }
        }
        visualizar_display(contador);
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

void encender_display(int num)
{
    // Habilitamos el display
    PORTC &= ~(1 << estado_display);

    // Convertir a binario y mostrar en los pines B0, B1, B2, B3
    for (int i = 0; i < 4; i++)
    {
        if (num & (1 << i))
        {
            PORTB |= (1 << (B0 - i)); // Encender el pin correspondiente
        }
        else
        {
            PORTB &= ~(1 << (B0 - i)); // Apagar el pin correspondiente
        }
    }
}

void visualizar_display(int cont)
{
    unidad = cont % 10;
    decena = (cont % 100) / 10;
    centena = cont / 100;

    encender_display(unidad);
    _delay_ms(1000);
    ciclar_display();

    encender_display(decena);
    _delay_ms(1000);
    ciclar_display();

    encender_display(centena);
    _delay_ms(1000);
    ciclar_display();
}

void ciclar_display()
{
    PORTC = 0xFF;
    switch (estado_display)
    {
    case Q1:
        estado_display = Q2;
        break;
    case Q2:
        estado_display = Q3;
        break;
    case Q3:
        estado_display = Q1;
        break;
    }
}