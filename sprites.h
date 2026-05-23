#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <stdio.h>
#include "gbt.h"

#define cantPiezas 11      // 7 clásicas (I,J,L,O,S,T,Z) + 4 DX (x,c,p,*)
#define MAX_PIEZAS 11      // Máximo para arrays estáticos (modo DX)

#define cantCaracteres 38  // 26 letras (A-Z) + 10 números (0-9) + 1 espacio + 1 guión bajo
#define altoCaracter 8     // Alto fijo de todas las fuentes (8 píxeles)
#define anchoCaracter8  8  // Ancho fuente 8×8
#define anchoCaracter16 16 // Ancho fuente 8×16

// Datos de las piezas (11 tipos, cada una 4×4)
extern int piezas [cantPiezas][4][4];

// Fuentes monoespaciadas: 8×8 (CGA) y 8×16 (VGA)
extern int fuente8x8 [cantCaracteres][altoCaracter][anchoCaracter8];
extern int fuente8x16 [cantCaracteres][altoCaracter][anchoCaracter16];

// Fuente proporcional (ancho variable, generada automáticamente desde la 8×8)
#define MAX_ANCHO_PROP 12 // Ancho máximo de un carácter proporcional
extern int fuenteProp[cantCaracteres][altoCaracter][MAX_ANCHO_PROP];
extern int anchoProp[cantCaracteres];     // Ancho real de cada carácter
void GENERAR_FUENTE_PROPORCIONAL(void);  // Recorta columnas vacías de fuente8x8

#endif // SPRITES_H_INCLUDED