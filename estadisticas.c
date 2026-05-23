#include "estadisticas.h"
#include <stdio.h>
#include <string.h>

#define NOMBRE_ARCHIVO_STATS "estadisticas.dat"

void STATS_GUARDAR(const char *nombre, int puntaje)
{
    tEstadistica stats [MAX_STATS];
    int cant = STATS_CARGAR (stats, MAX_STATS); // Carga estadísticas existentes del archivo
    int indiceMinimo = 0;

    if (cant < MAX_STATS)
    {
        // Hay espacio libre: agrega la entrada al final del array
        strncpy (stats[cant].nombre, nombre, MAX_NOMBRE_STAT - 1);
        stats [cant].nombre [MAX_NOMBRE_STAT - 1] = '\0';
        stats [cant].puntaje = puntaje;
        cant ++;
    }
    else
    {
        // Ranking lleno: busca la entrada de menor puntaje
        for (int i = 1; i < cant; i++)
            if (stats [i].puntaje < stats[indiceMinimo].puntaje)
                indiceMinimo = i;

        // Solo reemplaza si el nuevo puntaje supera al mínimo actual
        if (puntaje > stats [indiceMinimo].puntaje)
        {
            strncpy (stats [indiceMinimo].nombre, nombre, MAX_NOMBRE_STAT - 1);
            stats [indiceMinimo].nombre[MAX_NOMBRE_STAT - 1] = '\0';
            stats [indiceMinimo].puntaje = puntaje;
        }
    }

    // Ordena el array de mayor a menor puntaje (burbujeo)
    for (int i = 0; i < cant - 1; i++)
        for (int j = 0; j < cant - 1 - i; j++)
            if (stats[j].puntaje < stats[j + 1].puntaje)
            {
                tEstadistica tmp = stats[j];
                stats[j] = stats[j + 1];
                stats[j + 1] = tmp;
            }

    // Sobrescribe el archivo con el ranking actualizado
    FILE *f = fopen(NOMBRE_ARCHIVO_STATS, "wb");
    if (!f) return;
    fwrite(stats, sizeof(tEstadistica), cant, f);
    fclose(f);
}

int STATS_CARGAR(tEstadistica *stats, int cant)
{
    // Abre el archivo en lectura binaria; si no existe (primera vez), retorna 0
    FILE *f = fopen(NOMBRE_ARCHIVO_STATS, "rb");
    if (!f) return 0;

    int leidos = (int)fread(stats, sizeof(tEstadistica), cant, f);
    fclose(f);
    return leidos; // Retorna la cantidad de entradas leídas
}