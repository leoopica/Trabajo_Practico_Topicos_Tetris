#include "graficos.h"
#include "sprites.c"
#include <stdlib.h>
#include <time.h>

// Definición del tablero
int tablero [filas][columnas] = {0};

// Paleta de colores
tGBT_ColorRGB paletaCGA [CANT_COLORES] =
{
    {0x00, 0x00, 0x00}, // 0:   Negro
    {0x00, 0x00, 0xAA}, // 1:   Azul
    {0x00, 0xAA, 0x00}, // 2:   Verde
    {0x00, 0xAA, 0xAA}, // 3:   Cian
    {0xAA, 0x00, 0x00}, // 4:   Rojo
    {0xAA, 0x00, 0xAA}, // 5:   Magenta
    {0xAA, 0x55, 0x00}, // 6:   Marron
    {0xAA, 0xAA, 0xAA}, // 7:   Gris claro
    {0x55, 0x55, 0x55}, // 8:   Gris oscuro
    {0x55, 0x55, 0xFF}, // 9:   Azul brillante
    {0x55, 0xFF, 0x55}, // 10:  Verde brillante
    {0x55, 0xFF, 0xFF}, // 11:  Cian brillante
    {0xFF, 0x55, 0x55}, // 12:  Rojo brillante
    {0xFF, 0x55, 0xFF}, // 13:  Magenta brillante
    {0xFF, 0xFF, 0x55}, // 14:  Amarillo
    {0xFF, 0xFF, 0xFF}  // 15:  Usado como transparente por GBT
};

int colorBrillo [CANT_COLORES] = {0,9,10,11,12,13,15,15,7,15,15,15,15,15,15,15}; // Paleta de colores para la parte de brillo
int colorSombra [CANT_COLORES] = {0,1,2,3,4,5,4,8,0,1,2,3,4,5,6,7}; // Paleta de colores para la parte de sombra

sPieza actual;

void NUEVAPIEZA ()
{
    int tipo; // Pieza que va a salir
    int colorPiezas [cantPiezas] = {11, 9, 6, 14, 10, 13, 12}; // Asigna un color a cada pieza
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
    int fTablero, cTablero, fPieza, cPieza; // Filas y columnas de tablero y de matriz de pieza
    int posXPantalla, posYPantalla; // Posición horizontal y vertical
    int pixelXBloque, pixelYBloque; // Pixel horizontal y vertical de la pieza
    int ocupado = 0, colorBase, colorFinal; // Ubicación ocupada, color original del bloque, color que va a dibujarse en el pixel

    gbt_borrar_backbuffer (0); // Limpia pantalla (negro)

    DIBUJARFONDO ();
    DIBUJARMARCO ();
    DIBUJARGRILLA ();

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
                            ocupado = actual.color; // Indica que está ocupado por la pieza
                        }
                    }
                }
            }

            if (ocupado != 0) // Si está ocupado, lo dibuja
            {
                colorBase = ocupado; // Guarda el color base de la pieza a dibujar
                // Dibujar bloque
                posXPantalla = OFFSET_X + cTablero * TAM_CELDA -1; // Convierte coordenadas horizontales del tablero en coordenadas en pantalla
                posYPantalla = OFFSET_Y + fTablero * TAM_CELDA -1; // Convierte coordenadas verticales del tablero en coordenadas en pantalla

                for (pixelYBloque = 0; pixelYBloque <= TAM_CELDA; pixelYBloque ++) // Recorre filas de píxeles dentro del bloque
                {
                    for (pixelXBloque = 0; pixelXBloque <= TAM_CELDA; pixelXBloque ++) // Recorre columnas de píxeles dentro del bloque
                    {
                        colorFinal = colorBase; // Pone el pixel en el color de la base
                        
                        if (pixelYBloque <= 1 || pixelXBloque <= 1) // Verifica la posición del pixel para ver si está en la esquina superior izquierda
                        {
                            colorFinal = colorBrillo [colorBase]; // Pone brillo en la esquina superior izquierda (borde de 2 píxeles)
                        }
                        else 
                        {
                            if (pixelYBloque >= TAM_CELDA - 2 || pixelXBloque >= TAM_CELDA - 2) // Verifica la posición del pixel para ver si está en la esquina inferior derecha
                            {
                                colorFinal = colorSombra [colorBase]; // Pone sombra en esquina inferior derecha (borde de 2 píxeles)
                            }
                        }

                        gbt_dibujar_pixel(posXPantalla + pixelXBloque, posYPantalla + pixelYBloque, colorFinal); // Dibuja el pixel en cuestión en la posición correspondiente
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

void DIBUJARFONDO ()
{
   int x, y; // Horizontal, vertical
   int color; // Variable color

    for (y = 0; y < ALTO_VENTANA; y ++) // Recorre verticalmente la ventana
    {
        for (x = 0; x < ANCHO_VENTANA; x ++) // Recorre horizontalmente la ventana
        {
            color = 1; // Define el color del fondo como azul oscuro

            if ((x % 16 == 0) || (y % 16 == 0)) // Genera la cuadrícula en color gris
            {
                color = 8;
            }
            gbt_dibujar_pixel(x, y, color); // Dibuja
        }
    } 
}

void DIBUJARMARCO ()
{
    int x, y; // Horizontal, vertical
    int x0, y0; // Esquina superior izquierda del marco
    int ancho, alto; // Ancho y alto del marco
    x0 = OFFSET_X - 4; // Agranda el marco horizontalmente hacia afuera del tablero
    y0 = OFFSET_Y - 4; // Agranda el marco verticalmente hacia afuera del tablero
    ancho = columnas * TAM_CELDA + 8; // Calcula el ancho del marco en pixeles de acuerdo a la cantidad de columnas del tablero. Agrega 8 por el borde
    alto  = filas * TAM_CELDA + 8; // Calcula el alto del marco en pixeles de acuerdo a la cantidad de columnas del tablero. Agrega 8 por el borde
    int color;
    for (y = y0; y < y0 + alto; y ++) // Recorre horizontalmente el marco
    {
        for (x = x0; x < x0 + ancho; x ++) // Recorre verticalmente el marco
        {
            color = 0; // Fondo gris

            if (y <= y0 + 1 || x <= x0 + 1) // Evalúa si el pixel en cuestión está arriba o a la izquierda para ponerle brillo al marco
                {
                    color = 7; // Brillo
                }

            if (y >= y0 + alto - 2 || x >= x0 + ancho - 2) // Evalúa si el pixel en cuestión está abajo o a la derecha para ponerle sombra al marco
                {
                    color = 8; // Sombra
                }

            gbt_dibujar_pixel(x, y, color); // Dibuja el pixel
        }
    }
}

void DIBUJARGRILLA ()
{
    int fila, columna;
    int x, y;

    for (fila = 0; fila <= filas; fila ++) // Líneas horizontales
    {
        y = OFFSET_Y + fila * TAM_CELDA;

        for (x = OFFSET_X; x <= OFFSET_X + columnas * TAM_CELDA; x ++)
        {
            gbt_dibujar_pixel(x, y, 8); // gris oscuro
        }
    }

    for (columna = 0; columna <= columnas; columna ++) // Líneas verticales
    {
        x = OFFSET_X + columna * TAM_CELDA;

        for (y = OFFSET_Y; y <= OFFSET_Y + filas * TAM_CELDA; y ++)
        {
            gbt_dibujar_pixel(x, y, 8); // gris oscuro
        }
    }
}

