/*********CONSIGNA*********

Simulador de maquina de conteo de paquetes.

P1: Cambia el umbral y resetea el contador (interrupcion)
P2: Habilita o dehabilita el conteo (interrupcion)
P3: Simulador del contador (solo si P2 habilita el conteo), (encuesta)
    Cada que se presione P3 el contador aumenta en 1

El umbral puede ser entre 50 y 400, por lo tanto se necesita un contador de 16 bits.

Display: En estado de configuracion debe mostrar el umbral. Durante el conteo muestra la cantidad de packs
         Si supera el umbral sigue contando hasta 999

Timer: Cuando el timer supera el umbral se debe prender una alarma (led) por 10s, se debe configurar para
       que detecte flanco descendente

*/

int main(void)
{

    while (1)
    {
        break;
    }

    return 0;
}