#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#define cantPiezas 11
#define MAX_PIEZAS 11

#define cantCaracteres 38
#define altoCaracter 8
#define anchoCaracter8  8
#define anchoCaracter16 16

extern int piezas [cantPiezas] [4] [4];

extern int fuente8x8 [cantCaracteres] [altoCaracter] [anchoCaracter8];
extern int fuente8x16 [cantCaracteres] [altoCaracter] [anchoCaracter16];

#define MAX_ANCHO_PROP 12
extern int fuenteProp [cantCaracteres] [altoCaracter] [MAX_ANCHO_PROP];
extern int anchoProp [cantCaracteres];
void GENERAR_FUENTE_PROPORCIONAL ();

#endif // SPRITES_H_INCLUDED
