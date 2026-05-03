#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED

#include <stdio.h>
#include "gbt.h"

// Constantes
#define CANT_COLORES 16 // Define cantidad de colores
#define filas 20 // Define cantidad de filas del tablero
#define columnas 10 // Define cantidad de columnas del tablero
#define cantPiezas 7 // Define la cantidad de piezas
#define TAM_CELDA 8 // Define tamaño de cada mino en pixeles
#define ANCHO_VENTANA 320 // Define ancho de la ventana
#define ALTO_VENTANA 200 // Define alto de la ventana
#define ESCALA_VENTANA 2 // Define el escalado de la ventana
#define OFFSET_X (ANCHO_VENTANA - columnas * TAM_CELDA) / 2 // Define el margen horizontal desde la izquierda
#define OFFSET_Y (ALTO_VENTANA - filas * TAM_CELDA) / 2 // Define el margen vertical desde abajo
#define cantCaracteres 36 // 26 letras, 10 números

// Estructura para las piezas
typedef struct
{
    int forma [4][4]; // Matriz de forma de la pieza
    int fila, columna;
    int color;
} sPieza;

// Variables globales - extern permite que se compartan las variables globales entre archivos
extern int tablero [filas][columnas]; // Define el tablero
extern int piezas [cantPiezas][4][4]; // Define las piezas -> Cantidad de piezas, alto, ancho
extern tGBT_ColorRGB paletaCGA [CANT_COLORES]; // Define paleta de colores
extern int colorBrillo [CANT_COLORES]; // Define colores para la parte de brillo de las piezas
extern int colorSombra [CANT_COLORES]; // Define colores para la parte de sombra de las piezas
extern sPieza actual;

// Funciones
void NUEVAPIEZA (); // Elige una nueva pieza para que caiga en el tablero
void COPIARPIEZA (int destino [4][4], int origen [4][4]); // Copia la forma de la pieza de un lado a otro
void DIBUJAR (); // Dibuja los gráficos
int COLISION (int filaNueva, int columnaNueva, int forma [4][4]); // Evalúa la colisión de la pieza que está cayendo
void FIJARPIEZA (); // Fija la pieza en el tablero
void LIMPIARLINEAS (); // Limpia una fila si se completa la línea
void ROTARHORARIO ();
void ROTARANTIHORARIO ();
void DIBUJARFONDO (); // Dibuja el fondo
void DIBUJARMARCO (); // Dibuja el marco del tablero
void DIBUJARGRILLA (); // Dibuja la grilla del tablero

#endif