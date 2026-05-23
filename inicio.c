#include "inicio.h"

// Dibuja un rectángulo relleno de color sólido, píxel por píxel
void DIBUJAR_RECTANGULO(int x_origen, int y_origen, int ancho, int alto, int color) {
    for (int y = 0; y < alto; y++)
        for (int x = 0; x < ancho; x++)
            gbt_dibujar_pixel(x_origen + x, y_origen + y, color);
}

// Dibuja solo el contorno (borde) de un rectángulo, sin relleno
void DIBUJAR_BORDE_RECTANGULO(int x_origen, int y_origen, int ancho, int alto, int color) {
    // Líneas horizontales superior e inferior
    for (int x = 0; x < ancho; x++) {
        gbt_dibujar_pixel(x_origen + x, y_origen, color);
        gbt_dibujar_pixel(x_origen + x, y_origen + alto - 1, color);
    }
    // Líneas verticales izquierda y derecha
    for (int y = 0; y < alto; y++) {
        gbt_dibujar_pixel(x_origen, y_origen + y, color);
        gbt_dibujar_pixel(x_origen + ancho - 1, y_origen + y, color);
    }
}

// Mapa ASCII del texto "TETRIS" (12 filas × 49 columnas, escala 3x = 147×36 px aprox)
// Cada letra es un carácter distinto (T, E, 2, R, I, S) y M = máscara/sombra negra
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

// Dibuja el texto "TETRIS" a escala 3× usando sprite_tetris_ascii
// Cada letra tiene su color asignado: T=rojo, E=rojo claro, 2=amarillo, R=verde claro, I=cian, S=magenta, M=negro (sombra)
void DIBUJAR_TEXTO_TETRIS(int x_origen, int y_origen) {
    for (int y = 0; y < LOGO_TEXTO_ALTO; y++) {
        for (int x = 0; x < LOGO_TEXTO_ANCHO; x++) {

            int pixel = sprite_tetris_ascii[y][x];
            int color_indice;

            // Asigna color según el carácter en el mapa ASCII
            switch (pixel) {
                case 'T': color_indice = COLOR_ROJO;         break;
                case 'E': color_indice = COLOR_ROJO_CLARO;   break;
                case '2': color_indice = COLOR_AMARILLO;     break;
                case 'R': color_indice = COLOR_VERDE_CLARO;  break;
                case 'I': color_indice = COLOR_CIAN_CLARO;   break;
                case 'S': color_indice = COLOR_MAGENTA_CLARO;break;
                case 'M': color_indice = COLOR_NEGRO;        break;
                default:  continue; // Espacios: no dibuja nada
            }

            // Cada píxel del mapa se dibuja como un bloque 3×3 en pantalla
            int escala = 3;
            for (int dy = 0; dy < escala; dy++)
                for (int dx = 0; dx < escala; dx++)
                    gbt_dibujar_pixel(
                        x_origen + (x * escala) + dx,
                        y_origen + (y * escala) + dy,
                        color_indice
                    );
        }
    }
}

// Dibuja el logo completo de TETRIS: una "T" gigante azul con el texto "TETRIS" centrado arriba
// El logo es una T compuesta por dos rectángulos (bloque horizontal + palo vertical)
// con padding exterior (rectángulos celestes) y bordes cian claro
void DIBUJAR_LOGO_COMPLETO(int x_base, int y_base) {
    // Dimensiones fijas de la "T" gigante
    const int ANCHO_TOP   = 167; // Ancho del bloque horizontal (texto 147 + 10 px de margen a cada lado)
    const int ALTO_TOP    = 50;  // Alto del bloque horizontal
    const int ANCHO_PALO  = 61;  // Ancho del palo vertical (impar para centrado simétrico)
    const int ALTO_PALO   = 50;  // Alto del palo vertical
    const int ANCHO_PADDING_TOP  = ANCHO_TOP + 10;   // Padding exterior: +5 px por lado
    const int ALTO_PADDING_TOP   = ALTO_TOP + 10;
    const int ANCHO_PADDING_PALO = ANCHO_PALO + 10;
    const int ALTO_PADDING_PALO  = ALTO_PALO + 10;
    const int POSICION_PADDING_X = x_base - 5;
    const int POSICION_PADDING_Y = y_base - 5;

    // Desplazamiento del palo para que quede centrado bajo el bloque horizontal
    const int OFFSET_PALO = (ANCHO_TOP - ANCHO_PALO) / 2; // ≈ 60 px

    // 0. Padding exterior: rectángulo azul claro con borde cian claro
    // Padding del palo
    DIBUJAR_RECTANGULO(POSICION_PADDING_X + OFFSET_PALO, POSICION_PADDING_Y + ALTO_TOP,
                       ANCHO_PADDING_PALO, ALTO_PADDING_PALO, COLOR_AZUL_CLARO);
    DIBUJAR_BORDE_RECTANGULO(POSICION_PADDING_X + OFFSET_PALO, POSICION_PADDING_Y + ALTO_TOP,
                             ANCHO_PADDING_PALO, ALTO_PADDING_PALO, COLOR_CIAN_CLARO);
    // Padding del top
    DIBUJAR_RECTANGULO(POSICION_PADDING_X, POSICION_PADDING_Y,
                       ANCHO_PADDING_TOP, ALTO_PADDING_TOP, COLOR_AZUL_CLARO);
    DIBUJAR_BORDE_RECTANGULO(POSICION_PADDING_X, POSICION_PADDING_Y,
                             ANCHO_PADDING_TOP, ALTO_PADDING_TOP, COLOR_CIAN_CLARO);

    // 0.5 Tapa la unión entre los bordes del top y el palo (línea horizontal interior)
    for (int x = POSICION_PADDING_X + OFFSET_PALO + 1;
         x < POSICION_PADDING_X + OFFSET_PALO + ANCHO_PADDING_PALO - 1; x++) {
        gbt_dibujar_pixel(x, POSICION_PADDING_Y + ALTO_PADDING_TOP - 1, COLOR_AZUL_CLARO);
        gbt_dibujar_pixel(x, POSICION_PADDING_Y + ALTO_PADDING_TOP,     COLOR_AZUL_CLARO);
    }

    // 1. Cuerpo principal del logo: bloque horizontal (parte superior de la T)
    DIBUJAR_RECTANGULO(x_base, y_base, ANCHO_TOP, ALTO_TOP, COLOR_AZUL);

    // 2. Palo vertical centrado
    DIBUJAR_RECTANGULO(x_base + OFFSET_PALO, y_base + ALTO_TOP,
                       ANCHO_PALO, ALTO_PALO, COLOR_AZUL);

    // 3. Bordes de ambos rectángulos (cian claro)
    DIBUJAR_BORDE_RECTANGULO(x_base, y_base, ANCHO_TOP, ALTO_TOP, COLOR_CIAN_CLARO);
    DIBUJAR_BORDE_RECTANGULO(x_base + OFFSET_PALO, y_base + ALTO_TOP,
                             ANCHO_PALO, ALTO_PALO, COLOR_CIAN_CLARO);

    // 4. Tapa la unión interna entre los dos rectángulos (borde interior del top)
    for (int x = x_base + OFFSET_PALO + 1; x < x_base + OFFSET_PALO + ANCHO_PALO - 1; x++) {
        gbt_dibujar_pixel(x, y_base + ALTO_TOP - 1, COLOR_AZUL);
        gbt_dibujar_pixel(x, y_base + ALTO_TOP,     COLOR_AZUL);
    }

    // 5. Texto "TETRIS" centrado dentro del bloque horizontal
    //    El texto ocupa 49×3 = 147 px de ancho, centrado en ANCHO_TOP=167 (margen 10 px c/u)
    const int ANCHO_TEXTO = LOGO_TEXTO_ANCHO * 3;  // = 147
    const int ALTO_TEXTO  = LOGO_TEXTO_ALTO  * 3;  // = 39
    DIBUJAR_TEXTO_TETRIS(
        x_base + (ANCHO_TOP - ANCHO_TEXTO) / 2,
        y_base + (ALTO_TOP  - ALTO_TEXTO)  / 2
    );
}