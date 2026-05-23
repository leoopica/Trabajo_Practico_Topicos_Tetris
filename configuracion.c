#include "configuracion.h"
#include "graficos.h"
#include <string.h>

#define NOMBRE_ARCHIVO_CONFIG "config.dat"

// Velocidades disponibles: segundos entre cada descenso automático de la pieza
float velocidades_disponibles [CANT_VELOCIDADES] = {1.0, 0.7, 0.4};
const char* nombres_velocidades [CANT_VELOCIDADES] = {"LENTA", "NORMAL", "RAPIDA"};

sConfig config_actual;

// Paletas de colores en RGB 24 bits (formato GBT_FORMATO_888)
// Cada paleta tiene 16 colores. Color 15 es transparente en GBT.
tGBT_ColorRGB paletas[CANT_PALETAS][16] =
{
    // PALETA 0: CGA - colores IBM CGA estándar (alta intensidad)
    {
        {0x00, 0x00, 0x00}, // 0:  Negro
        {0x00, 0x00, 0xAA}, // 1:  Azul
        {0x00, 0xAA, 0x00}, // 2:  Verde
        {0x00, 0xAA, 0xAA}, // 3:  Cian
        {0xAA, 0x00, 0x00}, // 4:  Rojo
        {0xAA, 0x00, 0xAA}, // 5:  Magenta
        {0xAA, 0x55, 0x00}, // 6:  Marrón
        {0xAA, 0xAA, 0xAA}, // 7:  Gris claro
        {0x55, 0x55, 0x55}, // 8:  Gris oscuro
        {0x55, 0x55, 0xFF}, // 9:  Azul brillante
        {0x55, 0xFF, 0x55}, // 10: Verde brillante
        {0x55, 0xFF, 0xFF}, // 11: Cian brillante
        {0xFF, 0x55, 0x55}, // 12: Rojo brillante
        {0xFF, 0x55, 0xFF}, // 13: Magenta brillante
        {0xFF, 0xFF, 0x55}, // 14: Amarillo
        {0xFF, 0xFF, 0xFF}, // 15: Blanco (transparente en GBT)
    },

    // PALETA 1: Game Boy (colores originales DMG: #0F380F, #306230, #8BAC0F, #9BBC0F)
    // Los colores se reparten entre los 16 índices para dar variación visual aún
    // cuando solo hay 4 tonos reales. 8 y 14 comparten #306230, por ejemplo.
    {
        {0x0F, 0x38, 0x0F}, // 0:  Negro GB (fondo vacío / transparente)
        {0x09, 0x21, 0x09}, // 1:  Verde oscuro GB (fondo de pantalla)
        {0x30, 0x62, 0x30}, // 2:  Verde oscuro GB (pieza L — sombra)
        {0x8B, 0xAC, 0x0F}, // 3:  Verde medio GB  (pieza O)
        {0x30, 0x62, 0x30}, // 4:  Verde oscuro GB (pieza S — sombra)
        {0x8B, 0xAC, 0x0F}, // 5:  Verde medio GB  (pieza T)
        {0x9B, 0xBC, 0x0F}, // 6:  Verde claro GB  (pieza L — base)
        {0x9B, 0xBC, 0x0F}, // 7:  Verde claro GB  (marco brillo / texto)
        {0x30, 0x62, 0x30}, // 8:  Verde oscuro GB (marco sombra / grilla)
        {0x9B, 0xBC, 0x0F}, // 9:  Verde claro GB  (pieza J — igual que L)
        {0x8B, 0xAC, 0x0F}, // 10: Verde medio GB  (pieza S — base)
        {0x9B, 0xBC, 0x0F}, // 11: Verde claro GB  (pieza I — base)
        {0x8B, 0xAC, 0x0F}, // 12: Verde medio GB  (pieza Z — base)
        {0x9B, 0xBC, 0x0F}, // 13: Verde claro GB  (pieza T — brillo)
        {0x30, 0x62, 0x30}, // 14: Verde oscuro GB (pieza O — sombra)
        {0x9B, 0xBC, 0x0F}, // 15: Verde claro GB  (transparente GBT)
    },

    // PALETA 2: Atari - colores vibrantes estilo Atari 2600/7800
    {
        {0x0D, 0x0D, 0x1A}, // 0: Negro azulado
        {0x1A, 0x00, 0x4D}, // 1: Violeta oscuro
        {0x7B, 0x00, 0x7B}, // 2: Púrpura
        {0xB5, 0x35, 0x00}, // 3: Naranja oscuro
        {0xCC, 0x00, 0x00}, // 4: Rojo intenso
        {0xFF, 0x22, 0x00}, // 5: Rojo naranja
        {0xFF, 0x77, 0x00}, // 6: Naranja
        {0xFF, 0xBB, 0x55}, // 7: Amarillo anaranjado
        {0x33, 0x00, 0x66}, // 8: Violeta muy oscuro
        {0xAA, 0x00, 0xFF}, // 9: Violeta brillante
        {0xFF, 0x00, 0x88}, // 10: Rosa fucsia
        {0xFF, 0x44, 0xCC}, // 11: Rosa claro
        {0xFF, 0x66, 0x00}, // 12: Naranja brillante
        {0xFF, 0xDD, 0x00}, // 13: Amarillo brillante
        {0xFF, 0xFF, 0x99}, // 14: Amarillo claro
        {0xFF, 0xFF, 0xFF}, // 15: Blanco (transparente)
    },
};

void CONFIG_DEFAULTS (sConfig *c)
{
    // Valores por defecto: CGA 320×200, escala 2, paleta CGA, velocidad LENTA, modo CLASICO, ancho 10
    c->resolucion = RESOLUCION_CGA;
    c->escala = 2;
    c->paleta = PALETA_CGA;
    c->velocidad_inicial = 0;
    c->modo_juego = MODO_CLASICO;
    c->ancho_tablero = 10;
}

int CONFIG_GUARDAR (const sConfig *c)
{
    // Abre (o crea) config.dat en escritura binaria y vuelca toda la estructura sConfig
    FILE *f = fopen (NOMBRE_ARCHIVO_CONFIG, "wb");
    if (!f) return -1;

    fwrite(c, sizeof (sConfig), 1, f);
    fclose(f);
    return 0;
}

int CONFIG_CARGAR (sConfig *c)
{
    size_t leidos;
    FILE *f = fopen(NOMBRE_ARCHIVO_CONFIG, "rb");
    if (!f) return -1;

    leidos = fread (c, sizeof (sConfig), 1, f);
    fclose (f);
    if (leidos != 1) return -1;

    // Validación de cada campo contra valores fuera de rango (archivo corrupto o de otra versión)
    if (c->resolucion < 0 || c->resolucion > 1)
        c->resolucion = RESOLUCION_CGA;
    if (c->escala < 1 || c->escala > 4)
        c->escala = 2;
    if (c->paleta < 0 || c->paleta >= CANT_PALETAS)
        c->paleta = PALETA_CGA;
    if (c->velocidad_inicial < 0 || c->velocidad_inicial >= CANT_VELOCIDADES)
        c->velocidad_inicial = 0;
    if (c->modo_juego < 0 || c->modo_juego > 1)
        c->modo_juego = MODO_CLASICO;
    if (c->ancho_tablero < 8 || c->ancho_tablero > MAX_COLUMNAS)
        c->ancho_tablero = 10;

    return 0;
}

void CONFIG_APLICAR (const sConfig *c)
{
    char nombreVentana [64];

    // Aplica la paleta seleccionada a GBT
    gbt_aplicar_paleta ((tGBT_ColorRGB*)paletas [c->paleta], 16, GBT_FORMATO_888);

    // Establece la velocidad de caída inicial
    duracion_caida = velocidades_disponibles[c->velocidad_inicial];

    // Sincroniza el ancho del tablero con la configuración
    columnasTablero = c->ancho_tablero;

    // Recrea la ventana GBT con la nueva resolución y escala
    sprintf (nombreVentana, "Tetris %dx%d", CONFIG_ANCHO (), CONFIG_ALTO ());
    gbt_destruir_ventana();
    gbt_crear_ventana (nombreVentana, CONFIG_ANCHO (), CONFIG_ALTO (), c->escala);

    // Vuelve a aplicar paleta (gbt_crear_ventana resetea los colores por defecto)
    gbt_aplicar_paleta ((tGBT_ColorRGB*)paletas [c->paleta], 16, GBT_FORMATO_888);
}

int CONFIG_TAM_MINO ()
{
    // Retorna el tamaño en píxeles de cada celda (mino): 8 en CGA, 16 en VGA
    return (config_actual.resolucion == RESOLUCION_VGA) ? 16 : 8;
}

int CONFIG_ANCHO ()
{
    // Retorna el ancho lógico de la ventana según la resolución
    return (config_actual.resolucion == RESOLUCION_VGA) ? ANCHO_VGA : ANCHO_CGA;
}

int CONFIG_ALTO ()
{
    // Retorna el alto lógico de la ventana según la resolución
    return (config_actual.resolucion == RESOLUCION_VGA) ? ALTO_VGA : ALTO_CGA;
}