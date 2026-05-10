#ifndef TRABAJO_PRACTICO_TOPICOS_TETRIS_FUNCIONALIDADES_H
#define TRABAJO_PRACTICO_TOPICOS_TETRIS_FUNCIONALIDADES_H

#include "sprites.h"

#define filasTablero 20 // Define cantidad de filasTablero del tablero
#define columnasTablero 10 // Define cantidad de columnasTablero del tablero
#define MAX_FILAS_BORRAR 4

// Estructura para las piezas
typedef struct
{
    int forma [4][4]; // Matriz de forma de la pieza
    int fila, columna;
    int color;
    int rotacion; // Estado de rotación (0-3)
    int tipo;     // Tipo de pieza (0-6)
} sPieza;

// Estados del juego
typedef enum {
    ESTADO_RUNNING,
    ESTADO_PAUSED,
    ESTADO_GAMEOVER
} eEstadoJuego;

// Variables globales - extern permite que se compartan las variables globales entre archivos
extern int tablero [filasTablero][columnasTablero]; // Define el tablero
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
extern int filas_a_borrar[MAX_FILAS_BORRAR];
extern int cant_filas_borrar;
extern int animacion_borrado_activa;
extern int animacion_frame;

void LLENARBOLSA ();
int OBTENERPIEZABOLSA ();
void INICIALIZARPIEZA (sPieza *p, int tipo);
void REINICIARJUEGO (); // Reinicia los valores del juego
void NUEVAPIEZA (); // Elige una nueva pieza para que caiga en el tablero
void COPIARPIEZA (int destino [4][4], int origen [4][4]); // Copia la forma de la pieza de un lado a otro
int COLISION (int filaNueva, int columnaNueva, int forma [4][4]); // Evalúa la colisión de la pieza que está cayendo
void FIJARPIEZA (); // Fija la pieza en el tablero
void LIMPIARLINEAS (); // Limpia una fila si se completa la línea
void ACTUALIZAR_ANIMACION_BORRADO();
void COLAPSAR_FILAS();
void APLICAR_ROTACION (int sentido); // 1 horario, -1 antihorario
void ROTARHORARIO ();
void ROTARANTIHORARIO ();

#endif //TRABAJO_PRACTICO_TOPICOS_TETRIS_FUNCIONALIDADES_H
