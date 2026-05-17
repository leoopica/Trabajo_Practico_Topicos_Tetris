#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "configuracion.h"
#include "graficos.h"
#include "archivos.h"

// Resultado que devuelve MENU_PRINCIPAL
typedef enum {
    MENU_RESULTADO_JUGAR,
    MENU_RESULTADO_CONTINUAR,
    MENU_RESULTADO_SALIR,
} eMenuResultado;

// Resultado que devuelve el menú de pausa
typedef enum {
    PAUSA_RESULTADO_CONTINUAR,
    PAUSA_RESULTADO_GUARDAR_Y_SALIR,
    PAUSA_RESULTADO_SALIR_SIN_GUARDAR,
} ePausaResultado;

// Muestra el menú principal y retorna lo que eligió el jugador
eMenuResultado MENU_PRINCIPAL(void);

// Muestra el menú de pausa con opciones (retorna qué eligió el jugador)
ePausaResultado MENU_PAUSA(void);

// Muestra el submenú de opciones (modifica config_actual y guarda)
void MENU_OPCIONES(void);

// Muestra la pantalla de estadísticas
void MENU_ESTADISTICAS(void);

// Muestra la pantalla de instrucciones
void MENU_INSTRUCCIONES(void);

#endif // MENU_H_INCLUDED