#include "graficos.h"
#include <stdlib.h>
#include <time.h>

// Definición del tablero
int tablero [filas][columnas] = {0};

// Definición de las formas de las piezas 
int piezas [cantPiezas][4][4] =
{
    // Pieza I
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },
    
    // Pieza L
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },

    // Pieza J
    {
        {0, 1, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },

    // Pieza O
    {
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },

    // Pieza S
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },

    // Pieza T
    {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },

    // Pieza Z
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    }
};

// Paleta
tGBT_ColorRGB paletaCGA [CANT_COLORES] =
{
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0xAA},
    {0x00, 0xAA, 0x00},
    {0x00, 0xAA, 0xAA},
    {0xAA, 0x00, 0x00},
    {0xAA, 0x00, 0xAA},
    {0xAA, 0x55, 0x00},
    {0xAA, 0xAA, 0xAA},
    {0x55, 0x55, 0x55},
    {0x55, 0x55, 0xFF},
    {0x55, 0xFF, 0x55},
    {0x55, 0xFF, 0xFF},
    {0xFF, 0x55, 0x55},
    {0xFF, 0x55, 0xFF},
    {0xFF, 0xFF, 0x55},
    {0xFF, 0xFF, 0xFF}
};

int colorBrillo [CANT_COLORES] = {0,9,10,11,12,13,15,15,7,15,15,15,15,15,15,15};
int colorSombra [CANT_COLORES] = {0,1,2,3,4,5,4,8,0,1,2,3,4,5,6,7};

sPieza actual;

void NUEVAPIEZA ()
{
    int tipo;
    int colorPiezas [cantPiezas] = {11, 9, 6, 14, 10, 13, 12};
    tipo = rand () % cantPiezas; // Randomiza la pieza
    COPIARPIEZA (actual.forma, piezas [tipo]); // Busca la pieza en el catálogo de acuerdo al número randomizado en TIPO, y copia la forma de dicha pieza en la pieza actual
    actual.fila = 0; // Ubica la pieza en la fila 0 (arriba de todo)
    actual.columna = columnas / 2 - 2; // Ubica la pieza en la mitad horizontal del tablero
    actual.color = colorPiezas [tipo]; // Pone el color
}

void COPIARPIEZA (int destino [4][4], int origen [4][4])
{
    int f, c;
    for (f = 0; f < 4; f ++) // Recorre filas de la matriz de la forma
    {
        for (c = 0; c < 4; c ++) // Recorre columnas de la matriz de la forma
        {
            destino [f][c] = origen [f][c]; // Copia el pixel de la posición de origen en el destino
        }
    }
}

void DIBUJAR ()
{
    int fTablero, cTablero, fPieza, cPieza, posXPantalla, posYPantalla, pixelXBloque, pixelYBloque, ocupado = 0, colorBase, colorFinal;

    gbt_borrar_backbuffer (0); // Limpia pantalla (negro)

    for (fTablero = 0; fTablero < filas; fTablero ++) // Recorre filas del tablero
    {
        for (cTablero = 0; cTablero < columnas; cTablero ++) // Recorre columnas del tablero
        {
            ocupado = tablero [fTablero][cTablero]; // Verifica si ya hay un mino en esa posición
            
            for (fPieza = 0; fPieza < 4; fPieza ++) // Recorre filas de matriz de la pieza
            {
                for (cPieza = 0; cPieza < 4; cPieza ++) // Recorre columnas de matriz de la pieza
                {
                    if (actual.forma [fPieza][cPieza] == 1) // Verifica si la matriz de la pieza tiene un mino en esa posición
                    {
                        if (actual.fila + fPieza == fTablero && actual.columna + cPieza == cTablero) // Verifica si el mino en cuestión está en cierta posición del tablero
                        {
                            ocupado = actual.color;
                        }
                    }
                }
            }

            if (ocupado != 0) // Si está ocupado, lo dibuja
            {
                colorBase = ocupado;
                // Dibujar bloque
                posXPantalla = OFFSET_X + cTablero * TAM_CELDA;
                posYPantalla = OFFSET_Y + fTablero * TAM_CELDA;

                for (pixelYBloque = 0; pixelYBloque < TAM_CELDA; pixelYBloque ++)
                {
                    for (pixelXBloque = 0; pixelXBloque < TAM_CELDA; pixelXBloque ++)
                    {
                        colorFinal = colorBase;

                        // Pone brillo en la esquina superior izquierda (borde de 2 píxeles)
                        if (pixelYBloque <= 1 || pixelXBloque <= 1)
                        {
                            colorFinal = colorBrillo[colorBase];
                        }
                        else 
                        {
                            // Pone sombra en esquina inferior derecha (borde de 2 píxeles)
                            if (pixelYBloque >= TAM_CELDA - 2 || pixelXBloque >= TAM_CELDA - 2)
                            {
                                colorFinal = colorSombra[colorBase];
                            }
                        }

                        gbt_dibujar_pixel(posXPantalla + pixelXBloque, posYPantalla + pixelYBloque, colorFinal);
                    }
                }
            }
        }
    }
}

int COLISION (int filaNueva, int columnaNueva, int forma [4][4])
{
    int filaPieza, columnaPieza, fTablero, cTablero, colisiona = 0;
    for (filaPieza = 0; filaPieza < 4; filaPieza ++) // Recorre matriz de la pieza por filas
    {
        for (columnaPieza = 0; columnaPieza < 4; columnaPieza ++) // Recorre matriz de la pieza por columnas
        {
            if (forma [filaPieza][columnaPieza] == 1) // Verifica la matriz de la pieza para ver si tiene o no ocupado (para poder dibujarla)
            {
                fTablero = filaNueva + filaPieza; // Pone la pieza en el tablero
                cTablero = columnaNueva + columnaPieza; // Pone la pieza en el tablero
                if (fTablero >= filas || cTablero < 0 || cTablero >= columnas) // Evalua colisión con los bordes del tablero
                {
                    colisiona = 1;
                    return colisiona;
                }
                if (tablero [fTablero][cTablero]  != 0) // Evalua colisión dentro del tablero, o sea, si hay un bloque ocupando dicho espacio
                {
                    colisiona = 1;
                    return colisiona;
                }
            }
        }
    }
    return colisiona;
}

void FIJARPIEZA ()
{
    int filaPieza, columnaPieza;
    for (filaPieza = 0; filaPieza < 4; filaPieza ++) // Recorre matriz de la pieza por filas
    {
        for (columnaPieza = 0; columnaPieza < 4; columnaPieza ++) // Recorre matriz de la pieza por columnas
        {
            if (actual.forma [filaPieza][columnaPieza] == 1) // Verifica si en dicha posición de la matriz de la pieza hay o no un mino
            {
                tablero [actual.fila + filaPieza][actual.columna + columnaPieza] = actual.color; // Si hay un mino, lo dibuja en el tablero
            }
        }
    }
}

void LIMPIARLINEAS ()
{
    int fila, columna, filaAux, llena;

    for (fila = filas - 1; fila >= 0; fila --) // Recorre las filas del tablero, desde abajo hasta arriba
    {
        llena = 1;

        for (columna = 0; columna < columnas; columna ++) // Recorre las columnas del tablero
        {
            if (tablero[fila][columna] == 0) // Evalúa si está ocupado el espacio en el tablero. Si encuentra un vacío, la fila no está llena
            {
                llena = 0;
            }
        }

        if (llena == 1) // Evalúa si llena quedó en 1. Si quedó en 1, significa que toda la fila está llena
        {
            for (filaAux = fila; filaAux > 0; filaAux --) // Recorre desde la fila llena hacia arriba
            {
                for (columna = 0; columna < columnas; columna ++) // Recorre las columnas
                {
                    tablero[filaAux][columna] = tablero[filaAux - 1][columna]; // Copia el contenido de la fila de arriba en la fila actual
                }
            }

            for (columna = 0; columna < columnas; columna ++)
            {
                tablero[0][columna] = 0; // Borra la fila superior
            }

            fila ++; // Vuelve a evaluar la misma fila, porque bajó una nueva
        }
    }
}

void ROTARHORARIO ()
{
    int temporal [4][4]; // Matriz temporal de pieza
    int i, j;
    for (i = 0; i < 4; i ++) // Recorre las filas de la matriz de la pieza
    {
        for (j = 0; j < 4; j ++) // Recorre las columnas de la matriz de la pieza
        {
            temporal [j][3 - i] = actual.forma [i][j]; // Rota la pieza 90º en sentido horario, y lo guarda en el temporal
        }
    }
    if (COLISION (actual.fila, actual.columna, temporal) == 0) // Verifica si hay colisión al rotar la pieza
    {
        COPIARPIEZA (actual.forma, temporal); // Si no hay rotación, copia la forma de la pieza rotada a la actual
    }
}

void ROTARANTIHORARIO ()
{
    int temporal [4][4]; // Matriz temporal de pieza
    int i, j;
    for (i = 0; i < 4; i ++) // Recorre las filas de la matriz de la pieza
    {
        for (j = 0; j < 4; j ++) // Recorre las columnas de la matriz de la pieza
        {
            temporal [3 - j][i] = actual.forma [i][j]; // Rota la pieza 90º en sentido horario, y lo guarda en el temporal
        }
    }
    if (COLISION (actual.fila, actual.columna, temporal) == 0) // Verifica si hay colisión al rotar la pieza
    {
        COPIARPIEZA (actual.forma, temporal); // Si no hay rotación, copia la forma de la pieza rotada a la actual
    }
}
