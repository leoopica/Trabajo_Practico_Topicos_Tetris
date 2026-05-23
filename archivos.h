#ifndef TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H
#define TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H

#include "funcionalidades.h"
#include "configuracion.h"

#define NOMBRE_ARCHIVO_PARTIDA "partida.dat"

typedef struct {
    int tablero[FILAS_TOTALES][MAX_COLUMNAS]; // 24 filas (20 visibles + 4 ocultas)
    sPieza actual;                            // |  columnas físicas (16)      |
    sPieza proxima;                           // +-------------------------------+
    int puntaje;
    int nivel;
    int lineas_totales;
    int piezas_caidas;
    double duracion_caida;
    char nombreJugador[21];
    int bolsa[MAX_PIEZAS];
    int bolsaIndice;
    int valido;          // 1 = partida válida, 0 = inválida
    int cantidad_piezas; // 7 clásico, 11 DX (para restaurar bolsa)
    int ancho_tablero;   // Ancho real (8-16) al guardar
    int modo_juego;      // MODO_CLASICO o MODO_DX
} sEstadoPartida;

int PARTIDA_EXISTE(void);  // 1 si hay partida guardada válida
int PARTIDA_GUARDAR(void); // 0 éxito, -1 error
int PARTIDA_CARGAR(void);  // 0 éxito, -1 error/no existe
void PARTIDA_BORRAR(void);

#endif // TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H
