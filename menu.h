#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

typedef enum {
    MENU_RESULTADO_JUGAR,
    MENU_RESULTADO_CONTINUAR,
    MENU_RESULTADO_SALIR,
} eMenuResultado;

typedef enum {
    PAUSA_RESULTADO_CONTINUAR,
    PAUSA_RESULTADO_GUARDAR_Y_SALIR,
    PAUSA_RESULTADO_SALIR_SIN_GUARDAR,
} ePausaResultado;

// Menú principal con opciones JUGAR, CONTINUAR (si hay guardado), ESTADISTICAS, OPCIONES, SALIR
eMenuResultado MENU_PRINCIPAL ();
// Menú de pausa superpuesto al juego: CONTINUAR, GUARDAR Y SALIR, SALIR SIN GUARDAR
ePausaResultado MENU_PAUSA ();
// Submenú de configuración: paleta, resolución, velocidad, modo, ancho de tablero
void MENU_OPCIONES ();
// Pantalla de ranking de puntajes (top 5)
void MENU_ESTADISTICAS ();
// Pantalla de instrucciones con tabla de controles
void MENU_INSTRUCCIONES ();

#endif // MENU_H_INCLUDED
