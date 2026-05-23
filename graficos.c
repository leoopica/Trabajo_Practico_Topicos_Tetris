#include "graficos.h"
#include "sprites.h"
#include "inicio.h"
#include <stdlib.h>
#include <string.h>
#include "funcionalidades.h"

// Paleta de colores CGA (16 colores)
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

// Para cada color base, el índice de color a usar en la zona de brillo (esquina sup-izq)
int colorBrillo [cantColores] = {0,9,10,11,12,13,15,15,7,15,15,15,15,15,15,15};
// Para cada color base, el índice de color a usar en la zona de sombra (esquina inf-der)
int colorSombra [cantColores] = {0,1,2,3,4,5,4,8,0,1,2,3,4,5,6,7};

extern int pieza_fijada_sin_nueva;

// Dibuja el tablero completo, la pieza actual, el fondo, marco, grilla, puntajes, próxima pieza, cheat y nombre del jugador
void DIBUJAR ()
{
    int fTablero, cTablero, fPieza, cPieza;
    int posXPantalla, posYPantalla;
    int pixelXBloque, pixelYBloque;
    int ocupado = 0, colorBase, colorFinal;

    gbt_borrar_backbuffer (0); // Limpia toda la pantalla con color 0 (negro)

    // Capas de dibujo en orden
    DIBUJARFONDO ();
    DIBUJARMARCO ();
    DIBUJARGRILLA ();
    DIBUJARPUNTAJE ();
    DIBUJARPROXIMA ();
    DIBUJARCHEAT ();
    DIBUJARTEXTO(offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20 - 10, "PLAYER ", anchoCaracter8);
    DIBUJARTEXTO(offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20, nombreJugador, anchoCaracter8);

    // Recorre SOLO las filas VISIBLES (salta FILAS_INVISIBLES filas arriba)
    for (fTablero = FILAS_INVISIBLES; fTablero < FILAS_TOTALES; fTablero ++)
    {
        for (cTablero = 0; cTablero < columnasTablero; cTablero ++)
        {
            ocupado = tablero [fTablero][cTablero];

            // Sobre-escribe con la pieza actual si está en esta celda
            if (!pieza_fijada_sin_nueva)
            {
                for (fPieza = 0; fPieza < 4; fPieza ++)
                {
                    for (cPieza = 0; cPieza < 4; cPieza ++)
                    {
                        if (actual.forma [fPieza][cPieza] == 1)
                        {
                            int colPiezaMundo = actual.columna + cPieza;
                            if (config_actual.modo_juego == MODO_DX)
                            {
                                if (colPiezaMundo < 0) colPiezaMundo += columnasTablero;
                                if (colPiezaMundo >= columnasTablero) colPiezaMundo -= columnasTablero;
                            }
                            if (actual.fila + fPieza == fTablero && colPiezaMundo == cTablero)
                                ocupado = actual.color;
                        }
                    }
                }
            }

            // Dibuja el bloque si la celda está ocupada
            if (ocupado != 0)
            {
                colorBase = ocupado;
                posXPantalla = offsetHorizontal + cTablero * tamMino - 1;
                // Desplaza la Y para ignorar las filas invisibles (fTablero parte de FILAS_INVISIBLES)
                posYPantalla = offsetVertical + (fTablero - FILAS_INVISIBLES) * tamMino - 1;

                // Dibuja cada píxel del bloque (tamMino+1 × tamMino+1) con efecto 3D
                for (pixelYBloque = 0; pixelYBloque <= tamMino; pixelYBloque ++)
                {
                    for (pixelXBloque = 0; pixelXBloque <= tamMino; pixelXBloque ++)
                    {
                        colorFinal = colorBase;

                        // Borde superior e izquierdo: brillo (2 píxeles)
                        if (pixelYBloque <= 1 || pixelXBloque <= 1)
                            colorFinal = colorBrillo [colorBase];
                        else if (pixelYBloque >= tamMino - 2 || pixelXBloque >= tamMino - 2)
                            colorFinal = colorSombra [colorBase]; // Borde inferior y derecho: sombra

                        gbt_dibujar_pixel(posXPantalla + pixelXBloque, posYPantalla + pixelYBloque, colorFinal);
                    }
                }
            }
        }
    }
}

// Pinta toda la ventana con fondo azul oscuro (color 1) y una cuadrícula decorativa cada 16px (color 8)
void DIBUJARFONDO ()
{
    int x, y;
    int color;

    for (y = 0; y < altoVentana; y ++)
    {
        for (x = 0; x < anchoVentana; x ++)
        {
            color = 1;
            // Cuadrícula decorativa: pinta de gris las líneas cada 16 píxeles
            if ((x % 16 == 0) || (y % 16 == 0))
                color = 8;
            gbt_dibujar_pixel(x, y, color);
        }
    }
}

// Dibuja un marco alrededor del tablero usando DIBUJARMARCOGENERICO
void DIBUJARMARCO ()
{
    int x0 = offsetHorizontal - 4;
    int y0 = offsetVertical - 4;
    int ancho = columnasTablero * tamMino + 8;
    int alto  = filasTablero * tamMino + 8;
    DIBUJARMARCOGENERICO(x0, y0, ancho, alto, 0);
}

// Dibuja un marco rectangular genérico con borde 3D (brillo arriba/izquierda, sombra abajo/derecha)
void DIBUJARMARCOGENERICO (int x0, int y0, int ancho, int alto, int colorFondo)
{
    int x, y;
    int color;

    for (y = y0; y < y0 + alto; y ++)
    {
        for (x = x0; x < x0 + ancho; x ++)
        {
            color = colorFondo;

            if (y <= y0 + 1 || x <= x0 + 1)           // Bordes superior e izquierdo → gris claro (brillo)
                color = 7;
            else if (y >= y0 + alto - 2 || x >= x0 + ancho - 2) // Bordes inferior y derecho → gris oscuro (sombra)
                color = 8;

            gbt_dibujar_pixel(x, y, color);
        }
    }
}

// Dibuja la grilla de líneas divisorias entre celdas del tablero (color 8, gris oscuro)
void DIBUJARGRILLA ()
{
    int fila, columna;
    int x, y;

    // Líneas horizontales
    for (fila = 0; fila <= filasTablero; fila ++)
    {
        y = offsetVertical + fila * tamMino;
        for (x = offsetHorizontal; x <= offsetHorizontal + columnasTablero * tamMino; x ++)
            gbt_dibujar_pixel(x, y, 8);
    }

    // Líneas verticales
    for (columna = 0; columna <= columnasTablero; columna ++)
    {
        x = offsetHorizontal + columna * tamMino;
        for (y = offsetVertical; y <= offsetVertical + filasTablero * tamMino; y ++)
            gbt_dibujar_pixel(x, y, 8);
    }
}

// Dibuja un carácter en la pantalla usando la fuente 8×8 (anchoCaracter8) o 8×16 (anchoCaracter16)
void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color)
{
    int filaCaracter, columnaCaracter;
    for (filaCaracter = 0; filaCaracter < altoCaracter; filaCaracter ++)
    {
        for (columnaCaracter = 0; columnaCaracter < anchoCaracter; columnaCaracter ++)
        {
            if (anchoCaracter == anchoCaracter8)
            {
                if (fuente8x8 [caracter][filaCaracter][columnaCaracter] == 1)
                    gbt_dibujar_pixel (posXPantalla + columnaCaracter, posYPantalla + filaCaracter, color);
            }
            if (anchoCaracter == anchoCaracter16)
            {
                if (fuente8x16 [caracter][filaCaracter][columnaCaracter] == 1)
                    gbt_dibujar_pixel (posXPantalla + columnaCaracter, posYPantalla + filaCaracter, color);
            }
        }
    }
}

// Dibuja una cadena de texto en la pantalla con fuente monoespaciada
// Soporta A-Z, 0-9 y espacio
void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter)
{
    int i = 0, caracter;
    while (texto [i] != '\0')
    {
        caracter = -1;
        if (texto [i] >= 'A' && texto [i] <= 'Z')
            caracter = texto [i] - 'A';       // 0-25
        else if (texto [i] >= '0' && texto [i] <= '9')
            caracter = 26 + (texto [i] - '0'); // 26-35
        else if (texto [i] == ' ')
            caracter = 36;                     // espacio

        if (caracter != -1)
            DIBUJARCARACTER (posXPantalla + i * anchoCaracter, posYPantalla, caracter, anchoCaracter, 7);
        i++ ;
    }
}

// Dibuja el indicador CHEAT debajo del panel NEXT
// Color 7 (gris claro) si está disponible o activo, color 8 (gris oscuro) si en cooldown
// Fondo negro para asegurar legibilidad en cualquier paleta
void DIBUJARCHEAT ()
{
    int ancho = (int)strlen("CHEAT") * anchoCaracter8;
    int x0 = offsetHorizontal - 74;
    int y0 = offsetVertical + 4 * tamMino + 26 + 10;
    DIBUJAR_RECTANGULO(x0 - 2, y0 - 2, ancho + 4, altoCaracter + 4, COLOR_NEGRO);
    int color = (cheat_cooldown_restante > 0.0 && !cheat_activo) ? 8 : 7;
    const char *txt = "CHEAT";
    for (int i = 0; txt[i] != '\0'; i++)
    {
        if (txt[i] >= 'A' && txt[i] <= 'Z')
            DIBUJARCARACTER(x0 + i * anchoCaracter8, y0, txt[i] - 'A', anchoCaracter8, color);
    }
}

// Dibuja texto con fuente proporcional (cada letra tiene su propio ancho)
// Usa anchoProp[] para determinar el ancho de cada carácter y fuenteProp[][][] para los píxeles
// Interletrado de 1 píxel entre caracteres consecutivos
void DIBUJARTEXTOPROP (int x, int y, const char *texto, int color)
{
    int px = x;
    for (const char *p = texto; *p != '\0'; p++)
    {
        int idx = -1;
        if (*p >= 'A' && *p <= 'Z')
            idx = *p - 'A';
        else if (*p >= '0' && *p <= '9')
            idx = 26 + (*p - '0');
        else if (*p == ' ')
            idx = 36;
        else if (*p == '_')
            idx = 37;

        if (idx >= 0)
        {
            int w = anchoProp[idx];
            for (int fil = 0; fil < altoCaracter; fil++)
                for (int col = 0; col < w; col++)
                    if (fuenteProp[idx][fil][col])
                        gbt_dibujar_pixel(px + col, y + fil, color);
            px += w + 1; // Ancho del carácter + 1 de interletrado
        }
        else
        {
            px += 5; // Carácter desconocido: salto fijo de 5 píxeles
        }
    }
}

// Dibuja el panel NEXT (próxima pieza) a la izquierda del tablero
// Muestra un marco con la etiqueta "NEXT" y una miniatura centrada de la próxima pieza
void DIBUJARPROXIMA ()
{
    int f, c, px, py, x0, y0;
    int colorBase, colorFinal, pixelX, pixelY;

    x0 = offsetHorizontal - 80;
    y0 = offsetVertical;

    int anchoPanel = 4 * tamMino + 16;
    int altoPanel  = 4 * tamMino + 26;
    DIBUJARMARCOGENERICO(x0 - 4, y0 - 4, anchoPanel, altoPanel, 0);

    DIBUJARTEXTO(x0, y0, "NEXT", anchoCaracter8);

    // Durante la animación de borrado no se dibuja la pieza (el tablero cambia)
    if (animacion_borrado_activa)
        return;

    // Calcula el bounding box de la pieza para centrarla en el panel
    int minC = 4, maxC = -1, minF = 4, maxF = -1;
    for (f = 0; f < 4; f++)
        for (c = 0; c < 4; c++)
            if (proxima.forma[f][c] == 1)
            {
                if (c < minC) minC = c;
                if (c > maxC) maxC = c;
                if (f < minF) minF = f;
                if (f > maxF) maxF = f;
            }

    int anchoInterior = anchoPanel - 16;
    int anchoPieza = (maxC - minC + 1) * tamMino;
    int altoPieza  = (maxF - minF + 1) * tamMino;
    int offsetX = (anchoInterior - anchoPieza) / 2 - minC * tamMino;
    int offsetY = (anchoInterior - altoPieza)  / 2 - minF * tamMino;

    // Dibuja los bloques de la miniatura con efecto 3D (brillo/sombra)
    for (f = 0; f < 4; f++)
    {
        for (c = 0; c < 4; c++)
        {
            if (proxima.forma[f][c] == 1)
            {
                colorBase = proxima.color;
                px = x0 + offsetX + c * tamMino;
                py = y0 + 15 + offsetY + f * tamMino;

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

// Muestra las estadísticas (SCORE, LEVEL, LINES, SPEED) a la derecha del tablero
void DIBUJARPUNTAJE ()
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

// Dibuja el título "TETRIS" en la pantalla de inicio con letras de colores alternados
void DIBUJARTITULO ()
{
    char titulo [] = "TETRIS";
    int colores [] = {12, 14, 10, 11, 13, 9};
    int i, caracter;
    for (i = 0; i < 6; i ++)
    {
        if (titulo [i] >= 'A' && titulo [i] <= 'Z')
            caracter = titulo [i] - 'A';
        DIBUJARCARACTER (100 + i * anchoCaracter16, 50, caracter, anchoCaracter16, colores [i]);
    }
}

// Pantalla de pausa superpuesta: marco centrado con "PAUSA" y opciones
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
    char *linea2 = "P CONTINUAR   Q SALIR";

    DIBUJARTEXTO(centroX - ((int)strlen(linea1) * anchoCaracter8) / 2, y0 + 10, linea1, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea2) * anchoCaracter8) / 2, y0 + 30, linea2, anchoCaracter8);
}

// Pantalla de Game Over superpuesta: marco centrado con score, opciones reiniciar/salir
void DIBUJARGAMEOVER ()
{
    int anchoCaja = 180;
    int altoCaja  = 90;
    int centroX = anchoVentana / 2;
    int centroY = altoVentana  / 2;
    int x0 = centroX - anchoCaja / 2;
    int y0 = centroY - altoCaja  / 2;

    DIBUJARMARCOGENERICO(x0, y0, anchoCaja, altoCaja, 0);

    char puntajeFinal[32];
    sprintf(puntajeFinal, "SCORE %d", puntaje);

    char *linea1 = "GAME OVER";
    char *linea2 = puntajeFinal;
    char *linea3 = "R     REINICIAR";
    char *linea4 = "ENTER MENU PRINCIPAL";
    char *linea5 = "Q     SALIR";

    DIBUJARTEXTO(centroX - ((int)strlen(linea1) * anchoCaracter8) / 2, y0 + 8,  linea1, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea2) * anchoCaracter8) / 2, y0 + 22, linea2, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea3) * anchoCaracter8) / 2, y0 + 42, linea3, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea4) * anchoCaracter8) / 2, y0 + 55, linea4, anchoCaracter8);
    DIBUJARTEXTO(centroX - ((int)strlen(linea5) * anchoCaracter8) / 2, y0 + 68, linea5, anchoCaracter8);
}

// Pantalla de ingreso de nombre del jugador
// Teclado virtual con letras A-Z, 0-9, espacio, retroceso; ESC cancela (nombre vacío)
// Muestra el logo TETRIS, "INGRESE SU NOMBRE:", el nombre con cursor y la instrucción ESC
void DIBUJARINICIO(char *nombre)
{
    int i = 0;
    int terminado = 0;
    eGBT_Tecla tecla;

    nombre[0] = '\0';

    while (!terminado)
    {
        gbt_procesar_entrada();
        tecla = gbt_obtener_tecla_presionada();

        if (tecla != GBTK_DESCONOCIDA)
        {
            if (tecla == GBTK_ESCAPE)
            {
                nombre[0] = '\0'; // Señal de cancelación
                return;
            }
            else if (tecla == GBTK_ENTER)
            {
                if (i > 0)      // No permite nombre vacío
                    terminado = 1;
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
                if (i < 13)     // Máximo 13 caracteres
                {
                    nombre[i] = tecla - 32; // Convierte a mayúscula
                    i++;
                    nombre[i] = '\0';
                }
            }
            else if (tecla >= '0' && tecla <= '9')
            {
                if (i < 13)
                {
                    nombre[i] = tecla;
                    i++;
                    nombre[i] = '\0';
                }
            }
            else if (tecla == GBTK_ESPACIO)
            {
                if (i < 13 && i > 0)
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

        // Logo TETRIS centrado
        int logoY = (altoVentana / 2 - 110) / 2;
        if (logoY < 3) logoY = 3;
        DIBUJAR_LOGO_COMPLETO((anchoVentana - 167) / 2, logoY);

        // Grupo de texto debajo del logo
        int textY = logoY + 110 + (altoVentana - (logoY + 110) - 8 - 10 - 8 - 10 - 8) / 2;
        int nameY = textY + 18;
        int escY = nameY + 18;
        DIBUJARTEXTO((anchoVentana - 18 * anchoCaracter8) / 2, textY, "INGRESE SU NOMBRE:", anchoCaracter8);
        DIBUJARTEXTO((anchoVentana - 13 * anchoCaracter8) / 2, nameY, nombre, anchoCaracter8);

        // Cursor parpadeante (carácter 37 = '_')
        if (i < 13)
        {
            DIBUJARCARACTER(
                (anchoVentana - 13 * anchoCaracter8) / 2 + i * anchoCaracter8,
                nameY, 37, anchoCaracter8, 7
            );
        }

        DIBUJARTEXTO((anchoVentana - 19 * anchoCaracter8) / 2, escY, "ESC VOLVER AL MENU", anchoCaracter8);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}
