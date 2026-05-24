#ifndef INICIO_H_INCLUDED
#define INICIO_H_INCLUDED

// COLORES DE LA PALETA CGA 
// Índice 15 está reservado como transparente por GBT
#define COLOR_NEGRO 0
#define COLOR_AZUL 1
#define COLOR_VERDE 2
#define COLOR_CIAN 3
#define COLOR_ROJO 4
#define COLOR_MAGENTA 5
#define COLOR_MARRON 6
#define COLOR_GRIS_CLARO 7
#define COLOR_GRIS_OSCURO 8
#define COLOR_AZUL_CLARO 9
#define COLOR_VERDE_CLARO 10
#define COLOR_CIAN_CLARO 11
#define COLOR_ROJO_CLARO 12
#define COLOR_MAGENTA_CLARO 13
#define COLOR_AMARILLO 14

// Dimensiones del texto "TETRIS" en el logo (mapa ASCII 49×13 caracteres)
#define LOGO_TEXTO_ANCHO 49
#define LOGO_TEXTO_ALTO  13

void DIBUJAR_RECTANGULO (int x_origen, int y_origen, int ancho, int alto, int color);
void DIBUJAR_BORDE_RECTANGULO (int x_origen, int y_origen, int ancho, int alto, int color);
void DIBUJAR_TEXTO_TETRIS (int x_origen, int y_origen);
void DIBUJAR_LOGO_COMPLETO (int x_base, int y_base);

#endif // INICIO_H_INCLUDED
