#include "estadisticas.h"
#include <stdio.h>
#include <string.h>

#define NOMBRE_ARCHIVO_STATS "estadisticas.dat" // Nombre del archivo donde se guardan estadísticas

// Guarda un nuevo puntaje en el ranking
void STATS_GUARDAR (const char *nombre, int puntaje)
{
    tEstadistica stats [MAX_STATS], tmp;
    int cant, indiceMinimo = 0, i, j;
    FILE *f;

    cant = STATS_CARGAR (stats, MAX_STATS); // Carga estadísticas existentes del archivo. 

    if (cant < MAX_STATS) // Si hay espacio libre en el ranking, agrega la nueva entrada al final del array. Si está completo, busca la entrada con el puntaje más bajo
    {
        strncpy (stats[cant].nombre, nombre, MAX_NOMBRE_STAT - 1); // Copia nombre
        stats [cant].nombre [MAX_NOMBRE_STAT - 1] = '\0';
        stats [cant].puntaje = puntaje;
        cant ++;
    }
    else
    {
        for (i = 1; i < cant; i++)
            if (stats [i].puntaje < stats [indiceMinimo].puntaje)
            {
                indiceMinimo = i;
            }

        
        if (puntaje > stats [indiceMinimo].puntaje) // Si el nuevo puntaje supera al mínimo actual, lo reemplaza 
        {
            strncpy (stats [indiceMinimo].nombre, nombre, MAX_NOMBRE_STAT - 1);
            stats [indiceMinimo].nombre[MAX_NOMBRE_STAT - 1] = '\0';
            stats [indiceMinimo].puntaje = puntaje;
        }
    }

    // Ordena el array de mayor a menor puntaje
    for (i = 0; i < cant - 1; i++)
        for (j = 0; j < cant - 1 - i; j++)
            if (stats [j].puntaje < stats [j + 1].puntaje)
            {
                tmp = stats [j];
                stats [j] = stats [j + 1];
                stats [j + 1] = tmp;
            }

    // Sobrescribe el archivo con el ranking actualizado
    f = fopen(NOMBRE_ARCHIVO_STATS, "wb");
    if (!f)
    {
        return;
    }
    fwrite (stats, sizeof (tEstadistica), cant, f);
    fclose(f);
}

// Carga las estadísticas desde el archivo estadisticas.dat
int STATS_CARGAR (tEstadistica *stats, int cant)
{
    FILE *f;
    int leidos;

    f = fopen(NOMBRE_ARCHIVO_STATS, "rb");
    if (!f)
    {
        return 0;
    }

    leidos = (int) fread (stats, sizeof (tEstadistica), cant, f);
    fclose(f);
    return leidos;
}