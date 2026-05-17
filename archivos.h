#ifndef ARCHIVOS_H_INCLUDED
#define ARCHIVOS_H_INCLUDED

#include "funcionalidades.h"
#include "configuracion.h"

#define NOMBRE_ARCHIVO_PARTIDA "partida.dat"

// Estructura que guarda el estado completo de una partida
typedef struct {
    int tablero[filasTablero][columnasTablero];
    sPieza actual;
    sPieza proxima;
    int puntaje;
    int nivel;
    int lineas_totales;
    int piezas_caidas;
    double duracion_caida;
    char nombreJugador[21];
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