#include "inicio.h"

/**
 * @brief Dibuja un rectángulo relleno de un color sólido.
 */
void DIBUJAR_RECTANGULO(int x_origen, int y_origen, int ancho, int alto, int color) {
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            gbt_dibujar_pixel(x_origen + x, y_origen + y, color);
        }
    }
}

/**
 * @brief Dibuja solo el contorno (borde) de un rectángulo.
 */
void DIBUJAR_BORDE_RECTANGULO(int x_origen, int y_origen, int ancho, int alto, int color) {
    // Líneas horizontales (Arriba y Abajo)
    for (int x = 0; x < ancho; x++) {
        gbt_dibujar_pixel(x_origen + x, y_origen, color);
        gbt_dibujar_pixel(x_origen + x, y_origen + alto - 1, color);
    }
    // Líneas verticales (Izquierda y Derecha)
    for (int y = 0; y < alto; y++) {
        gbt_dibujar_pixel(x_origen, y_origen + y, color);
        gbt_dibujar_pixel(x_origen + ancho - 1, y_origen + y, color);
    }
}

// Mapa ASCII del logo TETRIS (12 filas x 49 columnas).
// 5 Letras de 7 columnas + Letra "I" de 3 columnas + 1 de sombra por letra + 5 espacio de separación: 5*7 + 3 + 6 + 5 = 42.
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
    "  TTTM   EEEEEEEM   222M   RRM  RRM IIIM SSSSSSSM",
    "   MMM    MMMMMMM    MMM    MM   MM  MMM  MMMMMMM",
    "                                                 "
};

void DIBUJAR_TEXTO_TETRIS(int x_origen, int y_origen) {
    for (int y = 0; y < LOGO_TEXTO_ALTO; y++) {
        for (int x = 0; x < LOGO_TEXTO_ANCHO; x++) {

            int pixel = sprite_tetris_ascii[y][x];
            int color_indice;

            switch (pixel) {
                case 'T': color_indice = COLOR_ROJO; break; //
                case 'E': color_indice = COLOR_ROJO_CLARO;  break; //
                case '2': color_indice = COLOR_AMARILLO; break; //
                case 'R': color_indice = COLOR_VERDE_CLARO; break; //
                case 'I': color_indice = COLOR_CIAN_CLARO; break; //
                case 'S': color_indice = COLOR_MAGENTA_CLARO; break; //
                case 'M': color_indice = COLOR_NEGRO;   break;
                default:  continue;
            }

            int escala = 3;
            for (int dy = 0; dy < escala; dy++) {
                for (int dx = 0; dx < escala; dx++) {
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

/**
 * @brief Dibuja el logo completo de Tetris en las coordenadas indicadas.
 * * @param x_base Coordenada X donde arranca la punta superior izquierda del logo.
 * @param y_base Coordenada Y donde arranca la punta superior izquierda del logo.
 */
void DIBUJAR_LOGO_COMPLETO(int x_base, int y_base) {
    // Dimensiones de la "T" gigante
    const int ANCHO_TOP   = 167;   // bloque horizontal (texto 147 + 10 margen *2)
    const int ALTO_TOP    = 50;    // alto del bloque horizontal
    const int ANCHO_PALO  = 61;    // ancho del palo vertical (impar -> centrado perfecto)
    const int ALTO_PALO   = 50;    // alto del palo
    const int ANCHO_PADDING_TOP = ANCHO_TOP +10;
    const int ALTO_PADDING_TOP  = ALTO_TOP +10;
    const int ANCHO_PADDING_PALO = ANCHO_PALO +10;
    const int ALTO_PADDING_PALO  = ALTO_PALO +10;
    const int POSICION_PADDING_X = x_base-5;
    const int POSICION_PADDING_Y = y_base-5;

    //Dimensiones del padding exterior de la T

    // Centrado matemático del palo respecto del bloque superior
    const int OFFSET_PALO = (ANCHO_TOP - ANCHO_PALO) / 2;  // = 60

    // 0. Dibujamos padding exterior con sus bordes
    //Padding PALO
    DIBUJAR_RECTANGULO(POSICION_PADDING_X + OFFSET_PALO, POSICION_PADDING_Y + ALTO_TOP, ANCHO_PADDING_PALO, ALTO_PADDING_PALO, COLOR_AZUL_CLARO);
    DIBUJAR_BORDE_RECTANGULO(POSICION_PADDING_X + OFFSET_PALO, POSICION_PADDING_Y + ALTO_TOP, ANCHO_PADDING_PALO, ALTO_PADDING_PALO, COLOR_CIAN_CLARO);

    // Padding TOP
    DIBUJAR_RECTANGULO(POSICION_PADDING_X,POSICION_PADDING_Y,ANCHO_PADDING_TOP,ALTO_PADDING_TOP,COLOR_AZUL_CLARO);
    DIBUJAR_BORDE_RECTANGULO(POSICION_PADDING_X,POSICION_PADDING_Y,ANCHO_PADDING_TOP,ALTO_PADDING_TOP,COLOR_CIAN_CLARO);

    // 0.5 Tapamos la union interna entre los bordes
    for (int x = POSICION_PADDING_X + OFFSET_PALO + 1;
                  x < POSICION_PADDING_X + OFFSET_PALO + ANCHO_PADDING_PALO - 1; x++) {
        gbt_dibujar_pixel(x, POSICION_PADDING_Y + ALTO_PADDING_TOP - 1, COLOR_AZUL_CLARO); // borde inferior del top
        gbt_dibujar_pixel(x, POSICION_PADDING_Y + ALTO_PADDING_TOP,     COLOR_AZUL_CLARO); // borde superior del palo
                  }

    // 1. Bloque horizontal (parte superior de la T)
    DIBUJAR_RECTANGULO(x_base, y_base, ANCHO_TOP, ALTO_TOP, COLOR_AZUL);

    // 2. Palo vertical, perfectamente centrado
    DIBUJAR_RECTANGULO(x_base + OFFSET_PALO, y_base + ALTO_TOP,
                       ANCHO_PALO, ALTO_PALO, COLOR_AZUL);

    // 3. Bordes
    DIBUJAR_BORDE_RECTANGULO(x_base, y_base, ANCHO_TOP, ALTO_TOP, COLOR_CIAN_CLARO);
    DIBUJAR_BORDE_RECTANGULO(x_base + OFFSET_PALO, y_base + ALTO_TOP,
                             ANCHO_PALO, ALTO_PALO, COLOR_CIAN_CLARO);

    // 4. Tapamos la unión interna entre los dos rectángulos
    //    (los píxeles internos del palo, dejando los bordes laterales del palo intactos)
    for (int x = x_base + OFFSET_PALO + 1;
                  x < x_base + OFFSET_PALO + ANCHO_PALO - 1; x++) {
        gbt_dibujar_pixel(x, y_base + ALTO_TOP - 1, COLOR_AZUL); // borde inferior del top
        gbt_dibujar_pixel(x, y_base + ALTO_TOP,     COLOR_AZUL); // borde superior del palo
    }

    // 5. Letras centradas dentro del bloque superior
    //    Texto = LOGO_TEXTO_ANCHO * escala(3) = 49 * 3 = 147
    //    margin interior lateral = (ANCHO_TOP - 147) / 2 = 10
    const int ANCHO_TEXTO = LOGO_TEXTO_ANCHO * 3;        // 147
    const int ALTO_TEXTO  = LOGO_TEXTO_ALTO  * 3;        // 33
    DIBUJAR_TEXTO_TETRIS(
        x_base + (ANCHO_TOP - ANCHO_TEXTO) / 2,               // 10
        y_base + (ALTO_TOP  - ALTO_TEXTO)  / 2                // 8
    );
}