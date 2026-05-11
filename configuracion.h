#ifndef CONFIGURACION_H_INCLUDED
#define CONFIGURACION_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "gbt.h"

// Resoluciones lógicas disponibles
#define RESOLUCION_CGA 0 // 320x200
#define RESOLUCION_VGA 1 // 640x480

#define ANCHO_CGA 320
#define ALTO_CGA 200
#define ANCHO_VGA 640
#define ALTO_VGA 480

// Paletas disponibles
#define PALETA_CGA 0
#define PALETA_GAMEBOY 1
#define PALETA_ATARI 2
#define CANT_PALETAS 3

// Velocidades de caída iniciales disponibles
#define CANT_VELOCIDADES 3
extern float velocidades_disponibles [CANT_VELOCIDADES];
extern const char* nombres_velocidades [CANT_VELOCIDADES];

// Estructura de configuración
typedef struct {
    int resolucion; // RESOLUCION_CGA o RESOLUCION_VGA
    int escala; // Factor de escala de la ventana
    int paleta; // PALETA_CGA, PALETA_GAMEBOY, PALETA_NES
    int velocidad_inicial; // Índice en velocidades_disponibles
} sConfig;

// Paletas de colores
extern tGBT_ColorRGB paletas [CANT_PALETAS][16]; // CANT_PALETAS, cantidad de colores

// Configuración global activa
extern sConfig config_actual;

// Funciones
void CONFIG_DEFAULTS (sConfig *c); // Carga valores por defecto. Se usa cuando no existe el archivo config.dat
int CONFIG_GUARDAR (const sConfig *c); // Guarda en config.dat
int  CONFIG_CARGAR (sConfig *c); // Carga desde config.dat
void CONFIG_APLICAR (const sConfig *c); // Aplica la configuración al juego
int CONFIG_ANCHO (); // Retorna ancho según la resolución activa
int CONFIG_ALTO (); // Retorna alto según la resolución activa

#endif // CONFIGURACION_H_INCLUDED