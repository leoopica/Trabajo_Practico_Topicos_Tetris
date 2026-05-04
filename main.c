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
    double duracion_actual = duracion_caida;
    tGBT_Temporizador *timer_caida = gbt_temporizador_crear (duracion_actual); // Temporizador para limitar la velocidad del juego
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
    tGBT_Temporizador *timer_fijacion = NULL;

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
                    int multiplicador = (nivel / 2) + 1;
                    if (multiplicador > 5) multiplicador = 5;

                    int bonus_velocidad = (int)((1.0 - duracion_caida) * 10);
                    if (bonus_velocidad < 0) bonus_velocidad = 0;

                    puntaje += multiplicador + bonus_velocidad;
                }

                if (COLISION (actual.fila + 1, actual.columna, actual.forma)) // Verifica si la pieza está en el suelo
                {
                    if (timer_fijacion == NULL)
                    {
                        timer_fijacion = gbt_temporizador_crear (duracion_caida * 2.0); // 50% de velocidad = 200% de duración
                    }

                    if (gbt_temporizador_consumir (timer_fijacion))
                    {
                        FIJARPIEZA (); // Fija la pieza
                        LIMPIARLINEAS (); // Si se completó una fila, la limpia
                        NUEVAPIEZA (); // Obtiene nueva pieza
                        gbt_temporizador_destruir (timer_fijacion);
                        timer_fijacion = NULL;

                        // Si la duración cambió en NUEVAPIEZA, recrear el temporizador de caída
                        if (duracion_actual != duracion_caida)
                        {
                            gbt_temporizador_destruir (timer_caida);
                            timer_caida = gbt_temporizador_crear (duracion_caida);
                            duracion_actual = duracion_caida;
                        }
                    }
                }
                else
                {
                    if (timer_fijacion != NULL)
                    {
                        gbt_temporizador_destruir (timer_fijacion);
                        timer_fijacion = NULL;
                    }

                    if (gbt_temporizador_consumir (timer_caida))
                    {
                        actual.fila ++; // Baja la pieza una fila si no hubo colisión
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
    if (timer_fijacion) gbt_temporizador_destruir(timer_fijacion);
    gbt_cerrar (); // Cierra la biblioteca GBT
    return 0;
}
