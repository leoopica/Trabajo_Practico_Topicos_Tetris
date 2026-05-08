#ifndef INICIO_H_INCLUDED
#define INICIO_H_INCLUDED

#include <stdint.h>
#include "gbt.h"


// --- COLORES DE LA PALETA CGA ---
#define COLOR_NEGRO     0
#define COLOR_AZUL      1
#define COLOR_VERDE     2
#define COLOR_CIAN      3
#define COLOR_ROJO      4
#define COLOR_MAGENTA   5
#define COLOR_MARRON    6
#define COLOR_GRIS_CLARO     7
#define COLOR_GRIS_OSCURO    8
#define COLOR_AZUL_CLARO     9
#define COLOR_VERDE_CLARO    10
#define COLOR_CIAN_CLARO     11
#define COLOR_ROJO_CLARO     12
#define COLOR_MAGENTA_CLARO  13
#define COLOR_AMARILLO       14

#define LOGO_TEXTO_ANCHO 49
#define LOGO_TEXTO_ALTO  13

extern const char* sprite_tetris_ascii[LOGO_TEXTO_ALTO];

void dibujar_rectangulo(int x_origen, int y_origen, int ancho, int alto, int color);
void dibujar_borde_rectangulo(int x_origen, int y_origen, int ancho, int alto, int color);
void dibujar_texto_tetris(int x_origen, int y_origen);
void DIBUJARLOGOCOMPLETO(int x_base, int y_base);

#endif