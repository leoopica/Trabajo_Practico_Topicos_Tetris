/*
PRUEBA DE TETRIS BÁSICO
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "gbt.h"

#define filas 20 // Cantidad de filas del tablero
#define columnas 10 // Cantidad de columnas del tablero
#define cantPiezas 7 // Cantidad de piezas
#define CANT_COLORES 16 // Cantidad de colores
#define ANCHO_VENTANA 128
#define ALTO_VENTANA 128
#define ESCALA_VENTANA 5
#define TAM_CELDA 6 // Cuántos pixeles tiene cada bloque del tablero
#define OFFSET_X 20 // Margen horizontal pantalla desde la izquierda
#define OFFSET_Y 10 // Margen vertical pantalla desde arriba


// Definición del tablero
int tablero [filas][columnas] = {0}; // Usa binario para detectar si el casillero está o no ocupado. Inicia todos en desocupado

// Definición de las formas de las piezas 
int piezas [cantPiezas][4][4] = // [Cantidad de piezas], [4 filas], [4 columnas]
{
    // Pieza I
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },
    
    // Pieza J
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },

    // Pieza L
    {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
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

// Definición de colores
tGBT_ColorRGB paletaCGA [CANT_COLORES] = {

    /// 0-15: Colores CGA (16 colores)
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

// Estructura para las piezas
typedef struct
{
    int forma [4][4]; // Forma de la pieza
    int fila, columna; // Coordenada de la pieza en el tablero
} sPieza;

sPieza actual; // Pieza actual cayendo

void NUEVAPIEZA (); // Elige una nueva pieza para que caiga en el tablero
void COPIARPIEZA (int destino [4][4], int origen [4][4]); // Copia la forma de la pieza de un lado a otro
void DIBUJAR(); // Dibuja los gráficos
int COLISION (int filaNueva, int columnaNueva, int forma [4][4]); // Evalúa la colisión de la pieza que está cayendo
void FIJARPIEZA (); // Fija la pieza en el tablero
void LIMPIARLINEAS (); // Limpia una fila si se completa la línea
void ROTARHORARIO ();
void ROTARANTIHORARIO ();

int main ()
{
    char nombreVentana[128]; // Vector para el nombre de la ventana
    
    if (gbt_iniciar() != 0) // Iniciar la biblioteca GBT
    {
        fprintf (stderr, "Error al iniciar GBT: %s\n", gbt_obtener_log()); // Si falla en el inicio, lo indica
        return -1;
    }

    sprintf (nombreVentana, "Tetris %dx%d", ANCHO_VENTANA, ALTO_VENTANA); // Pone el nombre Tetris en la ventana con las medidas de la misma

    if (gbt_crear_ventana (nombreVentana, ANCHO_VENTANA, ALTO_VENTANA, ESCALA_VENTANA) != 0) // Crea la ventana
    {
        fprintf(stderr, "Error al iniciar el modulo de graficos de GBT: %s\n", gbt_obtener_log()); // Si falla en la creación, lo indica
        return -1;
    }

    if (gbt_aplicar_paleta(paletaCGA, CANT_COLORES, GBT_FORMATO_888) != 0) // Aplica la paleta de colores definida
    {
        fprintf (stderr, "Error al aplicar la nueva paleta de colores: %s\n", gbt_obtener_log()); // Si falla la aplicación de la paleta, lo indica
        return -1;
    }

    eGBT_Tecla tecla; // Define la variable tecla
    srand (time (0)); // Para randomizar
    NUEVAPIEZA (); // Obtiene pieza nueva
    tGBT_Temporizador *timer_caida = gbt_temporizador_crear (0.3); // Temporizador para limitar la velocidad del juego
    if (!timer_caida)
    {
        fprintf (stderr, "Error al crear temporizador: %s\n", gbt_obtener_log()); // Si falla en la creación, lo indica
        return -1;
    }
    tGBT_Temporizador *timer_mov = gbt_temporizador_crear(0.1); // Temporizador para limitar la velocidad de movimiento
    if (!timer_mov)
    {
        fprintf (stderr, "Error al crear temporizador: %s\n", gbt_obtener_log()); // Si falla en la creación, lo indica
        return -1;
    }

    uint8_t corriendo = 1; // Define el estado corriendo

    while (corriendo) // Mientras esté funcionando
    {
        gbt_procesar_entrada (); // Procesa la entrada de teclas
        tecla = gbt_obtener_tecla_presionada ();
        if (gbt_tecla_sostenida (GBTK_ESCAPE)) // Verifica input de la tecla Escape
        {
            corriendo = 0;
            printf("Saliendo del juego\n");
        }
        else 
        {
            if (gbt_tecla_presionada (GBTK_d)) // Input tecla D para girar sentido horario
            {
                ROTARHORARIO ();
            }
            if (gbt_tecla_presionada (GBTK_a)) // Input tecla A para girar sentido antihorario
            {
                ROTARANTIHORARIO ();
            }
            if (gbt_temporizador_consumir(timer_mov))
            {
                if (gbt_tecla_sostenida (GBTK_IZQUIERDA) && !COLISION (actual.fila, actual.columna - 1, actual.forma)) // Verifica input de flecha izquierda y colisión
                {
                    actual.columna --;
                }
                if (gbt_tecla_sostenida (GBTK_DERECHA) && !COLISION (actual.fila, actual.columna + 1, actual.forma)) // Verifica input de flecha derecha y colisión
                {
                    actual.columna ++;
                }
                if (gbt_tecla_sostenida (GBTK_ABAJO) && !COLISION (actual.fila + 1, actual.columna, actual.forma)) // Verifica input de flecha abajo y colisión
                {
                    actual.fila ++;
                }

                if (gbt_temporizador_consumir (timer_caida))
                {
                    if (!COLISION (actual.fila + 1, actual.columna, actual.forma)) // Verifica colisión para caída automática
                    {
                        actual.fila ++; // Baja la pieza una fila si no hubo colisión
                    }
                    else
                    {
                        FIJARPIEZA (); // Si hubo colisión, fija la pieza
                        LIMPIARLINEAS (); // Si se completó una fila, la limpia
                        NUEVAPIEZA (); // Obtiene nueva pieza
                    }
                }
            }
            DIBUJAR ();
            gbt_volcar_backbuffer();
            gbt_esperar(16);
        }
    }
    gbt_destruir_ventana();
    gbt_temporizador_destruir(timer_caida);
    gbt_temporizador_destruir(timer_mov);
    gbt_cerrar();
    return 0;
}

void NUEVAPIEZA ()
{
    int tipo;
    tipo = rand () % cantPiezas; // Randomiza la pieza
    COPIARPIEZA (actual.forma, piezas [tipo]); // Busca la pieza en el catálogo de acuerdo al número randomizado en TIPO, y copia la forma de dicha pieza en la pieza actual
    actual.fila = 0; // Ubica la pieza en la fila 0 (arriba de todo)
    actual.columna = columnas / 2 - 2; // Ubica la pieza en la mitad horizontal del tablero
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
    int fTablero, cTablero, fPieza, cPieza, posXPantalla, posYPantalla, pixelXBloque, pixelYBloque, ocupado;

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
                            ocupado = 1;
                        }
                    }
                }
            }

            if (ocupado == 1) // Si está ocupado, lo dibuja
            {
                // Dibujar bloque
                posXPantalla = OFFSET_X + cTablero * TAM_CELDA;
                posYPantalla = OFFSET_Y + fTablero * TAM_CELDA;

                for (pixelYBloque = 0; pixelYBloque < TAM_CELDA; pixelYBloque ++)
                {
                    for (pixelXBloque = 0; pixelXBloque < TAM_CELDA; pixelXBloque ++)
                    {
                        gbt_dibujar_pixel(posXPantalla + pixelXBloque, posYPantalla + pixelYBloque, 10); // color verde
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
                if (tablero [fTablero][cTablero] == 1) // Evalua colisión dentro del tablero, o sea, si hay un bloque ocupando dicho espacio
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
                tablero [actual.fila + filaPieza][actual.columna + columnaPieza] = 1; // Si hay un mino, lo dibuja en el tablero
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
