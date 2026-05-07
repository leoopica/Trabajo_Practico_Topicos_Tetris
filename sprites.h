#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <stdio.h>
#include "gbt.h"

#define cantPiezas 7 // Define la cantidad de piezas
#define tamMino 8 // Define tamaño de cada mino en pixeles
#define cantCaracteres 38 // 26 letras, 10 números, 1 vacío, 1 guion bajo
#define altoCaracter 8
#define anchoCaracter8 8
#define anchoCaracter16 16

extern int piezas [cantPiezas][4][4];
extern int fuente8x8 [cantCaracteres][altoCaracter][anchoCaracter8];
extern int fuente8x16 [cantCaracteres][altoCaracter][anchoCaracter16];

#endif // SPRITES_H_INCLUDED
