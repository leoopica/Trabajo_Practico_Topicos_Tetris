/*
DATOS DEL GRUPO

Apellido: Abrate, Micaela Abril
DNI: 42319879
Usuario: MicaAbrate
Entrega: No

Apellido: Engler, Matías Ezequiel
DNI: 43316397
Usuario: me-engler
Entrega: Sí

Apellido: Pica, Leonardo Ariel
DNI: 43083209
Usuario: leoopica
Entrega: Sí
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "graficos.h"
#include "inicio.h"
#include "funcionalidades.h"
#include "configuracion.h"
#include "menu.h"
#include "estadisticas.h"
#include "archivos.h"

extern char nombreJugador [21]; // Nombre ingresado por el jugador
extern int animacion_borrado_activa; // 1 = animación de limpieza de líneas activa

int pieza_fijada_sin_nueva = 0; // 1 = pieza fijada pero sin generar la siguiente

// Parsea argumentos CLI: -cga, -vga, -escala N
static void PARSEAR_ARGV (int argc, char *argv [], sConfig *c)
{
    int i, escala;

    for (i = 1; i < argc; i ++)
    {
        if (strcmp (argv[i], "-cga") == 0)
        {
            c->resolucion = RESOLUCION_CGA;
        }
        else if (strcmp (argv[i], "-vga") == 0)
        {
            c->resolucion = RESOLUCION_VGA;
        }
        else if (strcmp (argv [i], "-escala") == 0 && i + 1 < argc)
        {
            escala = atoi (argv[i + 1]);
            if (escala >= 1 && escala <= 4)
            {
                c->escala = escala;
            }
            i ++;
        }
    }
}

// Loop principal de una partida. cargando=1 restaura partida guardada.
// Retorna 0 para volver al menú, 1 para salir del programa.
static int JUGAR (int cargando)
{
    double duracion_actual;
    tGBT_Temporizador *timer_caida;
    tGBT_Temporizador *timer_mov;
    tGBT_Temporizador *timer_fijacion;
    uint8_t corriendo;
    int salir_juego, mult, bonus;
    static int ya_guardo = 0;
    ePausaResultado resultado_pausa;

    corriendo = 1;
    salir_juego = 0;

    if (!cargando || PARTIDA_CARGAR () != 0)
    {
        DIBUJARINICIO (nombreJugador);
        if (nombreJugador [0] == '\0')
        {
            return 0;
        }
        if (config_actual.modo_juego == MODO_DX)
        {
            columnasTablero = config_actual.ancho_tablero;
        }
        else
        {
            columnasTablero = 10;
        }
        if (config_actual.modo_juego == MODO_DX)
        {
            piezas_en_uso = MAX_PIEZAS;
        }
        else
        {
            piezas_en_uso = 7;
        }
        REINICIARJUEGO ();
    }

    pieza_fijada_sin_nueva = 0;
    duracion_caida = velocidades_disponibles[config_actual.velocidad_inicial];

    if (!cargando)
    {
        NUEVAPIEZA ();
    }

    duracion_actual = duracion_caida;

    timer_caida = gbt_temporizador_crear (duracion_actual);
    if (!timer_caida)
    {
        fprintf (stderr, "Error temporizador\n");
        return 1;
    }

    timer_mov = gbt_temporizador_crear (0.1);
    if (!timer_mov)
    {
        fprintf (stderr, "Error temporizador\n");
        return 1;
    }

    timer_fijacion = NULL;

    while (corriendo)
    {
        gbt_procesar_entrada ();

        if (gbt_tecla_sostenida (GBTK_q))
        {
            salir_juego = 1;
            corriendo = 0;
        }
        else
        {
            if (estado_juego == ESTADO_RUNNING && !animacion_borrado_activa)
            {
                if (gbt_tecla_presionada (GBTK_p))
                {
                    resultado_pausa = MENU_PAUSA ();

                    if (resultado_pausa == PAUSA_RESULTADO_CONTINUAR)
                    {
                        estado_juego = ESTADO_RUNNING;
                    }
                    else if (resultado_pausa == PAUSA_RESULTADO_GUARDAR_Y_SALIR)
                    {
                        PARTIDA_GUARDAR ();
                        corriendo = 0;
                    }
                    else if (resultado_pausa == PAUSA_RESULTADO_SALIR_SIN_GUARDAR)
                    {
                        PARTIDA_BORRAR ();
                        corriendo = 0;
                    }
                }

                if (estado_juego == ESTADO_RUNNING)
                {
                    if (gbt_tecla_presionada (GBTK_d))
                    {
                        ROTARHORARIO ();
                    }
                    if (gbt_tecla_presionada (GBTK_a))
                    {
                        ROTARANTIHORARIO ();
                    }

                    if (gbt_tecla_presionada (GBTK_c) && !cheat_activo && cheat_cooldown_restante <= 0.0)
                    {
                        cheat_activo = 1;
                        cheat_tiempo_restante = 5.0;
                        duracion_caida *= 3.0;
                        duracion_actual = duracion_caida;
                        gbt_temporizador_destruir (timer_caida);
                        timer_caida = gbt_temporizador_crear (duracion_actual);
                        if (timer_fijacion)
                        {
                            gbt_temporizador_destruir (timer_fijacion);
                            timer_fijacion = NULL;
                        }
                    }

                    if (cheat_activo)
                    {
                        cheat_tiempo_restante -= 0.016;
                        if (cheat_tiempo_restante <= 0.0)
                        {
                            cheat_activo = 0;
                            cheat_cooldown_restante = 60.0;
                            duracion_caida /= 3.0;
                            duracion_actual = duracion_caida;
                            gbt_temporizador_destruir(timer_caida);
                            timer_caida = gbt_temporizador_crear(duracion_actual);
                            if (timer_fijacion)
                            {
                                gbt_temporizador_destruir(timer_fijacion);
                                timer_fijacion = NULL;
                            }
                        }
                    }

                    if (cheat_cooldown_restante > 0.0)
                    {
                        cheat_cooldown_restante -= 0.016;
                    }

                    if (gbt_temporizador_consumir (timer_mov))
                    {
                        if (gbt_tecla_sostenida(GBTK_IZQUIERDA) && !COLISION (actual.fila, actual.columna - 1, actual.forma))
                        {
                            actual.columna --;
                            if (config_actual.modo_juego == MODO_DX)
                            {
                                if (actual.columna < 0)
                                {
                                    actual.columna += columnasTablero;
                                }
                                if (actual.columna >= columnasTablero)
                                {
                                    actual.columna -= columnasTablero;
                                }
                            }
                        }

                        if (gbt_tecla_sostenida(GBTK_DERECHA) && !COLISION (actual.fila, actual.columna + 1, actual.forma))
                        {
                            actual.columna ++;
                            if (config_actual.modo_juego == MODO_DX)
                            {
                                if (actual.columna < 0)
                                {
                                    actual.columna += columnasTablero;
                                }
                                if (actual.columna >= columnasTablero)
                                {
                                    actual.columna -= columnasTablero;
                                }
                            }
                        }

                        if (gbt_tecla_sostenida (GBTK_ABAJO) && !COLISION (actual.fila + 1, actual.columna, actual.forma))
                        {
                            actual.fila ++;
                            mult = (nivel / 2) + 1;
                            if (mult > 5)
                            {
                                mult = 5;
                            }
                            bonus = (int) ((1.0 - duracion_caida) * 10);
                            if (bonus < 0)
                            {
                                bonus = 0;
                            }
                            puntaje += mult + bonus;
                        }

                        if (COLISION (actual.fila + 1, actual.columna, actual.forma))
                        {
                            if (timer_fijacion == NULL)
                            {
                                timer_fijacion = gbt_temporizador_crear (duracion_caida * 0.5);
                            }

                            if (gbt_temporizador_consumir (timer_fijacion))
                            {
                                FIJARPIEZA ();
                                LIMPIARLINEAS ();
                                pieza_fijada_sin_nueva = 1;
                                gbt_temporizador_destruir (timer_fijacion);
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
                                gbt_temporizador_destruir (timer_fijacion);
                                timer_fijacion = NULL;
                            }

                            if (gbt_temporizador_consumir (timer_caida))
                            {
                                actual.fila++;
                            }
                        }
                    }
                }
            }
            else if (estado_juego == ESTADO_GAMEOVER)
            {
                if (!ya_guardo)
                {
                    STATS_GUARDAR (nombreJugador, puntaje);
                    PARTIDA_BORRAR ();
                    ya_guardo = 1;
                }

                if (gbt_tecla_presionada (GBTK_r))
                {
                    ya_guardo = 0;
                    REINICIARJUEGO ();
                    duracion_caida = velocidades_disponibles [config_actual.velocidad_inicial];
                    gbt_temporizador_destruir (timer_caida);
                    timer_caida = gbt_temporizador_crear (duracion_caida);
                    duracion_actual = duracion_caida;
                    pieza_fijada_sin_nueva = 0;
                    if (timer_fijacion)
                    {
                        gbt_temporizador_destruir (timer_fijacion);
                        timer_fijacion = NULL;
                    }
                    NUEVAPIEZA ();
                }

                if (gbt_tecla_presionada (GBTK_ENTER) || gbt_tecla_presionada (GBTK_ESCAPE))
                {
                    ya_guardo = 0;
                    corriendo = 0;
                }
            }

            ACTUALIZAR_ANIMACION_BORRADO ();

            if (pieza_fijada_sin_nueva && !animacion_borrado_activa)
            {
                NUEVAPIEZA ();
                pieza_fijada_sin_nueva = 0;
            }

            DIBUJAR ();
            if (estado_juego == ESTADO_GAMEOVER)
            {
                DIBUJARGAMEOVER ();
            }

            gbt_volcar_backbuffer ();
            gbt_esperar(16);
        }
    }

    gbt_temporizador_destruir (timer_caida);
    gbt_temporizador_destruir (timer_mov);
    if (timer_fijacion)
    {
        gbt_temporizador_destruir (timer_fijacion);
    }

    return salir_juego;
}

// Pantalla presentación: logo TETRIS + "PRESIONE ENTER" hasta que el usuario comience
static void PANTALLA_PRESENTACION (void)
{
    int logoX, logoY, anchoMsg, idx;
    const char *msg, *p;

    msg = "PRESIONE ENTER";

    while (1)
    {
        gbt_procesar_entrada ();

        if (gbt_tecla_presionada (GBTK_ENTER))
        {
            return;
        }

        if (gbt_tecla_sostenida (GBTK_q))
        {
            exit(0);
        }

        gbt_borrar_backbuffer (COLOR_NEGRO);
        DIBUJARFONDO ();

        logoX = (CONFIG_ANCHO () - 167) / 2;
        logoY = (CONFIG_ALTO () - 110) / 3;
        DIBUJAR_LOGO_COMPLETO (logoX, logoY);

        anchoMsg = 0;
        for (p = msg; *p; p ++)
        {
            idx = -1;
            if (*p >= 'A' && *p <= 'Z')
            {
                idx = *p - 'A';
            }
            else if (*p >= '0' && *p <= '9')
            {
                idx = 26 + (*p - '0');
            }
            else if (*p == ' ')
            {
                idx = 36;
            }
            if (idx >= 0)
            {
                anchoMsg += anchoProp[idx] + 1;
            }
        }

        DIBUJARTEXTOPROP ((CONFIG_ANCHO() - anchoMsg) / 2, logoY + 134, msg, 11);

        gbt_volcar_backbuffer ();
        gbt_esperar (16);
    }
}

int main (int argc, char *argv [])
{
    char nombreVentana [128];
    eMenuResultado resultado;
    int cargando, salir, menu_activo;

    if (gbt_iniciar () != 0)
    {
        fprintf (stderr, "Error al iniciar GBT: %s\n", gbt_obtener_log ());
        return -1;
    }

    if (CONFIG_CARGAR (&config_actual) != 0)
    {
        CONFIG_DEFAULTS (&config_actual);
    }

    PARSEAR_ARGV (argc, argv, &config_actual);

    if (config_actual.modo_juego == MODO_DX)
    {
        piezas_en_uso = MAX_PIEZAS;
    }
    else
    {
        piezas_en_uso = 7;
    }
    if (config_actual.modo_juego == MODO_DX)
    {
        columnasTablero = config_actual.ancho_tablero;
    }
    else
    {
        columnasTablero = 10;
    }

    sprintf (nombreVentana, "Tetris %dx%d", CONFIG_ANCHO (), CONFIG_ALTO ());
    if (gbt_crear_ventana (nombreVentana, CONFIG_ANCHO (), CONFIG_ALTO (), config_actual.escala) != 0)
    {
        fprintf (stderr, "Error al crear ventana: %s\n", gbt_obtener_log ());
        return -1;
    }

    CONFIG_APLICAR (&config_actual);
    srand (time (0));

    PANTALLA_PRESENTACION ();

    menu_activo = 1;
    while (menu_activo)
    {
        resultado = MENU_PRINCIPAL ();

        if (resultado == MENU_RESULTADO_SALIR)
        {
            menu_activo = 0;
        }
        else
        {
            if (resultado == MENU_RESULTADO_CONTINUAR)
            {
                cargando = 1;
            }
            else
            {
                cargando = 0;
            }
            salir = JUGAR (cargando);
            if (salir)
            {
                menu_activo = 0;
            }
        }
    }

    gbt_destruir_ventana ();
    gbt_cerrar ();
    return 0;
}
