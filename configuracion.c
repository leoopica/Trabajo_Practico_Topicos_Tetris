#include "configuracion.h"
#include "graficos.h"
#include <string.h>

#define NOMBRE_ARCHIVO_CONFIG "config.dat" // Nombre del archivo donde se guarda la configuración

float velocidades_disponibles [CANT_VELOCIDADES] = {1.0, 0.7, 0.4};
const char* nombres_velocidades [CANT_VELOCIDADES] = {"LENTA", "NORMAL", "RAPIDA"};

sConfig config_actual; // Configuración activa

// PALETAS
tGBT_ColorRGB paletas[CANT_PALETAS][16] =
{
    // PALETA 0: CGA
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
        {0xFF, 0xFF, 0xFF}, // 15: Blanco (transparente)
    },

    // PALETA 1: Game Boy (colores originales DMG)
    // Los 4 tonos originales son: #0F380F, #306230, #8BAC0F, #9BBC0F
    {
        {0x0F, 0x38, 0x0F}, // 0:  Negro GB (fondo vacío / transparente)
        {0x09, 0x21, 0x09}, // 1:  Verde oscuro GB (fondo de pantalla)
        {0x30, 0x62, 0x30}, // 2:  Verde oscuro GB (pieza L — sombra)
        {0x8B, 0xAC, 0x0F}, // 3:  Verde medio GB  (pieza O)
        {0x30, 0x62, 0x30}, // 4:  Verde oscuro GB (pieza S — sombra)
        {0x8B, 0xAC, 0x0F}, // 5:  Verde medio GB  (pieza T)
        {0x9B, 0xBC, 0x0F}, // 6:  Verde claro GB  (pieza L — base)
        {0x9B, 0xBC, 0x0F}, // 7:  Verde claro GB  (marco brillo / texto)
        {0x30, 0x62, 0x30}, // 8:  Negro GB        (marco sombra / grilla)
        {0x9B, 0xBC, 0x0F}, // 9:  Verde claro GB  (pieza J — igual que L)
        {0x8B, 0xAC, 0x0F}, // 10: Verde medio GB  (pieza S — base)
        {0x9B, 0xBC, 0x0F}, // 11: Verde claro GB  (pieza I — base)
        {0x8B, 0xAC, 0x0F}, // 12: Verde medio GB  (pieza Z — base)
        {0x9B, 0xBC, 0x0F}, // 13: Verde claro GB  (pieza T — brillo)
        {0x30, 0x62, 0x30}, // 14: Verde oscuro GB (pieza O — sombra)
        {0x9B, 0xBC, 0x0F}, // 15: Verde claro GB  (transparente GBT)
    },

    // PALETA 2: Atari
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

// FUNCIONES
void CONFIG_DEFAULTS (sConfig *c)
{
    c->resolucion = RESOLUCION_CGA;
    c->escala = 2;
    c->paleta = PALETA_CGA;
    c->velocidad_inicial = 0; // LENTA
}

int CONFIG_GUARDAR (const sConfig *c)
{
    FILE *f = fopen (NOMBRE_ARCHIVO_CONFIG, "wb"); // Abre config.dat
    if (!f) // Si falla la apertura, retorna -1
    {
        return -1;
    }
    fwrite(c, sizeof (sConfig), 1, f); // Escribe la configuración en config.dat
    fclose(f); // Cierra config.dat
    return 0; // Si funciona, retorna 0
}

int CONFIG_CARGAR (sConfig *c)
{
    size_t leidos;
    FILE *f = fopen(NOMBRE_ARCHIVO_CONFIG, "rb"); // Abre config.dat
    if (!f) // Si falla la apertura, retorna -1
    {
        return -1;
    }
    leidos = fread (c, sizeof (sConfig), 1, f); // Lee un elemento del tamañó de sConfig dentro del archivo, y lo guarda en la variable c
    fclose (f); // Cierra el archivo
    if (leidos != 1) // Si lee más de un elemento, retorna -1
    {
        return -1;
    }

    // Validación de valores por si el archivo está corrupto. Si algún valor está mal, pone el valor predeterminado
    if (c->resolucion < 0 || c->resolucion > 1)
    {
        c->resolucion = RESOLUCION_CGA;
    }
    if (c->escala < 1 || c->escala > 4)
    {
        c->escala = 2;
    }
    if (c->paleta < 0 || c->paleta >= CANT_PALETAS)
    {
        c->paleta = PALETA_CGA;
    }
    if (c->velocidad_inicial < 0 || c->velocidad_inicial >= CANT_VELOCIDADES)
    {
        c->velocidad_inicial = 0;
    }

    return 0;
}

void CONFIG_APLICAR (const sConfig *c)
{
    char nombreVentana [64];

    gbt_aplicar_paleta ((tGBT_ColorRGB*)paletas [c->paleta], 16, GBT_FORMATO_888); // Aplica la paleta seleccionada

    duracion_caida = velocidades_disponibles[c->velocidad_inicial]; // Aplica la velocidad inicial de caída

    // Recrea la ventana con la nueva resolución y escala
    sprintf (nombreVentana, "Tetris %dx%d", CONFIG_ANCHO (), CONFIG_ALTO ());
    gbt_destruir_ventana();
    gbt_crear_ventana (nombreVentana, CONFIG_ANCHO (), CONFIG_ALTO (), c->escala);

    // Re-aplica la paleta tras recrear la ventana
    gbt_aplicar_paleta ((tGBT_ColorRGB*)paletas [c->paleta], 16, GBT_FORMATO_888);
}

int CONFIG_TAM_MINO ()
{
    // CGA: 8px por mino, VGA: 16px por mino (doble escala)
    return (config_actual.resolucion == RESOLUCION_VGA) ? 16 : 8;
}
int CONFIG_ANCHO ()
{
    int ancho;
    if (config_actual.resolucion == RESOLUCION_VGA) // Si la resolución seleccionada es VGA, retorna el ancho VGA. Sino, retorna el ancho CGA
    {
        ancho = ANCHO_VGA;
    }
    else
    {
        ancho = ANCHO_CGA;
    }
    return ancho;
}

int CONFIG_ALTO ()
{
    int alto;
    if (config_actual.resolucion == RESOLUCION_VGA) // Si la resolución seleccionada es VGA, retorna el alto VGA. Sino, retorna el alto CGA
    {
        alto = ALTO_VGA;
    }
    else
    {
        alto = ALTO_CGA;
    }
    return alto;
}