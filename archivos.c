#include "archivos.h"
#include "configuracion.h"
#include <stdio.h>
#include <string.h>

extern int bolsa[MAX_PIEZAS];
extern int bolsaIndice;

int PARTIDA_EXISTE(void)
{
    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "rb");
    if (!f) return 0;

    sEstadoPartida estado;
    int leidos = (int)fread(&estado, sizeof(sEstadoPartida), 1, f);
    fclose(f);

    return (leidos == 1 && estado.valido == 1);
}

int PARTIDA_GUARDAR(void)
{
    sEstadoPartida estado;

    // Copiar estado del tablero
    for (int f = 0; f < filasTablero; f++)
        for (int c = 0; c < columnasTablero; c++)
            estado.tablero[f][c] = tablero[f][c];

    estado.actual        = actual;
    estado.proxima       = proxima;
    estado.puntaje       = puntaje;
    estado.nivel         = nivel;
    estado.lineas_totales= lineas_totales;
    estado.piezas_caidas = piezas_caidas;
    estado.duracion_caida= duracion_caida;
    strncpy(estado.nombreJugador, nombreJugador, 21);
    estado.nombreJugador[20] = '\0';

    // Guardar el sistema de bolsa
    for (int i = 0; i < piezas_en_uso; i++)
        estado.bolsa[i] = bolsa[i];
    estado.bolsaIndice = bolsaIndice;

    estado.valido = 1;
    estado.cantidad_piezas = piezas_en_uso;
    estado.ancho_tablero = columnasTablero;
    estado.modo_juego = config_actual.modo_juego;

    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "wb");
    if (!f) return -1;

    int ok = (int)fwrite(&estado, sizeof(sEstadoPartida), 1, f);
    fclose(f);
    return (ok == 1) ? 0 : -1;
}

int PARTIDA_CARGAR(void)
{
    sEstadoPartida estado;
    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "rb");
    if (!f) return -1;

    int leidos = (int)fread(&estado, sizeof(sEstadoPartida), 1, f);
    fclose(f);

    if (leidos != 1 || estado.valido != 1) return -1;

    // Inicializar punteros del tablero (necesario para el tablero dinámico)
    for (int fi = 0; fi < filasTablero; fi++)
        filas_tablero[fi] = celdas_tablero[fi];
    tablero = filas_tablero;

    // Restaurar estado del tablero
    for (int fi = 0; fi < filasTablero; fi++)
        for (int c = 0; c < columnasTablero; c++)
            tablero[fi][c] = estado.tablero[fi][c];

    actual         = estado.actual;
    proxima        = estado.proxima;
    puntaje        = estado.puntaje;
    nivel          = estado.nivel;
    lineas_totales = estado.lineas_totales;
    piezas_caidas  = estado.piezas_caidas;
    duracion_caida = estado.duracion_caida;
    strncpy(nombreJugador, estado.nombreJugador, 21);
    nombreJugador[20] = '\0';

    // Restaurar bolsa
    int piezas_guardadas = estado.cantidad_piezas;
    if (piezas_guardadas < 7 || piezas_guardadas > MAX_PIEZAS)
        piezas_guardadas = piezas_en_uso;
    for (int i = 0; i < piezas_guardadas; i++)
        bolsa[i] = estado.bolsa[i];
    bolsaIndice = estado.bolsaIndice;

    // Restaurar la cantidad de piezas activas al momento de guardar
    piezas_en_uso = piezas_guardadas;

    // Restaurar ancho del tablero
    if (estado.ancho_tablero >= 8 && estado.ancho_tablero <= MAX_COLUMNAS)
        columnasTablero = estado.ancho_tablero;

    // Restaurar modo de juego para mantener circularidad del tablero
    if (estado.modo_juego == MODO_CLASICO || estado.modo_juego == MODO_DX)
        config_actual.modo_juego = estado.modo_juego;

    estado_juego = ESTADO_RUNNING;
    return 0;
}

void PARTIDA_BORRAR(void)
{
    // Sobrescribir con valido=0 para marcarla como inválida
    sEstadoPartida estado = {0};
    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "wb");
    if (f)
    {
        fwrite(&estado, sizeof(sEstadoPartida), 1, f);
        fclose(f);
    }
}