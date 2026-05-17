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
#include "archivos.h"

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
            i++;
        }
    }
}

// Ejecuta un loop de partida. Si cargando==1, restaura desde archivo; si no, empieza nueva.
// Retorna: 0 = volver al menú, 1 = salir del programa
static int JUGAR(int cargando)
{
    if (cargando)
    {
        // Cargar partida guardada
        if (PARTIDA_CARGAR() != 0)
        {
            // Si falla la carga, iniciar nueva
            DIBUJARINICIO(nombreJugador);
            if (nombreJugador[0] == '\0') return 0; // ESC: volver al menú
            columnasTablero = (config_actual.modo_juego == MODO_DX) ? config_actual.ancho_tablero : 10;
            piezas_en_uso = (config_actual.modo_juego == MODO_DX) ? MAX_PIEZAS : 7;
            REINICIARJUEGO();
        }
    }
    else
    {
        // Nueva partida
        DIBUJARINICIO(nombreJugador);
        if (nombreJugador[0] == '\0') return 0; // ESC: volver al menú
        columnasTablero = (config_actual.modo_juego == MODO_DX) ? config_actual.ancho_tablero : 10;
        piezas_en_uso = (config_actual.modo_juego == MODO_DX) ? MAX_PIEZAS : 7;
        REINICIARJUEGO();
    }

    pieza_fijada_sin_nueva = 0;
    animacion_estaba_activa = 0;

    duracion_caida = velocidades_disponibles[config_actual.velocidad_inicial];

    if (!cargando)
        NUEVAPIEZA();

    double duracion_actual = duracion_caida;

    tGBT_Temporizador *timer_caida = gbt_temporizador_crear(duracion_actual);
    if (!timer_caida) { fprintf(stderr, "Error temporizador\n"); return 1; }

    tGBT_Temporizador *timer_mov = gbt_temporizador_crear(0.1);
    if (!timer_mov) { fprintf(stderr, "Error temporizador\n"); return 1; }

    tGBT_Temporizador *timer_fijacion = NULL;

    uint8_t corriendo = 1;
    int salir_juego = 0;

    while (corriendo)
    {
        gbt_procesar_entrada();

        if (gbt_tecla_sostenida(GBTK_q))
        {
            salir_juego = 1;
            corriendo = 0;
        }
        else
        {
            if (estado_juego == ESTADO_RUNNING && !animacion_borrado_activa)
            {
                if (gbt_tecla_presionada(GBTK_p))
                {
                    // Abrir menú de pausa con opciones
                    ePausaResultado resultado_pausa = MENU_PAUSA();

                    if (resultado_pausa == PAUSA_RESULTADO_CONTINUAR)
                    {
                        estado_juego = ESTADO_RUNNING;
                    }
                    else if (resultado_pausa == PAUSA_RESULTADO_GUARDAR_Y_SALIR)
                    {
                        PARTIDA_GUARDAR();
                        corriendo = 0; // Volver al menú
                    }
                    else if (resultado_pausa == PAUSA_RESULTADO_SALIR_SIN_GUARDAR)
                    {
                        PARTIDA_BORRAR(); // Borrar cualquier guardado previo de esta partida
                        corriendo = 0; // Volver al menú
                    }
                }

                if (estado_juego == ESTADO_RUNNING)
                {
                    if (gbt_tecla_presionada(GBTK_d))
                        ROTARHORARIO();
                    if (gbt_tecla_presionada(GBTK_a))
                        ROTARANTIHORARIO();

                    // Cheat: C activa lentitud 5s, cooldown 60s
                    if (gbt_tecla_presionada(GBTK_c) && !cheat_activo && cheat_cooldown_restante <= 0.0)
                    {
                        cheat_activo = 1;
                        cheat_tiempo_restante = 5.0;
                        duracion_caida *= 3.0;
                        duracion_actual = duracion_caida;
                        gbt_temporizador_destruir(timer_caida);
                        timer_caida = gbt_temporizador_crear(duracion_actual);
                        if (timer_fijacion) { gbt_temporizador_destruir(timer_fijacion); timer_fijacion = NULL; }
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
                            if (timer_fijacion) { gbt_temporizador_destruir(timer_fijacion); timer_fijacion = NULL; }
                        }
                    }
                    if (cheat_cooldown_restante > 0.0)
                        cheat_cooldown_restante -= 0.016;

                    if (gbt_temporizador_consumir(timer_mov))
                    {
                        if (gbt_tecla_sostenida(GBTK_IZQUIERDA) && !COLISION(actual.fila, actual.columna - 1, actual.forma))
                        {
                            actual.columna--;
                            if (config_actual.modo_juego == MODO_DX)
                            {
                                if (actual.columna < 0) actual.columna += columnasTablero;
                                if (actual.columna >= columnasTablero) actual.columna -= columnasTablero;
                            }
                        }
                        if (gbt_tecla_sostenida(GBTK_DERECHA) && !COLISION(actual.fila, actual.columna + 1, actual.forma))
                        {
                            actual.columna++;
                            if (config_actual.modo_juego == MODO_DX)
                            {
                                if (actual.columna < 0) actual.columna += columnasTablero;
                                if (actual.columna >= columnasTablero) actual.columna -= columnasTablero;
                            }
                        }
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
            }
            else if (estado_juego == ESTADO_GAMEOVER)
            {
                static int ya_guardo = 0;
                if (!ya_guardo)
                {
                    STATS_GUARDAR(nombreJugador, puntaje);
                    PARTIDA_BORRAR(); // Borrar guardado al terminar partida
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
                    NUEVAPIEZA();
                }
                if (gbt_tecla_presionada(GBTK_ENTER) || gbt_tecla_presionada(GBTK_ESCAPE))
                {
                    ya_guardo = 0;
                    corriendo = 0;
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
            if (estado_juego == ESTADO_GAMEOVER) DIBUJARGAMEOVER();

            gbt_volcar_backbuffer();
            gbt_esperar(16);
        }
    }

    gbt_temporizador_destruir(timer_caida);
    gbt_temporizador_destruir(timer_mov);
    if (timer_fijacion) gbt_temporizador_destruir(timer_fijacion);

    return salir_juego;
}

static void PANTALLA_PRESENTACION(void)
{
    while (1)
    {
        gbt_procesar_entrada();
        if (gbt_tecla_presionada(GBTK_ENTER))
            return;
        if (gbt_tecla_sostenida(GBTK_q))
            exit(0);

        gbt_borrar_backbuffer(COLOR_NEGRO);
        DIBUJARFONDO();

        int logoX = (CONFIG_ANCHO() - 167) / 2;
        int logoY = (CONFIG_ALTO() - 110) / 3;
        DIBUJAR_LOGO_COMPLETO(logoX, logoY);

        const char *msg = "PRESIONE ENTER";
        int anchoMsg = 0;
        for (const char *p = msg; *p; p++)
        {
            int idx = -1;
            if (*p >= 'A' && *p <= 'Z') idx = *p - 'A';
            else if (*p >= '0' && *p <= '9') idx = 26 + (*p - '0');
            else if (*p == ' ') idx = 36;
            if (idx >= 0) anchoMsg += anchoProp[idx] + 1;
        }
        DIBUJARTEXTOPROP((CONFIG_ANCHO() - anchoMsg) / 2,
                         logoY + 134, msg, 11);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}

int main (int argc, char *argv[])
{
    char nombreVentana [128];

    if (gbt_iniciar() != 0)
    {
        fprintf(stderr, "Error al iniciar GBT: %s\n", gbt_obtener_log());
        return -1;
    }

    if (CONFIG_CARGAR(&config_actual) != 0)
        CONFIG_DEFAULTS(&config_actual);

    PARSEAR_ARGV(argc, argv, &config_actual);

    // Inicializar cantidad de piezas y ancho del tablero según el modo de juego
    piezas_en_uso = (config_actual.modo_juego == MODO_DX) ? MAX_PIEZAS : 7;
    columnasTablero = (config_actual.modo_juego == MODO_DX) ? config_actual.ancho_tablero : 10;

    sprintf(nombreVentana, "Tetris %dx%d", CONFIG_ANCHO(), CONFIG_ALTO());
    if (gbt_crear_ventana(nombreVentana, CONFIG_ANCHO(), CONFIG_ALTO(), config_actual.escala) != 0)
    {
        fprintf(stderr, "Error al crear ventana: %s\n", gbt_obtener_log());
        return -1;
    }

    CONFIG_APLICAR(&config_actual);

    GENERAR_FUENTE_PROPORCIONAL();
    srand(time(0));

    PANTALLA_PRESENTACION();

    while (1)
    {
        eMenuResultado resultado = MENU_PRINCIPAL();

        if (resultado == MENU_RESULTADO_SALIR)
            break;

        int cargando = (resultado == MENU_RESULTADO_CONTINUAR) ? 1 : 0;
        int salir = JUGAR(cargando);
        if (salir) break;
    }

    gbt_destruir_ventana();
    gbt_cerrar();
    return 0;
}
