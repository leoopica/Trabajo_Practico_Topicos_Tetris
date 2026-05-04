/*
PRUEBA DE TETRIS BÁSICO
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "graficos.h"

extern char nombreJugador [21];

int main ()
{
    char nombreVentana [128]; // Vector para el nombre de la ventana
    
    if (gbt_iniciar() != 0) // Iniciar la biblioteca GBT
    {
        fprintf (stderr, "Error al iniciar GBT: %s\n", gbt_obtener_log()); // Si falla en el inicio, lo indica
        return -1;
    }

    sprintf (nombreVentana, "Tetris %dx%d", anchoVentana, altoVentana); // Pone el nombre Tetris en la ventana con las medidas de la misma

    if (gbt_crear_ventana (nombreVentana, anchoVentana, altoVentana, escalaVentana) != 0) // Crea la ventana
    {
        fprintf (stderr, "Error al iniciar el modulo de graficos de GBT: %s\n", gbt_obtener_log()); // Si falla en la creación, lo indica
        return -1;
    }

    if (gbt_aplicar_paleta (paletaCGA, cantColores, GBT_FORMATO_888) != 0) // Aplica la paleta de colores definida
    {
        fprintf (stderr, "Error al aplicar la nueva paleta de colores: %s\n", gbt_obtener_log()); // Si falla la aplicación de la paleta, lo indica
        return -1;
    }
    DIBUJARINICIO (nombreJugador);
    srand (time (0)); // Para randomizar
    NUEVAPIEZA (); // Obtiene pieza nueva
    tGBT_Temporizador *timer_caida = gbt_temporizador_crear (0.3); // Temporizador para limitar la velocidad del juego
    if (!timer_caida)
    {
        fprintf (stderr, "Error al crear temporizador: %s\n", gbt_obtener_log()); // Si falla en la creación, lo indica
        return -1;
    }
    tGBT_Temporizador *timer_mov = gbt_temporizador_crear(0.1); // Temporizador para limitar la velocidad de movimiento
    if (!timer_mov)
    {
        fprintf (stderr, "Error al crear temporizador: %s\n", gbt_obtener_log()); // Si falla en la creación, lo indica
        return -1;
    }

    uint8_t corriendo = 1; // Define el estado corriendo

    while (corriendo) // Mientras esté funcionando
    {
        gbt_procesar_entrada (); // Procesa la entrada de teclas
        if (gbt_tecla_sostenida (GBTK_ESCAPE)) // Verifica input de la tecla Escape
        {
            corriendo = 0;
            printf ("Saliendo del juego\n");
        }
        else 
        {
            if (gbt_tecla_presionada (GBTK_d)) // Input tecla D para girar sentido horario
            {
                ROTARHORARIO ();
            }
            if (gbt_tecla_presionada (GBTK_a)) // Input tecla A para girar sentido antihorario
            {
                ROTARANTIHORARIO ();
            }
            if (gbt_temporizador_consumir(timer_mov))
            {
                if (gbt_tecla_sostenida (GBTK_IZQUIERDA) && !COLISION (actual.fila, actual.columna - 1, actual.forma)) // Verifica input de flecha izquierda y colisión
                {
                    actual.columna --;
                }
                if (gbt_tecla_sostenida (GBTK_DERECHA) && !COLISION (actual.fila, actual.columna + 1, actual.forma)) // Verifica input de flecha derecha y colisión
                {
                    actual.columna ++;
                }
                if (gbt_tecla_sostenida (GBTK_ABAJO) && !COLISION (actual.fila + 1, actual.columna, actual.forma)) // Verifica input de flecha abajo y colisión
                {
                    actual.fila ++;
                }

                if (gbt_temporizador_consumir (timer_caida))
                {
                    if (!COLISION (actual.fila + 1, actual.columna, actual.forma)) // Verifica colisión para caída automática
                    {
                        actual.fila ++; // Baja la pieza una fila si no hubo colisión
                    }
                    else
                    {
                        FIJARPIEZA (); // Si hubo colisión, fija la pieza
                        LIMPIARLINEAS (); // Si se completó una fila, la limpia
                        NUEVAPIEZA (); // Obtiene nueva pieza
                    }
                }
            }
            DIBUJAR (); // Dibuja los gráficos
            gbt_volcar_backbuffer (); // Vuelca los gráficos en la ventana 
            gbt_esperar (16); // Espera 16 ms para evitar que el juego se vea demasiado rápido
        }
    }
    gbt_destruir_ventana (); // Destruye la ventana creada
    gbt_temporizador_destruir (timer_caida); // Destruye el temporizador de la caída
    gbt_temporizador_destruir (timer_mov); // Destruye el temporizador del movimiento de las piezas
    gbt_cerrar (); // Cierra la biblioteca GBT
    return 0;
}
