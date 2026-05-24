#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED

#include "funcionalidades.h"
#include "configuracion.h"

#define cantColores 16 // Cantidad de colores en la paleta

// Resolución dinámica según configuración activa
#define anchoVentana CONFIG_ANCHO () // Ancho de la ventana (320 o 640)
#define altoVentana CONFIG_ALTO () // Alto de la ventana (200 o 480)

// Offsets para centrar el tablero en la ventana
#define offsetHorizontal (CONFIG_ANCHO() - columnasTablero * tamMino) / 2
#define offsetVertical   (CONFIG_ALTO()  - filasTablero   * tamMino) / 2

// Variables globales de paleta de colores
extern int colorBrillo [cantColores]; // Índice de color para brillo
extern int colorSombra [cantColores]; // Índice de color para sombra

void DIBUJAR (); // Dibuja el frame completo del juego
void DIBUJARFONDO (); // Dibuja el fondo
void DIBUJARMARCO (); // Marco alrededor del tablero
void DIBUJARMARCOGENERICO (int x0, int y0, int ancho, int alto, int colorFondo); // Marco genérico
void DIBUJARGRILLA (); // Líneas divisorias entre celdas del tablero
void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color);
void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter);
void DIBUJARPROXIMA (); // Panel NEXT (próxima pieza)
void DIBUJARPUNTAJE (); // Panel de puntaje, nivel, líneas, velocidad
void DIBUJARPAUSA (); // Superposición de pausa
void DIBUJARGAMEOVER (); // Superposición de Game Over
void DIBUJARINICIO (char *nombre); // Pantalla de ingreso de nombre
void DIBUJARCHEAT (); // Indicador de CHEAT en el panel lateral
void DIBUJARTEXTOPROP (int x, int y, const char *texto, int color); // Texto con fuente proporcional

#endif // GRAFICOS_H_INCLUDED
