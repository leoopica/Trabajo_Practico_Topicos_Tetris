#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <stdio.h>
#include "gbt.h"

#define cantPiezas 11 // Define la cantidad de piezas (7 clásicas + 4 DX)
#define MAX_PIEZAS 11 // Máximo de piezas (para arrays estáticos)
// tamMino ahora es dinámico: 8px en CGA, 16px en VGA
// Se accede via la función TAM_MINO() definida en configuracion.h
#define cantCaracteres 38 // 26 letras, 10 números, 1 vacío, 1 guion bajo
#define altoCaracter 8
#define anchoCaracter8 8
#define anchoCaracter16 16

extern int piezas [cantPiezas][4][4];
extern int fuente8x8 [cantCaracteres][altoCaracter][anchoCaracter8];
extern int fuente8x16 [cantCaracteres][altoCaracter][anchoCaracter16];

// Fuente proporcional (no monoespaciada)
#define MAX_ANCHO_PROP 12
extern int fuenteProp[cantCaracteres][altoCaracter][MAX_ANCHO_PROP];
extern int anchoProp[cantCaracteres];
void GENERAR_FUENTE_PROPORCIONAL(void);

#endif // SPRITES_H_INCLUDED