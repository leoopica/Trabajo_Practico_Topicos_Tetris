#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#define cantPiezas 11
#define MAX_PIEZAS 11

#define cantCaracteres 38
#define altoCaracter 8
#define anchoCaracter8  8
#define anchoCaracter16 16


// Datos de las piezas (11 tipos, cada una 4×4)
extern int piezas [cantPiezas][4][4];

// Fuentes monoespaciadas: 8×8 (CGA) y 8×16 (VGA)
extern int fuente8x8 [cantCaracteres][altoCaracter][anchoCaracter8];
extern int fuente8x16 [cantCaracteres][altoCaracter][anchoCaracter16];

// Fuente proporcional (ancho variable)
#define MAX_ANCHO_PROP 12 // Ancho máximo de un carácter proporcional
extern int fuenteProp[cantCaracteres][altoCaracter][MAX_ANCHO_PROP];
extern int anchoProp[cantCaracteres];     // Ancho real de cada carácter


#endif // SPRITES_H_INCLUDED
