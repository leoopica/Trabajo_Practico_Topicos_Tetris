#include "configuracion.h"
#include <stdio.h>
#include "graficos.h"

// Nombre del archivo donde se guarda la configuración
#define NOMBRE_ARCHIVO_CONFIG "config.dat"

// Velocidades disponibles: segundos entre cada descenso automático de la pieza
float velocidades_disponibles [CANT_VELOCIDADES] = {1.0, 0.7, 0.4};
const char* nombres_velocidades [CANT_VELOCIDADES] = {"LENTA", "NORMAL", "RAPIDA"};

// Variable que almacena la configuración activa del juego
sConfig config_actual;

// Paletas de colores
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
        {0xFF, 0xFF, 0xFF}, // 15: Blanco (transparente para GBT)
    },

    // PALETA 1: Game Boy
    {
        {0x0F, 0x38, 0x0F}, // 0:  Negro GB (fondo vacío / transparente)
        {0x09, 0x21, 0x09}, // 1:  Verde oscuro GB (fondo de pantalla)
        {0x30, 0x62, 0x30}, // 2:  GB (pieza L — sombra)
        {0x8B, 0xAC, 0x0F}, // 3:  GB (pieza O)
        {0x30, 0x62, 0x30}, // 4:  GB (pieza S — sombra)
        {0x8B, 0xAC, 0x0F}, // 5:  GB (pieza T)
        {0x9B, 0xBC, 0x0F}, // 6:  GB (pieza L — base)
        {0x9B, 0xBC, 0x0F}, // 7:  GB (marco brillo / texto)
        {0x30, 0x62, 0x30}, // 8:  GB (marco sombra / grilla)
        {0x9B, 0xBC, 0x0F}, // 9:  GB (pieza J — igual que L)
        {0x8B, 0xAC, 0x0F}, // 10: GB (pieza S — base)
        {0x9B, 0xBC, 0x0F}, // 11: GB (pieza I — base)
        {0x8B, 0xAC, 0x0F}, // 12: GB (pieza Z — base)
        {0x9B, 0xBC, 0x0F}, // 13: GB (pieza T — brillo)
        {0x30, 0x62, 0x30}, // 14: GB (pieza O — sombra)
        {0x9B, 0xBC, 0x0F}, // 15: GB (transparente GBT)
    },

    // PALETA 2: Atari
    {
        {0x0D, 0x0D, 0x1A}, // 0:  Negro azulado
        {0x1A, 0x00, 0x4D}, // 1:  Violeta oscuro
        {0x7B, 0x00, 0x7B}, // 2:  Púrpura
        {0xB5, 0x35, 0x00}, // 3:  Naranja oscuro
        {0xCC, 0x00, 0x00}, // 4:  Rojo intenso
        {0xFF, 0x22, 0x00}, // 5:  Rojo naranja
        {0xFF, 0x77, 0x00}, // 6:  Naranja
        {0xFF, 0xBB, 0x55}, // 7:  Amarillo anaranjado
        {0x33, 0x00, 0x66}, // 8:  Violeta muy oscuro
        {0xAA, 0x00, 0xFF}, // 9:  Violeta brillante
        {0xFF, 0x00, 0x88}, // 10: Rosa fucsia
        {0xFF, 0x44, 0xCC}, // 11: Rosa claro
        {0xFF, 0x66, 0x00}, // 12: Naranja brillante
        {0xFF, 0xDD, 0x00}, // 13: Amarillo brillante
        {0xFF, 0xFF, 0x99}, // 14: Amarillo claro
        {0xFF, 0xFF, 0xFF}, // 15: Blanco (transparente)
    },
};

// Asigna los valores por defecto a una estructura sConfig
void CONFIG_DEFAULTS (sConfig *c)
{
    c->resolucion = RESOLUCION_CGA;
    c->escala = 2;
    c->paleta = PALETA_CGA;
    c->velocidad_inicial = 0;
    c->modo_juego = MODO_CLASICO;
    c->ancho_tablero = 10;
}

// Guarda la configuración en disco como archivo binario. 0 si OK, -1 si no pudo abrir el archivo
int CONFIG_GUARDAR (const sConfig *c)
{
    FILE *f = fopen (NOMBRE_ARCHIVO_CONFIG, "wb");
    if (!f)
    {
        return -1;  
    }

    fwrite (c, sizeof (sConfig), 1, f);
    fclose(f);
    
    return 0;
}

// Carga la configuración desde config.dat. 0 si OK, -1 si el archivo no existe o hubo error de lectura
int CONFIG_CARGAR (sConfig *c)
{
    size_t leidos;
    FILE *f = fopen (NOMBRE_ARCHIVO_CONFIG, "rb");
    if (!f)
    {
        return -1;
    }

    leidos = fread (c, sizeof (sConfig), 1, f);
    fclose (f);
    if (leidos != 1)
    {
        return -1;
    }

    // Validación de cada campo. Si está fuera de rango, asigna valor por defecto
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
    if (c->modo_juego < 0 || c->modo_juego > 1)
    {
        c->modo_juego = MODO_CLASICO;
    }
    if (c->ancho_tablero < 8 || c->ancho_tablero > MAX_COLUMNAS)
    {
        c->ancho_tablero = 10;
    }

    return 0;
}

// Aplica  configuración al juego
void CONFIG_APLICAR (const sConfig *c)
{
    char nombreVentana [64]; // Título de la ventana
    
    gbt_aplicar_paleta ((tGBT_ColorRGB*)paletas [c->paleta], 16, GBT_FORMATO_888); // Paleta de colores a la ventana GBT
    
    duracion_caida = velocidades_disponibles[c->velocidad_inicial]; // Velocidad de descenso de la pieza
    
    columnasTablero = c->ancho_tablero; // Ancho del tablero

    sprintf (nombreVentana, "Tetris %dx%d", CONFIG_ANCHO (), CONFIG_ALTO ()); 
    gbt_destruir_ventana(); // Cierra la ventana actual
    gbt_crear_ventana (nombreVentana, CONFIG_ANCHO (), CONFIG_ALTO (), c->escala); // Recrea la ventana con nueva resolución

    gbt_aplicar_paleta ((tGBT_ColorRGB*)paletas [c->paleta], 16, GBT_FORMATO_888); // Reaplica la paleta
}

// Retorna el tamaño en píxeles de cada mino. 8 píxeles en CGA, 16 píxeles en VGA
int CONFIG_TAM_MINO ()
{   
    int tam_pixel = 0;
    if (config_actual.resolucion == RESOLUCION_VGA)
    {
        tam_pixel = 16;
    }
    else 
    {
        tam_pixel = 8;
    }
    return tam_pixel;
}

// Retorna el ancho de la ventana según la resolución activa
int CONFIG_ANCHO ()
{
    int ancho = 0;
    if (config_actual.resolucion == RESOLUCION_VGA)
    {
        ancho = ANCHO_VGA;
    }
    else 
    {
        ancho = ANCHO_CGA;
    }
    return ancho;
}

// Retorna el alto de la ventana según la resolución activa
int CONFIG_ALTO ()
{
    int alto = 0;
    if (config_actual.resolucion == RESOLUCION_VGA)
    {
        alto = ALTO_VGA;
    }
    else 
    {
        alto = ALTO_CGA;
    }
    return alto;
}
