#include "estadisticas.h"
#include <stdio.h>
#include <string.h>

#define NOMBRE_ARCHIVO_STATS "estadisticas.dat" // Nombre del archivo donde se guardan las estadísticas

void STATS_GUARDAR(const char *nombre, int puntaje)
{
    tEstadistica stats [MAX_STATS]; // Vector donde se cargan las estadísticas existentes
    int cant = STATS_CARGAR (stats, MAX_STATS); // Carga las estadísticas del archivo y guarda cuántas había
    int indiceMinimo = 0; // Índice del menor puntaje. Asume que la primera entrada es la menor

    if (cant < MAX_STATS) // Evalúa si hay lugar disponible en el ranking (menos de 5 entradas)
    {
        // Copia el nombre en la siguiente posición libre del array
        strncpy (stats[cant].nombre, nombre, MAX_NOMBRE_STAT - 1);
        stats [cant].nombre [MAX_NOMBRE_STAT - 1] = '\0';
        stats [cant].puntaje = puntaje; // Guarda el puntaje en la nueva entrada
        cant ++; // Aumenta la cantidad de entradas
    }
    else // Si el ranking ya está lleno (5 entradas)
    {
        // Busca la entrada con el menor puntaje para ver si vale la pena reemplazarla
        for (int i = 1; i < cant; i++) // Recorre el resto de las entradas
            if (stats [i].puntaje < stats[indiceMinimo].puntaje) // Si encuentra una con menor puntaje
            {
                indiceMinimo = i; // Actualiza el índice del mínimo
            }

        if (puntaje > stats [indiceMinimo].puntaje) // Solo reemplaza si el nuevo puntaje supera al mínimo
        {
            strncpy (stats [indiceMinimo].nombre, nombre, MAX_NOMBRE_STAT - 1); // Reemplaza el nombre
            stats [indiceMinimo].nombre[MAX_NOMBRE_STAT - 1] = '\0'; // Asegura el fin de string
            stats [indiceMinimo].puntaje = puntaje; // Reemplaza el puntaje
        }
    }

    // Ordena el array de mayor a menor puntaje usando burbujeo
    for (int i = 0; i < cant - 1; i++) // Recorre el array cant-1 veces
        for (int j = 0; j < cant - 1 - i; j++) // En cada pasada, el mayor va "flotando" al final
            if (stats[j].puntaje < stats[j + 1].puntaje) // Si el elemento actual es menor que el siguiente
            {
                tEstadistica tmp = stats[j]; // Guarda el elemento actual en una variable temporal
                stats[j] = stats[j + 1];    // Lo reemplaza por el siguiente
                stats[j + 1] = tmp;          // Pone el temporal en la posición del siguiente
            }

    FILE *f = fopen(NOMBRE_ARCHIVO_STATS, "wb"); // Abre el archivo en modo escritura binaria (lo sobreescribe)
    if (!f) return; // Si no pudo abrir el archivo, sale sin guardar
    fwrite(stats, sizeof(tEstadistica), cant, f); // Escribe todas las entradas en el archivo
    fclose(f); // Cierra el archivo
}

int STATS_CARGAR(tEstadistica *stats, int cant)
{
    FILE *f = fopen(NOMBRE_ARCHIVO_STATS, "rb"); // Abre el archivo en modo lectura binaria
    if (!f) return 0; // Si el archivo no existe (primera ejecución), retorna 0 entradas
    int leidos = (int)fread(stats, sizeof(tEstadistica), cant, f); // Lee hasta 'cant' entradas del archivo
    fclose(f); // Cierra el archivo
    return leidos; // Retorna cuántas entradas se leyeron efectivamente
}