#include "archivos.h"
#include "configuracion.h"
#include <stdio.h>
#include <string.h>

extern int bolsa [MAX_PIEZAS];   // Array con la secuencia de piezas
extern int bolsaIndice;         // Posición actual en la bolsa

// Verifica si existe una partida guardada válida en disco. 1 = existe y es válida, 0 = no existe o es inválida
int PARTIDA_EXISTE ()
{
    int leidos;
    FILE *f;
    sEstadoPartida estado;

    f = fopen (NOMBRE_ARCHIVO_PARTIDA, "rb");
    if (!f)
    {
        return 0;
    }

    leidos = (int) fread (&estado, sizeof (sEstadoPartida), 1, f);
    fclose (f);

    if (leidos == 1 && estado.valido == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Guarda el estado actual de la partida en el archivo partida.dat. 0 = éxito, -1 = error al escribir
int PARTIDA_GUARDAR ()
{
    sEstadoPartida estado;
    int fi, c, i, ok;
    FILE *f;

    for (fi = 0; fi < FILAS_TOTALES; fi++)
    {
        for (c = 0; c < columnasTablero; c++)
        {
            estado.tablero [fi][c] = tablero [fi][c];
        }
    }

    estado.actual = actual;
    estado.proxima = proxima;
    estado.puntaje = puntaje;
    estado.nivel = nivel;
    estado.lineas_totales = lineas_totales;
    estado.piezas_caidas = piezas_caidas;
    estado.duracion_caida= duracion_caida;

    strncpy (estado.nombreJugador, nombreJugador, 21);
    estado.nombreJugador [20] = '\0';

    for (i = 0; i < piezas_en_uso; i++)
    {
        estado.bolsa [i] = bolsa [i];
    }
    estado.bolsaIndice = bolsaIndice;

    estado.valido = 1;
    estado.cantidad_piezas = piezas_en_uso;
    estado.ancho_tablero = columnasTablero;
    estado.modo_juego = config_actual.modo_juego;

    f = fopen (NOMBRE_ARCHIVO_PARTIDA, "wb");
    if (!f)
    {
        return -1;
    }

    ok = (int) fwrite (&estado, sizeof(sEstadoPartida), 1, f);
    fclose (f);

    if (ok == 1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

// Carga una partida guardada desde partida.dat y restaura todas las variables. 0 = éxito, -1 = error
int PARTIDA_CARGAR ()
{
    sEstadoPartida estado;
    FILE *f;
    int leidos, fi, c, i, piezas_guardadas;

    f = fopen (NOMBRE_ARCHIVO_PARTIDA, "rb");
    if (!f)
    {
        return -1;
    }

    leidos = (int) fread (&estado, sizeof (sEstadoPartida), 1, f);
    fclose (f);

    if (leidos != 1 || estado.valido != 1)
    {
        return -1;
    }

    for (fi = 0; fi < FILAS_TOTALES; fi++)
    {
        filas_tablero [fi] = celdas_tablero [fi];
    }
    tablero = filas_tablero;

    for (fi = 0; fi < FILAS_TOTALES; fi++)
    {
        for (c = 0; c < columnasTablero; c++)
        {
            tablero[fi][c] = estado.tablero[fi][c];
        }
    }

    actual = estado.actual;
    proxima = estado.proxima;
    puntaje = estado.puntaje;
    nivel = estado.nivel;
    lineas_totales = estado.lineas_totales;
    piezas_caidas  = estado.piezas_caidas;
    duracion_caida = estado.duracion_caida;

    strncpy (nombreJugador, estado.nombreJugador, 21);
    nombreJugador [20] = '\0';

    piezas_guardadas = estado.cantidad_piezas;
    if (piezas_guardadas < 7 || piezas_guardadas > MAX_PIEZAS)
    {
        piezas_guardadas = piezas_en_uso;
    }

    for (i = 0; i < piezas_guardadas; i++)
    {
        bolsa [i] = estado.bolsa [i];
    }
    bolsaIndice = estado.bolsaIndice;

    piezas_en_uso = piezas_guardadas;

    if (estado.ancho_tablero >= 8 && estado.ancho_tablero <= MAX_COLUMNAS)
    {
        columnasTablero = estado.ancho_tablero;
    }

    if (estado.modo_juego == MODO_CLASICO || estado.modo_juego == MODO_DX)
    {
        config_actual.modo_juego = estado.modo_juego;
    }

    estado_juego = ESTADO_RUNNING;
    return 0;
}

// Borra la partida guardada: marca la estructura como inválida y la sobrescribe
void PARTIDA_BORRAR ()
{
    sEstadoPartida estado;
    FILE *f;

    estado = (sEstadoPartida){0};

    f = fopen (NOMBRE_ARCHIVO_PARTIDA, "wb");
    if (f)
    {
        fwrite(&estado, sizeof(sEstadoPartida), 1, f);
        fclose(f);
    }
}
