#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED
#include <stdint.h>

typedef struct { /*Estructura de sprite*/
    uint16_t ancho;
    uint16_t alto;
    uint8_t *pixeles;
} Sprite;

extern Sprite sprite_mino; /*Sprite de mino. Buscar bien para qué sirve extern*/

extern Sprite font [128]; /*Sprite de fuente de texto. Buscar bien para qué sirve extern*/

void dibujar_sprite (Sprite *sprite, int ancho, int alto); /*Función para dibujar un sprite. Parámetros: puntero a sprite, ancho, alto*/
void dibujar_sprite_escalado (Sprite *sprite, int ancho, int alto, int escala); /*Función para dibujar un sprite aplicándole la escala definida. Parámetros: puntero a sprite, ancho, alto, escala*/
void dibujar_texto (const char *texto, int ancho, int alto, int escala); /*Función para dibujar el texto. Parámetros: puntero a texto a dibujar, ancho, alto, escala*/

#endif // GRAFICOS_H_INCLUDED