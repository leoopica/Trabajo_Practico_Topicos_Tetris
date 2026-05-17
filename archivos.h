#ifndef TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H
#define TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H

#include "funcionalidades.h"

#define NOMBRE_ARCHIVO_PARTIDA "partida.dat"

typedef struct {
    int tablero[filasTablero][MAX_COLUMNAS];
    sPieza actual;
    sPieza proxima;
    int puntaje;
    int nivel;
    int lineas_totales;
    int piezas_caidas;
    double duracion_caida;
    char nombreJugador[21];
    int bolsa[MAX_PIEZAS];
    int bolsaIndice;
    int valido;
    int cantidad_piezas; // Cantidad de piezas activas al guardar (7 clásico, 11 DX)
    int ancho_tablero;   // Ancho del tablero al guardar (8-16)
    int modo_juego;      // Modo de juego al guardar (MODO_CLASICO o MODO_DX)
} sEstadoPartida;

int PARTIDA_EXISTE(void);
int PARTIDA_GUARDAR(void);
int PARTIDA_CARGAR(void);
void PARTIDA_BORRAR(void);

#endif //TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H
