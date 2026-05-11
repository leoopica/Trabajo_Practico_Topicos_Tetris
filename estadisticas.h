#ifndef ESTADISTICAS_H_INCLUDED
#define ESTADISTICAS_H_INCLUDED

#define MAX_STATS 5
#define MAX_NOMBRE_STAT 21

// Estructura de una entrada de estadísticas
typedef struct {
    char nombre[MAX_NOMBRE_STAT];
    int  puntaje;
} tEstadistica;

/**
 * @brief Carga las estadísticas desde el archivo, ordenadas de mayor a menor puntaje.
 *
 * @param stats  Array de tEstadistica donde se guardan los resultados.
 * @param cant   Cantidad máxima a cargar (máximo MAX_STATS).
 * @return int   Cantidad de entradas cargadas.
 */
int STATS_CARGAR (tEstadistica *stats, int cant);

/**
 * @brief Guarda una nueva entrada de estadísticas.
 *        Si ya hay MAX_STATS entradas, reemplaza la de menor puntaje si corresponde.
 *
 * @param nombre  Nombre del jugador.
 * @param puntaje Puntaje obtenido.
 */
void STATS_GUARDAR (const char *nombre, int puntaje);

#endif // ESTADISTICAS_H_INCLUDED
