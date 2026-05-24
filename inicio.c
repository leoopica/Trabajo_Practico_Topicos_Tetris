#include "inicio.h"
#include "gbt.h"

// Dibuja un rectángulo relleno de color sólido, píxel por píxel
void DIBUJAR_RECTANGULO(int x_origen, int y_origen, int ancho, int alto, int color)
{
    int x, y;

    for (y = 0; y < alto; y++)
    {
        for (x = 0; x < ancho; x++)
        {
            gbt_dibujar_pixel(x_origen + x, y_origen + y, color);
        }
    }
}

// Dibuja solo el contorno (borde) de un rectángulo, sin relleno interior
void DIBUJAR_BORDE_RECTANGULO(int x_origen, int y_origen, int ancho, int alto, int color)
{
    int x, y;

    for (x = 0; x < ancho; x++)
    {
        gbt_dibujar_pixel(x_origen + x, y_origen, color);
    }
    for (x = 0; x < ancho; x++)
    {
        gbt_dibujar_pixel(x_origen + x, y_origen + alto - 1, color);
    }
    for (y = 0; y < alto; y++)
    {
        gbt_dibujar_pixel(x_origen, y_origen + y, color);
    }
    for (y = 0; y < alto; y++)
    {
        gbt_dibujar_pixel(x_origen + ancho - 1, y_origen + y, color);
    }
}

// Mapa ASCII del texto "TETRIS" (13 filas × 49 columnas ≈ 147×39 píxeles a escala 3×)
//   T = rojo,       E = rojo claro,   2 = amarillo,
//   R = verde claro, I = cian claro,   S = magenta claro,
//   M = máscara/sombra negra,         espacio = transparente
const char* sprite_tetris_ascii[LOGO_TEXTO_ALTO] = {
    "                                                 ",
    "TTTTTTT  EEEEEEE  2222222  RRRRRR   III  SSSSSSS ",
    "TTTTTTTM EEEEEEEM 2222222M RRRRRRR  IIIM SSSSSSSM",
    "  TTTMMM EEMMMMMM   222MMM RRMMMRRM IIIM SSMMMMMM",
    "  TTTM   EEM        222M   RRM  RRM IIIM SSM    ",
    "  TTTM   EEEEE      222M   RRRRRRRM IIIM SSSSSSS ",
    "  TTTM   EEEEEM     222M   RRRRRRMM IIIM SSSSSSSM",
    "  TTTM   EEMMMM     222M   RRMRRMM  IIIM  MMMMSSM",
    "  TTTM   EEM        222M   RRM RR   IIIM      SSM",
    "  TTTM   EEEEEEE    222M   RRM  RR  IIIM SSSSSSSM",
    "  TTTM   EEEEEM     222M   RRM  RRM IIIM SSSSSSSM",
    "   MMM    MMMMMMM    MMM    MM   MM  MMM  MMMMMMM",
    "                                                 "
};

// Dibuja el texto "TETRIS" a escala 3× usando el mapa sprite_tetris_ascii
void DIBUJAR_TEXTO_TETRIS(int x_origen, int y_origen)
{
    int y, x; // Índices para recorrer el mapa del texto
    int pixel; // Carácter en la posición actual del mapa
    int color_indice; // Índice de color asignado según el carácter
    int escala; // Escala de cada píxel del mapa
    int dy, dx; // Píxeles internos del bloque escalado

    escala = 3;

    for (y = 0; y < LOGO_TEXTO_ALTO; y++)
    {
        for (x = 0; x < LOGO_TEXTO_ANCHO; x++)
        {
            pixel = sprite_tetris_ascii[y][x];

            switch (pixel)
            {
                case 'T': color_indice = COLOR_ROJO;         break;
                case 'E': color_indice = COLOR_ROJO_CLARO;   break;
                case '2': color_indice = COLOR_AMARILLO;     break;
                case 'R': color_indice = COLOR_VERDE_CLARO;  break;
                case 'I': color_indice = COLOR_CIAN_CLARO;   break;
                case 'S': color_indice = COLOR_MAGENTA_CLARO;break;
                case 'M': color_indice = COLOR_NEGRO;        break;
                default:  continue;
            }

            for (dy = 0; dy < escala; dy++)
            {
                for (dx = 0; dx < escala; dx++)
                {
                    gbt_dibujar_pixel(
                        x_origen + (x * escala) + dx,
                        y_origen + (y * escala) + dy,
                        color_indice
                    );
                }
            }
        }
    }
}

// Dibuja el logo completo de TETRIS: una "T" gigante azul con bordes cian claro
void DIBUJAR_LOGO_COMPLETO(int x_base, int y_base)
{
    const int ANCHO_TOP = 167;
    const int ALTO_TOP = 50;
    const int ANCHO_PALO = 61;
    const int ALTO_PALO = 50;
    const int ANCHO_PADDING_TOP = ANCHO_TOP + 10;
    const int ALTO_PADDING_TOP = ALTO_TOP + 10;
    const int ANCHO_PADDING_PALO = ANCHO_PALO + 10;
    const int ALTO_PADDING_PALO = ALTO_PALO + 10;
    const int POSICION_PADDING_X = x_base - 5;
    const int POSICION_PADDING_Y = y_base - 5;
    const int OFFSET_PALO = (ANCHO_TOP - ANCHO_PALO) / 2;
    const int ANCHO_TEXTO = LOGO_TEXTO_ANCHO * 3;
    const int ALTO_TEXTO  = LOGO_TEXTO_ALTO  * 3;
    int x; // Índice para tapar uniones

    DIBUJAR_RECTANGULO (POSICION_PADDING_X + OFFSET_PALO, POSICION_PADDING_Y + ALTO_TOP, ANCHO_PADDING_PALO, ALTO_PADDING_PALO, COLOR_AZUL_CLARO);
    DIBUJAR_BORDE_RECTANGULO (POSICION_PADDING_X + OFFSET_PALO, POSICION_PADDING_Y + ALTO_TOP, ANCHO_PADDING_PALO, ALTO_PADDING_PALO, COLOR_CIAN_CLARO);
    DIBUJAR_RECTANGULO (POSICION_PADDING_X, POSICION_PADDING_Y, ANCHO_PADDING_TOP, ALTO_PADDING_TOP, COLOR_AZUL_CLARO);
    DIBUJAR_BORDE_RECTANGULO (POSICION_PADDING_X, POSICION_PADDING_Y, ANCHO_PADDING_TOP, ALTO_PADDING_TOP, COLOR_CIAN_CLARO);

    for (x = POSICION_PADDING_X + OFFSET_PALO + 1; x < POSICION_PADDING_X + OFFSET_PALO + ANCHO_PADDING_PALO - 1; x++)
    {
        gbt_dibujar_pixel(x, POSICION_PADDING_Y + ALTO_PADDING_TOP - 1, COLOR_AZUL_CLARO);
        gbt_dibujar_pixel(x, POSICION_PADDING_Y + ALTO_PADDING_TOP, COLOR_AZUL_CLARO);
    }

    DIBUJAR_RECTANGULO(x_base, y_base, ANCHO_TOP, ALTO_TOP, COLOR_AZUL);
    DIBUJAR_RECTANGULO(x_base + OFFSET_PALO, y_base + ALTO_TOP, ANCHO_PALO, ALTO_PALO, COLOR_AZUL);
    DIBUJAR_BORDE_RECTANGULO(x_base, y_base, ANCHO_TOP, ALTO_TOP, COLOR_CIAN_CLARO);
    DIBUJAR_BORDE_RECTANGULO(x_base + OFFSET_PALO, y_base + ALTO_TOP, ANCHO_PALO, ALTO_PALO, COLOR_CIAN_CLARO);

    for (x = x_base + OFFSET_PALO + 1; x < x_base + OFFSET_PALO + ANCHO_PALO - 1; x++)
    {
        gbt_dibujar_pixel(x, y_base + ALTO_TOP - 1, COLOR_AZUL);
        gbt_dibujar_pixel(x, y_base + ALTO_TOP, COLOR_AZUL);
    }

    DIBUJAR_TEXTO_TETRIS( x_base + (ANCHO_TOP - ANCHO_TEXTO) / 2, y_base + (ALTO_TOP - ALTO_TEXTO)  / 2);
}
