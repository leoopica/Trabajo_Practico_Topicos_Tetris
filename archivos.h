#ifndef ARCHIVOS_H_INCLUDED
#define ARCHIVOS_H_INCLUDED

#include "funcionalidades.h"

// Nombre del archivo donde se guarda la partida en disco
#define NOMBRE_ARCHIVO_PARTIDA "partida.dat"

// Estructura del estado completo de una partida guardada
typedef struct {
    int tablero [FILAS_TOTALES][MAX_COLUMNAS]; // 24 filas (20 visibles + 4 ocultas)
    sPieza actual; // Pieza que se está controlando actualmente
    sPieza proxima; // Siguiente pieza
    int puntaje; // Puntaje acumulado
    int nivel; // Nivel actual
    int lineas_totales; // Total de líneas completadas en la partida
    int piezas_caidas; // Contador de piezas que cayeron
    double duracion_caida; // Tiempo entre cada descenso automático
    char nombreJugador [21]; // Nombre del jugador
    int bolsa [MAX_PIEZAS]; // Estado de la bolsa de piezas
    int bolsaIndice; // Índice actual dentro de la bolsa
    int valido; // 1 = partida válida, 0 = inválida
    int cantidad_piezas; // 7 clásico, 11 DX
    int ancho_tablero; // Ancho real al guardar
    int modo_juego; // MODO_CLASICO o MODO_DX
} sEstadoPartida;

int PARTIDA_EXISTE ();  // Verifica si existe partida guardada. 1 si hay partida guardada válida, 0 si no
int PARTIDA_GUARDAR (); // Guarda la partida actual. 0 = éxito, -1 = error
int PARTIDA_CARGAR ();  // Carga una partida guardada. 0 = éxito, -1 = error/no existe
void PARTIDA_BORRAR (); // Marca la partida como inválida y la borra
#endif // ARCHIVOS_H_INCLUDED
