#include "graficos.h"
#include "sprites.h"
#include "inicio.h"
#include <stdlib.h>
#include <string.h>

// Paleta de colores
tGBT_ColorRGB paletaCGA [cantColores] =
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

int colorBrillo [cantColores] = {0,9,10,11,12,13,15,15,7,15,15,15,15,15,15,15}; // Paleta de colores para la parte de brillo
int colorSombra [cantColores] = {0,1,2,3,4,5,4,8,0,1,2,3,4,5,6,7}; // Paleta de colores para la parte de sombra

void DIBUJAR ()
{
    int fTablero, cTablero, fPieza, cPieza; // filasTablero y columnasTablero de tablero y de matriz de pieza
    int posXPantalla, posYPantalla; // Posición horizontal y vertical
    int pixelXBloque, pixelYBloque; // Pixel horizontal y vertical de la pieza
    int ocupado = 0, colorBase, colorFinal; // Ubicación ocupada, color original del bloque, color que va a dibujarse en el pixel

    gbt_borrar_backbuffer (0); // Limpia pantalla (negro)

    DIBUJARFONDO ();
    DIBUJARMARCO ();
    DIBUJARGRILLA ();
    DIBUJARPUNTAJE ();
    DIBUJARPROXIMA ();
    DIBUJARTEXTO(offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20 - 10, "PLAYER ", anchoCaracter8); // MODIFICAR PARA QUE SEA MÁS SIMPLE?
    DIBUJARTEXTO(offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20, nombreJugador, anchoCaracter8); // MODIFICAR PARA QUE SEA MÁS SIMPLE?

    for (fTablero = 0; fTablero < filasTablero; fTablero ++) // Recorre filasTablero del tablero
    {
        for (cTablero = 0; cTablero < columnasTablero; cTablero ++) // Recorre columnasTablero del tablero
        {
            ocupado = tablero [fTablero][cTablero]; // Verifica si ya hay un mino en esa posición
            
            for (fPieza = 0; fPieza < 4; fPieza ++) // Recorre filasTablero de matriz de la pieza
            {
                for (cPieza = 0; cPieza < 4; cPieza ++) // Recorre columnasTablero de matriz de la pieza
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
                posXPantalla = offsetHorizontal + cTablero * tamMino -1; // Convierte coordenadas horizontales del tablero en coordenadas en pantalla
                posYPantalla = offsetVertical + fTablero * tamMino -1; // Convierte coordenadas verticales del tablero en coordenadas en pantalla

                for (pixelYBloque = 0; pixelYBloque <= tamMino; pixelYBloque ++) // Recorre filasTablero de píxeles dentro del bloque
                {
                    for (pixelXBloque = 0; pixelXBloque <= tamMino; pixelXBloque ++) // Recorre columnasTablero de píxeles dentro del bloque
                    {
                        colorFinal = colorBase; // Pone el pixel en el color de la base
                        
                        if (pixelYBloque <= 1 || pixelXBloque <= 1) // Verifica la posición del pixel para ver si está en la esquina superior izquierda
                        {
                            colorFinal = colorBrillo [colorBase]; // Pone brillo en la esquina superior izquierda (borde de 2 píxeles)
                        }
                        else 
                        {
                            if (pixelYBloque >= tamMino - 2 || pixelXBloque >= tamMino - 2) // Verifica la posición del pixel para ver si está en la esquina inferior derecha
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


void DIBUJARFONDO ()
{
   int x, y; // Horizontal, vertical
   int color; // Variable color

    for (y = 0; y < altoVentana; y ++) // Recorre verticalmente la ventana
    {
        for (x = 0; x < anchoVentana; x ++) // Recorre horizontalmente la ventana
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
    int x0 = offsetHorizontal - 4;                // Esquina superior izquierda
    int y0 = offsetVertical - 4;
    int ancho = columnasTablero * tamMino + 8;    // Ancho del marco
    int alto  = filasTablero * tamMino + 8;       // Alto del marco

    // Reutiliza la función genérica (fondo color 0 = negro / "transparente" sobre el fondo del tablero)
    DIBUJARMARCOGENERICO(x0, y0, ancho, alto, 0);
}

void DIBUJARMARCOGENERICO (int x0, int y0, int ancho, int alto, int colorFondo)
{
    int x, y;
    int color;

    for (y = y0; y < y0 + alto; y ++)            // Recorre verticalmente el marco
    {
        for (x = x0; x < x0 + ancho; x ++)       // Recorre horizontalmente el marco
        {
            color = colorFondo;                  // Color de fondo de la caja

            if (y <= y0 + 1 || x <= x0 + 1)      // Borde superior o izquierdo -> brillo
            {
                color = 7;                       // Gris claro
            }

            if (y >= y0 + alto - 2 || x >= x0 + ancho - 2) // Borde inferior o derecho -> sombra
            {
                color = 8;                       // Gris oscuro
            }

            gbt_dibujar_pixel(x, y, color);
        }
    }
}

void DIBUJARGRILLA ()
{
    int fila, columna;
    int x, y;

    for (fila = 0; fila <= filasTablero; fila ++) // Líneas horizontales
    {
        y = offsetVertical + fila * tamMino;

        for (x = offsetHorizontal; x <= offsetHorizontal + columnasTablero * tamMino; x ++)
        {
            gbt_dibujar_pixel(x, y, 8); // gris oscuro
        }
    }

    for (columna = 0; columna <= columnasTablero; columna ++) // Líneas verticales
    {
        x = offsetHorizontal + columna * tamMino;

        for (y = offsetVertical; y <= offsetVertical + filasTablero * tamMino; y ++)
        {
            gbt_dibujar_pixel(x, y, 8); // gris oscuro
        }
    }
}

void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color) // PONER COMENTARIOS
{
    int filaCaracter, columnaCaracter;
    for (filaCaracter = 0; filaCaracter < altoCaracter; filaCaracter ++)
    {
        for (columnaCaracter = 0; columnaCaracter < anchoCaracter; columnaCaracter ++)
        {
            if (anchoCaracter == anchoCaracter8)
            {
                if (fuente8x8 [caracter][filaCaracter][columnaCaracter] == 1)
                {
                    gbt_dibujar_pixel (posXPantalla + columnaCaracter, posYPantalla + filaCaracter, color);
                }
            }
            if (anchoCaracter == anchoCaracter16)
            {
                if (fuente8x16 [caracter][filaCaracter][columnaCaracter] == 1)
                {
                    gbt_dibujar_pixel (posXPantalla + columnaCaracter, posYPantalla + filaCaracter, color);
                }
            }
        }
    }
}

void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter) // PONER COMENTARIOS
{
    int i = 0, caracter;
    while (texto [i] != '\0')
    {
        caracter = -1;
        if (texto [i] >= 'A' && texto [i] <= 'Z')
        {
            caracter = texto [i] - 'A';
        }
        else if (texto [i] >= '0' && texto [i] <= '9')
        {
            caracter = 26 + (texto [i] - '0');
        }
        else if (texto [i] == ' ')
        {
            caracter = 36;
        }
        if (caracter != -1) DIBUJARCARACTER (posXPantalla + i * anchoCaracter, posYPantalla, caracter, anchoCaracter, 7);
        i++ ;
    }
}

void DIBUJARPROXIMA ()
{
    int f, c, px, py, x0, y0;
    int colorBase, colorFinal, pixelX, pixelY;

    x0 = offsetHorizontal - 80;
    y0 = offsetVertical;

    DIBUJARTEXTO(x0, y0, "NEXT", anchoCaracter8);

    // Dibujar miniatura de la próxima pieza
    for (f = 0; f < 4; f++)
    {
        for (c = 0; c < 4; c++)
        {
            if (proxima.forma[f][c] == 1)
            {
                colorBase = proxima.color;
                px = x0 + c * tamMino;
                py = y0 + 15 + f * tamMino;

                for (pixelY = 0; pixelY <= tamMino; pixelY++)
                {
                    for (pixelX = 0; pixelX <= tamMino; pixelX++)
                    {
                        colorFinal = colorBase;
                        if (pixelY <= 1 || pixelX <= 1) colorFinal = colorBrillo[colorBase];
                        else if (pixelY >= tamMino - 2 || pixelX >= tamMino - 2) colorFinal = colorSombra[colorBase];
                        gbt_dibujar_pixel(px + pixelX, py + pixelY, colorFinal);
                    }
                }
            }
        }
    }
}

void DIBUJARPUNTAJE () // PONER COMENTARIOS
{
    char textoPuntaje [20];
    sprintf (textoPuntaje, "SCORE %d", puntaje);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "LEVEL %d", nivel);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 20, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "LINES %d", lineas_totales);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 40, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "SPEED %d", velocidad);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 60, textoPuntaje, anchoCaracter8);

}

void DIBUJARTITULO () // PONER COMENTARIOS
{
    char titulo [] = "TETRIS";
    int colores [] = {12, 14, 10, 11, 13, 9};
    int i, caracter;
    for (i = 0; i < 6; i ++) // 6 = cantidad letras TETRIS
    {
        if (titulo [i] >= 'A' && titulo [i] <= 'Z')
        {
            caracter = titulo [i] - 'A';
        }
        DIBUJARCARACTER (100 + i * anchoCaracter16, 50, caracter, anchoCaracter16, colores [i]);
    } 
}

void DIBUJARPAUSA ()
{
    int anchoCaja = 200;
    int altoCaja  = 50;
    int centroX = anchoVentana / 2;
    int centroY = altoVentana  / 2;
    int x0 = centroX - anchoCaja / 2;
    int y0 = centroY - altoCaja  / 2;

    DIBUJARMARCOGENERICO(x0, y0, anchoCaja, altoCaja, 0);

    char *linea1 = "PAUSA";
    char *linea2 = "PRESIONE P PARA VOLVER";

    DIBUJARTEXTO(centroX - ((int)strlen(linea1) * anchoCaracter8) / 2, y0 + 10, linea1, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea2) * anchoCaracter8) / 2, y0 + 30, linea2, anchoCaracter8);
}

void DIBUJARGAMEOVER ()
{
    // Tamaño de la caja negra (modificá estos valores para cambiar el tamaño)
    int anchoCaja = 160;
    int altoCaja  = 160;

    // Centro de la ventana
    int centroX = anchoVentana / 2;
    int centroY = altoVentana  / 2;

    // Esquina superior izquierda de la caja
    int x0 = centroX - anchoCaja / 2;
    int y0 = centroY - altoCaja  / 2;

    // Esquina superior derecha de la caja
    int xF = centroX + anchoCaja / 2;
    int yF = centroY + altoCaja  / 2;

    // Dibuja el marco con fondo negro
    DIBUJARMARCOGENERICO(x0, y0, anchoCaja, altoCaja, 0);

    // Helper local: calcula la X centrada según la longitud del texto
    // (lo dejo inline con strlen para mayor claridad)

    char puntajeFinal[32];
    sprintf(puntajeFinal, "SCORE %d", puntaje);

    char *linea1 = "GAME OVER";
    char *linea2 = puntajeFinal;
    char *linea3 = "R PARA REINICIAR";
    char *linea4 = "ESC PARA SALIR";

    // Posiciones Y separadas uniformemente dentro de la caja
    int y1 = y0 + 10;
    int y2 = y0 + 30;
    int y3 = yF - 30;
    int y4 = yF - 15;

    DIBUJARTEXTO(centroX - ((int)strlen(linea1) * anchoCaracter8) / 2, y1, linea1, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea2) * anchoCaracter8) / 2, y2, linea2, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea3) * anchoCaracter8) / 2, y3, linea3, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea4) * anchoCaracter8) / 2, y4, linea4, anchoCaracter8);
}

void DIBUJARINICIO(char *nombre) // PONER COMENTARIOS
{
    int i = 0;
    int terminado = 0;
    eGBT_Tecla tecla;

    int colores[6] = {12, 9, 10, 14, 13, 11}; // colores para TETRIS
    char titulo[] = "TETRIS";

    nombre[0] = '\0';

    while (!terminado)
    {
        gbt_procesar_entrada();
        tecla = gbt_obtener_tecla_presionada();

        if (tecla != GBTK_DESCONOCIDA)
        {
            if (tecla == GBTK_ENTER)
            {
                if (i > 0)
                {
                    terminado = 1;
                }
            }
            else if (tecla == GBTK_RETROCESO)
            {
                if (i > 0)
                {
                    i--;
                    nombre[i] = '\0';
                }
            }
            else if (tecla >= 'a' && tecla <= 'z')
            {
                if (i < 21 - 1)
                {
                    nombre[i] = tecla - 32; // mayúscula
                    i++;
                    nombre[i] = '\0';
                }
            }
            else if (tecla >= '0' && tecla <= '9')
            {
                if (i < 21 - 1)
                {
                    nombre[i] = tecla;
                    i++;
                    nombre[i] = '\0';
                }
            }
            else if (tecla == GBTK_ESPACIO)
            {
                if (i < 21 - 1 && i > 0)
                {
                    nombre[i] = ' ';
                    i++;
                    nombre[i] = '\0';
                }
            }
        }

        // --- DIBUJADO ---
        gbt_borrar_backbuffer(0);
        DIBUJARFONDO();

        // // 🔥 TETRIS
        DIBUJAR_LOGO_COMPLETO(69, 15);

        // Texto
        DIBUJARTEXTO(80, 140, "INGRESE SU NOMBRE:", anchoCaracter8);

        // Nombre
        DIBUJARTEXTO(80, 160, nombre, anchoCaracter8);

        // Cursor
        if (i < 21 - 1)
        {
            DIBUJARCARACTER(
                80 + i * anchoCaracter8,
                160,
                37, // índice del carácter "_"
                anchoCaracter8,
                7
            );
        }
        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}