#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED

#include <stdio.h>
#include "gbt.h"
#include "sprites.h"
#include "inicio.h"
#include "funcionalidades.h"

// Constantes
#define cantColores 16 // Define cantidad de colores
#define anchoVentana 320 // Define ancho de la ventana
#define altoVentana 200 // Define alto de la ventana
#define escalaVentana 2 // Define el escalado de la ventana
#define offsetHorizontal (anchoVentana - columnasTablero * tamMino) / 2 // Define el margen horizontal desde la izquierda
#define offsetVertical (altoVentana - filasTablero * tamMino) / 2 // Define el margen vertical desde abajo


// Variables globales - extern permite que se compartan las variables globales entre archivos
extern tGBT_ColorRGB paletaCGA [cantColores]; // Define paleta de colores
extern int colorBrillo [cantColores]; // Define colores para la parte de brillo de las piezas
extern int colorSombra [cantColores]; // Define colores para la parte de sombra de las piezas

// Funciones
void DIBUJAR (); // Dibuja los gráficos
void DIBUJARFONDO (); // Dibuja el fondo
void DIBUJARMARCO (); // Dibuja el marco del tablero
void DIBUJARMARCOGENERICO (int x0, int y0, int ancho, int alto, int colorFondo); // Dibuja un marco 3D en cualquier posición
void DIBUJARGRILLA (); // Dibuja la grilla del tablero
void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color); // PONER COMENTARIOS
void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter); // PONER COMENTARIOS
void DIBUJARPROXIMA (); // Dibuja la pieza siguiente
void DIBUJARPUNTAJE (); // PONER COMENTARIOS
void DIBUJARTITULO (); // PONER COMENTARIOS
void DIBUJARPAUSA ();
void DIBUJARGAMEOVER ();
void DIBUJARINICIO (char *nombre); // PONER COMENTARIOS

#endif