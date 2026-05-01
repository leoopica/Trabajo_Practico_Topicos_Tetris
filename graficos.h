#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED

#include <stdio.h>
#include "gbt.h"

// Constantes
#define CANT_COLORES 16
#define filas 20
#define columnas 10
#define cantPiezas 7
#define TAM_CELDA 8
#define ANCHO_VENTANA 320
#define ALTO_VENTANA 200
#define ESCALA_VENTANA 2
#define OFFSET_X (ANCHO_VENTANA - columnas * TAM_CELDA) / 2
#define OFFSET_Y (ALTO_VENTANA - filas * TAM_CELDA) / 2

// Estructura para las piezas
typedef struct
{
    int forma [4][4];
    int fila, columna;
    int color;
} sPieza;

// 🔹 Variables globales (DECLARADAS)
extern int tablero [filas][columnas];
extern int piezas [cantPiezas][4][4];
extern tGBT_ColorRGB paletaCGA [CANT_COLORES];
extern int colorBrillo [CANT_COLORES];
extern int colorSombra [CANT_COLORES];
extern sPieza actual;

// Funciones
void NUEVAPIEZA ();
void COPIARPIEZA (int destino [4][4], int origen [4][4]);
void DIBUJAR ();
int COLISION (int filaNueva, int columnaNueva, int forma [4][4]);
void FIJARPIEZA ();
void LIMPIARLINEAS ();
void ROTARHORARIO ();
void ROTARANTIHORARIO ();

#endif