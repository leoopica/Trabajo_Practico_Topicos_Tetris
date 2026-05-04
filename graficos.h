#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED

#include <stdio.h>
#include "gbt.h"
#include "sprites.h"

// Constantes
#define cantColores 16 // Define cantidad de colores
#define filasTablero 20 // Define cantidad de filasTablero del tablero
#define columnasTablero 10 // Define cantidad de columnasTablero del tablero
#define anchoVentana 320 // Define ancho de la ventana
#define altoVentana 200 // Define alto de la ventana
#define escalaVentana 2 // Define el escalado de la ventana
#define offsetHorizontal (anchoVentana - columnasTablero * tamMino) / 2 // Define el margen horizontal desde la izquierda
#define offsetVertical (altoVentana - filasTablero * tamMino) / 2 // Define el margen vertical desde abajo

// Estructura para las piezas
typedef struct
{
    int forma [4][4]; // Matriz de forma de la pieza
    int fila, columna;
    int color;
    int rotacion; // Estado de rotación (0-3) para SRS
    int tipo;     // Tipo de pieza (0-6)
} sPieza;

// Variables globales - extern permite que se compartan las variables globales entre archivos
extern int tablero [filasTablero][columnasTablero]; // Define el tablero
extern int piezas [cantPiezas][4][4]; // Define las piezas -> Cantidad de piezas, alto, ancho
extern tGBT_ColorRGB paletaCGA [cantColores]; // Define paleta de colores
extern int colorBrillo [cantColores]; // Define colores para la parte de brillo de las piezas
extern int colorSombra [cantColores]; // Define colores para la parte de sombra de las piezas
extern sPieza actual;
extern int puntaje;
extern int nivel;
extern int lineas_totales;
extern int piezas_caidas;
extern double duracion_caida;

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
void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color); // PONER COMENTARIOS
void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter); // PONER COMENTARIOS
void DIBUJARPUNTAJE (); // PONER COMENTARIOS
void DIBUJARTITULO (); // PONER COMENTARIOS
void DIBUJARINICIO (char *nombre); // PONER COMENTARIOS

#endif