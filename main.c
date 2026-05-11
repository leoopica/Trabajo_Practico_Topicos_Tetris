/*
DATOS DEL GRUPO
(Apellido, Nombre, DNI, Usuario de GitHub, Entrega)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "graficos.h"
#include "inicio.h"
#include "funcionalidades.h"
#include "configuracion.h"
#include "menu.h"
#include "estadisticas.h"

extern char nombreJugador [21];
extern int animacion_borrado_activa;
static int animacion_estaba_activa = 0;
int pieza_fijada_sin_nueva = 0;

// Parsea los argumentos de línea de comandos y los aplica a la config
static void PARSEAR_ARGV(int argc, char *argv[], sConfig *c)
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-cga") == 0)
            c->resolucion = RESOLUCION_CGA;
        else if (strcmp(argv[i], "-vga") == 0)
            c->resolucion = RESOLUCION_VGA;
        else if (strcmp(argv[i], "-escala") == 0 && i + 1 < argc)
        {
            int escala = atoi(argv[i + 1]);
            if (escala >= 1 && escala <= 4)
                c->escala = escala;
            i++; // Saltar el siguiente argumento (el valor de escala)
        }
    }
}

int main (int argc, char *argv[])
{
    char nombreVentana [128];

    // 1. Iniciar GBT
    if (gbt_iniciar() != 0)
    {
        fprintf(stderr, "Error al iniciar GBT: %s\n", gbt_obtener_log());
        return -1;
    }

    // 2. Cargar configuración (o usar defaults si no existe config.dat)
    if (CONFIG_CARGAR(&config_actual) != 0)
        CONFIG_DEFAULTS(&config_actual);

    // 3. Aplicar argumentos de línea de comandos (sobreescriben config)
    PARSEAR_ARGV(argc, argv, &config_actual);

    // 4. Crear ventana con la resolución y escala de la config
    sprintf(nombreVentana, "Tetris %dx%d", CONFIG_ANCHO(), CONFIG_ALTO());
    if (gbt_crear_ventana(nombreVentana, CONFIG_ANCHO(), CONFIG_ALTO(), config_actual.escala) != 0)
    {
        fprintf(stderr, "Error al crear ventana: %s\n", gbt_obtener_log());
        return -1;
    }

    // 5. Aplicar paleta y velocidad inicial de la config
    CONFIG_APLICAR(&config_actual);

    srand(time(0));

    // 6. Bucle del menú principal
    while (1)
    {
        eMenuResultado resultado = MENU_PRINCIPAL();
        if (resultado == MENU_RESULTADO_SALIR)
            break;

        // -- JUGAR --
        // Pantalla de ingreso de nombre
        DIBUJARINICIO(nombreJugador);

        // Reiniciar estado del juego
        REINICIARJUEGO();
        pieza_fijada_sin_nueva = 0;
        animacion_estaba_activa = 0;

        // Aplicar velocidad inicial de la config al juego
        duracion_caida = velocidades_disponibles[config_actual.velocidad_inicial];

        NUEVAPIEZA();
        double duracion_actual = duracion_caida;

        tGBT_Temporizador *timer_caida = gbt_temporizador_crear(duracion_actual);
        if (!timer_caida)
        {
            fprintf(stderr, "Error al crear temporizador: %s\n", gbt_obtener_log());
            return -1;
        }
        tGBT_Temporizador *timer_mov = gbt_temporizador_crear(0.1);
        if (!timer_mov)
        {
            fprintf(stderr, "Error al crear temporizador: %s\n", gbt_obtener_log());
            return -1;
        }
        tGBT_Temporizador *timer_fijacion = NULL;

        uint8_t corriendo = 1;
        int salir_juego = 0; // 1 = Q presionado, salir del programa completo

        while (corriendo)
        {
            gbt_procesar_entrada();

            if (gbt_tecla_sostenida(GBTK_q))
            {
                salir_juego = 1;
                corriendo = 0; // Salir de la partida
            }
            else
            {
                if (estado_juego == ESTADO_RUNNING && !animacion_borrado_activa)
                {
                    if (gbt_tecla_presionada(GBTK_p))
                        estado_juego = ESTADO_PAUSED;

                    if (gbt_tecla_presionada(GBTK_d))
                        ROTARHORARIO();
                    if (gbt_tecla_presionada(GBTK_a))
                        ROTARANTIHORARIO();

                    if (gbt_temporizador_consumir(timer_mov))
                    {
                        if (gbt_tecla_sostenida(GBTK_IZQUIERDA) && !COLISION(actual.fila, actual.columna - 1, actual.forma))
                            actual.columna--;
                        if (gbt_tecla_sostenida(GBTK_DERECHA) && !COLISION(actual.fila, actual.columna + 1, actual.forma))
                            actual.columna++;
                        if (gbt_tecla_sostenida(GBTK_ABAJO) && !COLISION(actual.fila + 1, actual.columna, actual.forma))
                        {
                            actual.fila++;
                            int mult = (nivel / 2) + 1;
                            if (mult > 5) mult = 5;
                            int bonus = (int)((1.0 - duracion_caida) * 10);
                            if (bonus < 0) bonus = 0;
                            puntaje += mult + bonus;
                        }

                        if (COLISION(actual.fila + 1, actual.columna, actual.forma))
                        {
                            if (timer_fijacion == NULL)
                                timer_fijacion = gbt_temporizador_crear(duracion_caida * 0.5);

                            if (gbt_temporizador_consumir(timer_fijacion))
                            {
                                FIJARPIEZA();
                                LIMPIARLINEAS();
                                pieza_fijada_sin_nueva = 1;

                                gbt_temporizador_destruir(timer_fijacion);
                                timer_fijacion = NULL;

                                if (duracion_actual != duracion_caida)
                                {
                                    gbt_temporizador_destruir(timer_caida);
                                    timer_caida = gbt_temporizador_crear(duracion_caida);
                                    duracion_actual = duracion_caida;
                                }
                            }
                        }
                        else
                        {
                            if (timer_fijacion != NULL)
                            {
                                gbt_temporizador_destruir(timer_fijacion);
                                timer_fijacion = NULL;
                            }
                            if (gbt_temporizador_consumir(timer_caida))
                                actual.fila++;
                        }
                    }
                }
                else if (estado_juego == ESTADO_PAUSED)
                {
                    if (gbt_tecla_presionada(GBTK_p))
                        estado_juego = ESTADO_RUNNING;
                }
                else if (estado_juego == ESTADO_GAMEOVER)
                {
                    // Guardar estadística al terminar la partida
                    static int ya_guardo = 0;
                    if (!ya_guardo)
                    {
                        STATS_GUARDAR(nombreJugador, puntaje);
                        ya_guardo = 1;
                    }

                    if (gbt_tecla_presionada(GBTK_r))
                    {
                        ya_guardo = 0;
                        REINICIARJUEGO();
                        duracion_caida = velocidades_disponibles[config_actual.velocidad_inicial];
                        gbt_temporizador_destruir(timer_caida);
                        timer_caida = gbt_temporizador_crear(duracion_caida);
                        duracion_actual = duracion_caida;
                        pieza_fijada_sin_nueva = 0;
                        if (timer_fijacion)
                        {
                            gbt_temporizador_destruir(timer_fijacion);
                            timer_fijacion = NULL;
                        }
                    }
                    if (gbt_tecla_presionada(GBTK_ENTER) || gbt_tecla_presionada(GBTK_ESCAPE))
                    {
                        ya_guardo = 0;
                        corriendo = 0; // Volver al menú
                    }
                }

                ACTUALIZAR_ANIMACION_BORRADO();
                if (pieza_fijada_sin_nueva && !animacion_borrado_activa)
                {
                    NUEVAPIEZA();
                    pieza_fijada_sin_nueva = 0;
                    animacion_estaba_activa = 0;
                }
                else
                {
                    animacion_estaba_activa = animacion_borrado_activa;
                }

                DIBUJAR();
                if (estado_juego == ESTADO_PAUSED)   DIBUJARPAUSA();
                if (estado_juego == ESTADO_GAMEOVER) DIBUJARGAMEOVER();

                gbt_volcar_backbuffer();
                gbt_esperar(16);
            }
        }

        // Limpiar temporizadores al salir de la partida
        gbt_temporizador_destruir(timer_caida);
        gbt_temporizador_destruir(timer_mov);
        if (timer_fijacion) gbt_temporizador_destruir(timer_fijacion);

        // Si Q fue presionado durante la partida, salir del programa
        if (salir_juego) break;
    }

    gbt_destruir_ventana();
    gbt_cerrar();
    return 0;
}