#ifndef CONFIGURACION_H_INCLUDED
#define CONFIGURACION_H_INCLUDED

#include "gbt.h"

// Resoluciones
#define RESOLUCION_CGA 0 // 320x200 píxeles
#define RESOLUCION_VGA 1 // 640x480 píxeles

// Dimensiones según resolución
#define ANCHO_CGA 320
#define ALTO_CGA 200
#define ANCHO_VGA 640
#define ALTO_VGA 480

// Paletas de color
#define PALETA_CGA 0
#define PALETA_GAMEBOY 1 
#define PALETA_ATARI 2
#define CANT_PALETAS 3

// Velocidades de caída
#define CANT_VELOCIDADES 3
extern float velocidades_disponibles [CANT_VELOCIDADES];
extern const char* nombres_velocidades [CANT_VELOCIDADES];

// Modos de juego
#define MODO_CLASICO 0
#define MODO_DX 1

#define tamMino (CONFIG_TAM_MINO()) // Macro dinámica para el tamaño del mino

// Estructura de la configuración del juego
typedef struct {
    int resolucion;
    int escala;
    int paleta;
    int velocidad_inicial;
    int modo_juego;
    int ancho_tablero;
} sConfig;

// Array de las paletas de colores disponibles
extern tGBT_ColorRGB paletas [CANT_PALETAS][16];

// Configuración activa actualmente
extern sConfig config_actual;

void CONFIG_DEFAULTS (sConfig *c); // Carga valores de configuración por defecto
int CONFIG_GUARDAR (const sConfig *c); // Guarda configuración en config.dat
int CONFIG_CARGAR (sConfig *c); // Carga configuración desde config.dat
void CONFIG_APLICAR (const sConfig *c); // Aplica la configuración
int CONFIG_ANCHO (); // Retorna el ancho según resolución activa
int CONFIG_ALTO ();// Retorna el alto según resolución activa
int CONFIG_TAM_MINO ();// Retorna el tamaño del mino

#endif // CONFIGURACION_H_INCLUDED
