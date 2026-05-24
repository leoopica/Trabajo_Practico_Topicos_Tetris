#ifndef FUNCIONALIDADES_H_INCLUDED
#define FUNCIONALIDADES_H_INCLUDED

#include "sprites.h"

#define filasTablero 20 // Filas VISIBLES del tablero
#define FILAS_INVISIBLES 4 // Filas ocultas arriba
#define FILAS_TOTALES (filasTablero + FILAS_INVISIBLES) // 24 filas físicas
#define MAX_COLUMNAS 16
#define MAX_FILAS_BORRAR 4

// Estructura para las piezas
typedef struct
{
    int forma [4][4];
    int fila, columna;
    int color;
    int rotacion;
    int tipo;
} sPieza;

// Estados del juego
typedef enum {
    ESTADO_RUNNING,
    ESTADO_PAUSED,
    ESTADO_GAMEOVER
} eEstadoJuego;

extern int columnasTablero;
// Variables globales
extern int *filas_tablero [FILAS_TOTALES]; // Array de punteros (uno por fila física)
extern int celdas_tablero[FILAS_TOTALES][MAX_COLUMNAS]; // Memoria real
extern int **tablero; // Puntero al array de punteros (acceso como tablero[f][c])
extern sPieza actual;
extern sPieza proxima;
extern int puntaje;
extern int nivel;
extern int lineas_totales;
extern int piezas_caidas;
extern double duracion_caida;
extern int velocidad;
extern eEstadoJuego estado_juego;
extern char nombreJugador [21];
extern int filas_a_borrar [MAX_FILAS_BORRAR];
extern int cant_filas_borrar;
extern int animacion_borrado_activa;
extern int animacion_frame;
extern int piezas_en_uso;
extern int cheat_activo;
extern double cheat_tiempo_restante;
extern double cheat_cooldown_restante;

void LLENARBOLSA (); // Llena la bolsa con índices 0 / piezas_en_uso-1 y los desordena
int OBTENERPIEZABOLSA (); // Retorna el siguiente tipo de pieza de la bolsa y rellena la bolsa si está vacía
void INICIALIZARPIEZA (sPieza *p, int tipo); // Inicializa una pieza copiando su forma, centrándola arriba del tablero y asignando color
void REINICIARJUEGO (); // Reinicia los valores del juego
void NUEVAPIEZA (); // Elige una nueva pieza para que caiga en el tablero
void COPIARPIEZA (int destino [4][4], int origen [4][4]); // Copia la forma de la pieza de un lado a otro
int COLISION (int filaNueva, int columnaNueva, int forma [4][4]); // Evalúa la colisión de la pieza que está cayendo
void FIJARPIEZA (); // Fija la pieza en el tablero
void LIMPIARLINEAS (); // Limpia una fila si se completa la línea
void ACTUALIZAR_ANIMACION_BORRADO(); // Anima el borrado desde el centro hacia los bordes
void COLAPSAR_FILAS(); // Reordena punteros para que filas vacías queden arriba y llenas abajo
void APLICAR_ROTACION (int sentido); // Rota la pieza con wall kicks: 1 = horario, -1 = antihorario
void ROTARHORARIO ();
void ROTARANTIHORARIO ();

#endif // FUNCIONALIDADES_H_INCLUDED