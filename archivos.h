<<<<<<< HEAD
#ifndef TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H
#define TRABAJO_PRACTICO_TOPICOS_TETRIS_ARCHIVOS_H

#include "funcionalidades.h"

#define NOMBRE_ARCHIVO_PARTIDA "partida.dat"

typedef struct {
    int tablero[filasTablero][MAX_COLUMNAS];
=======
#ifndef ARCHIVOS_H_INCLUDED
#define ARCHIVOS_H_INCLUDED

#include "funcionalidades.h"
#include "configuracion.h"

#define NOMBRE_ARCHIVO_PARTIDA "partida.dat"

// Estructura que guarda el estado completo de una partida
typedef struct {
    int tablero[filasTablero][columnasTablero];
>>>>>>> 8eda37d6c022504925005826f357e00d6ab5bd71
    sPieza actual;
    sPieza proxima;
    int puntaje;
    int nivel;
    int lineas_totales;
    int piezas_caidas;
    double duracion_caida;
    char nombreJugador[21];
<<<<<<< HEAD
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
=======
    int bolsa[7];
    int bolsaIndice;
    int valido; // 1 si hay partida guardada
} sEstadoPartida;

// Devuelve 1 si existe una partida guardada válida
int PARTIDA_EXISTE(void);

// Guarda la partida actual en el archivo binario. Retorna 0 si OK, -1 si error.
int PARTIDA_GUARDAR(void);

// Carga la partida desde el archivo binario. Retorna 0 si OK, -1 si error.
int PARTIDA_CARGAR(void);

// Borra el archivo de partida guardada
void PARTIDA_BORRAR(void);

#endif // ARCHIVOS_H_INCLUDED
>>>>>>> 8eda37d6c022504925005826f357e00d6ab5bd71
