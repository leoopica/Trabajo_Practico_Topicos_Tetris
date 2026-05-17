#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED

#include <stdio.h>
#include "gbt.h"
#include "sprites.h"
#include "inicio.h"
#include "funcionalidades.h"
#include "configuracion.h"

// Constantes fijas
#define cantColores   16
#define escalaVentana (config_actual.escala)

// Resolución dinámica según config
#define anchoVentana  CONFIG_ANCHO()
#define altoVentana   CONFIG_ALTO()

// Offsets centrados dinámicamente
#define offsetHorizontal (CONFIG_ANCHO() - columnasTablero * tamMino) / 2
#define offsetVertical   (CONFIG_ALTO()  - filasTablero   * tamMino) / 2

// Variables globales
extern tGBT_ColorRGB paletaCGA [cantColores];
extern int colorBrillo [cantColores];
extern int colorSombra [cantColores];

// Funciones de dibujo
void DIBUJAR ();
void DIBUJARFONDO ();
void DIBUJARMARCO ();
void DIBUJARMARCOGENERICO (int x0, int y0, int ancho, int alto, int colorFondo);
void DIBUJARGRILLA ();
void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color);
void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter);
void DIBUJARPROXIMA ();
void DIBUJARPUNTAJE ();
void DIBUJARTITULO ();
void DIBUJARPAUSA ();
void DIBUJARGAMEOVER ();
void DIBUJARINICIO (char *nombre);
void DIBUJARCHEAT ();
void DIBUJARTEXTOPROP (int x, int y, const char *texto, int color);

#endif // GRAFICOS_H_INCLUDED
