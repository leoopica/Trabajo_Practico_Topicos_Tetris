#include "archivos.h"
#include "configuracion.h"
#include <stdio.h>
#include <string.h>

extern int bolsa[MAX_PIEZAS];
extern int bolsaIndice;

int PARTIDA_EXISTE(void)
{
    // Abre partida.dat en modo lectura binaria
    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "rb");
    if (!f) return 0; // No existe el archivo

    sEstadoPartida estado;
    int leidos = (int)fread(&estado, sizeof(sEstadoPartida), 1, f);
    fclose(f);

    return (leidos == 1 && estado.valido == 1);
}

int PARTIDA_GUARDAR(void)
{
    sEstadoPartida estado;

    // Copia cada celda del tablero (todas las filas físicas, incluidas las invisibles)
    for (int f = 0; f < FILAS_TOTALES; f++)
        for (int c = 0; c < columnasTablero; c++)
            estado.tablero[f][c] = tablero[f][c];

    estado.actual        = actual;
    estado.proxima       = proxima;
    estado.puntaje       = puntaje;
    estado.nivel         = nivel;
    estado.lineas_totales= lineas_totales;
    estado.piezas_caidas = piezas_caidas;
    estado.duracion_caida= duracion_caida;
    strncpy(estado.nombreJugador, nombreJugador, 21);
    estado.nombreJugador[20] = '\0';

    // Guarda el estado de la bolsa de piezas (7 clásico / 11 DX)
    for (int i = 0; i < piezas_en_uso; i++)
        estado.bolsa[i] = bolsa[i];
    estado.bolsaIndice = bolsaIndice;

    estado.valido = 1;
    estado.cantidad_piezas = piezas_en_uso;
    estado.ancho_tablero = columnasTablero;
    estado.modo_juego = config_actual.modo_juego;

    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "wb");
    if (!f) return -1; // No se pudo crear el archivo

    int ok = (int)fwrite(&estado, sizeof(sEstadoPartida), 1, f);
    fclose(f);
    return (ok == 1) ? 0 : -1;
}

int PARTIDA_CARGAR(void)
{
    sEstadoPartida estado;
    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "rb");
    if (!f) return -1;

    int leidos = (int)fread(&estado, sizeof(sEstadoPartida), 1, f);
    fclose(f);

    if (leidos != 1 || estado.valido != 1) return -1;

    // Inicializa punteros del tablero dinámico (filas_tablero apunta a celdas_tablero)
    for (int fi = 0; fi < FILAS_TOTALES; fi++)
        filas_tablero[fi] = celdas_tablero[fi];
    tablero = filas_tablero;

    // Restaura cada celda del tablero (todas las filas, incluidas las invisibles)
    for (int fi = 0; fi < FILAS_TOTALES; fi++)
        for (int c = 0; c < columnasTablero; c++)
            tablero[fi][c] = estado.tablero[fi][c];

    actual         = estado.actual;
    proxima        = estado.proxima;
    puntaje        = estado.puntaje;
    nivel          = estado.nivel;
    lineas_totales = estado.lineas_totales;
    piezas_caidas  = estado.piezas_caidas;
    duracion_caida = estado.duracion_caida;
    strncpy(nombreJugador, estado.nombreJugador, 21);
    nombreJugador[20] = '\0';

    // Restaura la bolsa, validando la cantidad de piezas
    int piezas_guardadas = estado.cantidad_piezas;
    if (piezas_guardadas < 7 || piezas_guardadas > MAX_PIEZAS)
        piezas_guardadas = piezas_en_uso;
    for (int i = 0; i < piezas_guardadas; i++)
        bolsa[i] = estado.bolsa[i];
    bolsaIndice = estado.bolsaIndice;

    // Actualiza la cantidad de piezas activas según la partida guardada
    piezas_en_uso = piezas_guardadas;

    // Restaura el ancho guardado (solo si está en rango válido)
    if (estado.ancho_tablero >= 8 && estado.ancho_tablero <= MAX_COLUMNAS)
        columnasTablero = estado.ancho_tablero;

    // Restaura el modo de juego guardado para mantener circularidad del tablero
    if (estado.modo_juego == MODO_CLASICO || estado.modo_juego == MODO_DX)
        config_actual.modo_juego = estado.modo_juego;

    estado_juego = ESTADO_RUNNING; // Marca el estado como en ejecución
    return 0;
}

void PARTIDA_BORRAR(void)
{
    // Sobrescribe el archivo con una estructura en cero (valido=0)
    sEstadoPartida estado = {0};
    FILE *f = fopen(NOMBRE_ARCHIVO_PARTIDA, "wb");
    if (f)
    {
        fwrite(&estado, sizeof(sEstadoPartida), 1, f);
        fclose(f);
    }
}