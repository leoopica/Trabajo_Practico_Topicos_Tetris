/*
Apellido y nombre: Abrate, Micaela Abril
DNI: 42319879
Usuario: MicaAbrate
Entrega: Sí

Apellido y nombre: Engler, Matías Ezequiel
DNI: 43316397
Usuario: me-engler
Entrega: Sí

Apellido y nombre: Pica, Leonardo Ariel
DNI: 43083209
Usuario: leoopica
Entrega: Sí
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include "graficos.h"
#include "input.h"
#include "juego.h"
#include "gbt.h"

#define ancho_ventana 128
#define alto_ventana 128
#define ancho_ventana_CGA 320
#define alto_ventana_CGA 200
#define ancho_ventana_VGA 640
#define alto_ventana_VGA 480
#define escala_ventana 5
#define cant_colores 16

tGBT_ColorRGB paletaCGA[cant_colores] = {

    /// 0-15: Colores CGA (16 colores)
    {0x00, 0x00, 0x00}, // 0:   Negro
    {0x00, 0x00, 0xAA}, // 1:   Azul
    {0x00, 0xAA, 0x00}, // 2:   Verde
    {0x00, 0xAA, 0xAA}, // 3:   Cian
    {0xAA, 0x00, 0x00}, // 4:   Rojo
    {0xAA, 0x00, 0xAA}, // 5:   Magenta
    {0xAA, 0x55, 0x00}, // 6:   Marron
    {0xAA, 0xAA, 0xAA}, // 7:   Gris claro
    {0x55, 0x55, 0x55}, // 8:   Gris oscuro
    {0x55, 0x55, 0xFF}, // 9:   Azul brillante
    {0x55, 0xFF, 0x55}, // 10:  Verde brillante
    {0x55, 0xFF, 0xFF}, // 11:  Cian brillante
    {0xFF, 0x55, 0x55}, // 12:  Rojo brillante
    {0xFF, 0x55, 0xFF}, // 13:  Magenta brillante
    {0xFF, 0xFF, 0x55}, // 14:  Amarillo
    {0xFF, 0xFF, 0xFF}  // 15:  Usado como transparente por GBT
};

int main ()

{
    char nombre_ventana [128], nombre_jugador [4];
    uint8_t corriendo = 0;

    gbt_iniciar (); /*Inicia subsistemas de gbt.h*/

    if (gbt_iniciar() != 0) /*Verifica falla en inicialización de gbt.h*/
    {
        fprintf (stderr, "Error al iniciar GBT: %s\n", gbt_obtener_log ()); /*stderr es salida de errores*/
        return -1;
    }

    sprintf(nombre_ventana, "Ventana %dx%d", ancho_ventana, alto_ventana); /*Arma el texto "Ventana 128x128" y lo guarda en nombre_ventana*/

    gbt_crear_ventana(nombre_ventana, ancho_ventana, alto_ventana, escala_ventana);
    if (gbt_crear_ventana(nombre_ventana, ancho_ventana, alto_ventana, escala_ventana) != 0) /*Verifica falla en creación de ventana*/
    {
        fprintf(stderr, "Error al iniciar el modulo de graficos de GBT: %s\n", gbt_obtener_log());
        return -1;
    }

    gbt_aplicar_paleta(paletaCGA, cant_colores, GBT_FORMATO_888); /*Aplica la paleta de colores definida*/
    if (gbt_aplicar_paleta(paletaCGA, cant_colores, GBT_FORMATO_888) != 0) /*Verifica falla en aplicación de paleta de colores*/
    {
        fprintf(stderr, "Error al aplicar la nueva paleta de colores: %s\n", gbt_obtener_log());
        return -1;
    }

    srand (time(0)); /*Genera números aleatorios*/
    corriendo = 1; /*Estado del programa: corriendo*/

    while (corriendo = 1)
    {
        gbt_procesar_entrada (); /*Inicializa el procesamiento de entradas por teclado*/
        printf ("Ingrese nombre (3 caracteres):\n");
        fgets (nombre_jugador, 4, stdin);
        fflush (stdin);
        if (nombre_jugador [strlen (nombre_jugador) - 1] == '\n')
        {
            nombre_jugador [strlen (nombre_jugador) - 1] = '\0';
        }
    }

    return 0;
}
