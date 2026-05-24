#ifndef ESTADISTICAS_H_INCLUDED
#define ESTADISTICAS_H_INCLUDED

#define MAX_STATS 5 // Cantidad máxima puntajes en ranking
#define MAX_NOMBRE_STAT 21 // Largo nombre jugador

// Estructura de puntaje en ranking
typedef struct {
    char nombre[MAX_NOMBRE_STAT];
    int  puntaje;
} tEstadistica;

int STATS_CARGAR (tEstadistica *stats, int cant); // Carga estadísticas. stats: Array donde se guardan los resultados, cant: Cantidad máxima a cargar
void STATS_GUARDAR (const char *nombre, int puntaje); // Guarda una nueva entrada de estadísticas. Si ya hay MAX_STATS entradas, reemplaza la de menor puntaje si el nuevo es mayor. nombre: Nombre del jugador, puntaje: Puntaje obtenido

#endif // ESTADISTICAS_H_INCLUDED
