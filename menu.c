#include "menu.h"
#include "estadisticas.h"
#include "inicio.h"
#include <string.h>
#include <stdio.h>

// Flag global: si se presiona Q en cualquier submenú, se activa para propagar la salida
static int g_salir_juego = 0;

// Nombres de paletas para mostrar en el menú
static const char* NOMBRES_PALETAS[CANT_PALETAS] = {
    "CGA",
    "GAME BOY",
    "ATARI",
};

static const char* NOMBRES_RESOLUCIONES[2] = {
    "CGA 320X200",
    "VGA 640X480",
};

// -------------------------------------------------------
// Helpers internos de dibujo
// -------------------------------------------------------

/**
 * @brief Dibuja una opción de menú centrada, con o sin selección resaltada.
 */
static void DIBUJAR_OPCION(int posY, const char *texto, int seleccionada)
{
    int ancho = CONFIG_ANCHO();
    int largo = (int)strlen(texto);
    int posX = (ancho - largo * anchoCaracter8) / 2;

    if (seleccionada)
    {
        // Fondo resaltado
        DIBUJAR_RECTANGULO(posX - 4, posY - 1, largo * anchoCaracter8 + 8, altoCaracter + 2, COLOR_AZUL);
        // Flechas indicadoras
        DIBUJARCARACTER(posX - 12, posY, 17, anchoCaracter8, COLOR_AMARILLO); // '>'  (R en la fuente, se puede ajustar)
        DIBUJARTEXTO(posX, posY, (char*)texto, anchoCaracter8);
    }
    else
    {
        DIBUJARTEXTO(posX, posY, (char*)texto, anchoCaracter8);
    }
}

/**
 * @brief Dibuja el logo de Tetris centrado en la parte superior.
 * El Y del logo se calcula para que quede proporcionalmente cerca de las opciones.
 */
static void DIBUJAR_CABECERA(void)
{
    int ancho = CONFIG_ANCHO();
    int alto  = CONFIG_ALTO();
    // El logo mide 167px de ancho (definido en inicio.c)
    int logoAncho = 167;
    int logoAlto  = 110; // aprox: 50 (top) + 50 (palo) + 10 (padding)
    int logoX = (ancho - logoAncho) / 2;
    // El logo ocupa desde logoY hasta logoY+logoAlto.
    // Las opciones arrancan en primerItemY = alto*0.55 aprox.
    // Centramos el logo en el tercio superior de la pantalla.
    int logoY = (alto / 2 - logoAlto) / 2;
    if (logoY < 3) logoY = 3;
    DIBUJAR_LOGO_COMPLETO(logoX, logoY);
}

// -------------------------------------------------------
// MENU PRINCIPAL
// -------------------------------------------------------

eMenuResultado MENU_PRINCIPAL(void)
{
    // Opciones del menú principal
    const char *opciones[] = {"JUGAR", "ESTADISTICAS", "INSTRUCCIONES", "OPCIONES", "SALIR"};
    const int CANT_OPCIONES = 5;
    int seleccion = 0;

    g_salir_juego = 0; // Resetear flag al entrar al menú principal
    while (1)
    {
        // Recalcular layout cada frame para adaptarse al cambio de resolución
        int alto = CONFIG_ALTO();
        // Las opciones arrancan justo debajo del logo (que termina ~alto/2)
        // primerItemY = justo después del logo, centrado en la mitad inferior
        int logoAlto  = 110; // altura aprox del logo completo
        int logoY     = (alto / 2 - logoAlto) / 2;
        if (logoY < 3) logoY = 3;
        int logoBottom = logoY + logoAlto;
        int areaDisponible = alto - logoBottom;
        int totalOpciones  = CANT_OPCIONES * 14; // estimado height opciones
        int primerItemY = logoBottom + (areaDisponible - totalOpciones) / 2;
        if (primerItemY < logoBottom + 4) primerItemY = logoBottom + 4;
        int separacion  = 14 + (alto - 200) / 20; // Separación mayor en VGA

        gbt_procesar_entrada();

        // Navegación
        if (gbt_tecla_presionada(GBTK_ARRIBA))
        {
            seleccion = (seleccion - 1 + CANT_OPCIONES) % CANT_OPCIONES;
        }
        if (gbt_tecla_presionada(GBTK_ABAJO))
        {
            seleccion = (seleccion + 1) % CANT_OPCIONES;
        }
        if (gbt_tecla_presionada(GBTK_ENTER))
        {
            if (seleccion == 0) return MENU_RESULTADO_JUGAR;
            if (seleccion == 1) { MENU_ESTADISTICAS();  if (g_salir_juego) return MENU_RESULTADO_SALIR; }
            if (seleccion == 2) { MENU_INSTRUCCIONES(); if (g_salir_juego) return MENU_RESULTADO_SALIR; }
            if (seleccion == 3) { MENU_OPCIONES();      if (g_salir_juego) return MENU_RESULTADO_SALIR; }
            if (seleccion == 4) return MENU_RESULTADO_SALIR;
        }
        // Q sale del juego en cualquier parte; ESC en menú principal selecciona SALIR
        if (gbt_tecla_sostenida(GBTK_q))
        {
            return MENU_RESULTADO_SALIR;
        }
        if (gbt_tecla_presionada(GBTK_ESCAPE))
        {
            seleccion = 4; // Mover selección a SALIR (feedback visual)
        }

        // Dibujado
        gbt_borrar_backbuffer(COLOR_NEGRO);
        DIBUJARFONDO();
        DIBUJAR_CABECERA();

        for (int i = 0; i < CANT_OPCIONES; i++)
        {
            DIBUJAR_OPCION(primerItemY + i * separacion, opciones[i], i == seleccion);
        }

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}

// -------------------------------------------------------
// MENU OPCIONES
// -------------------------------------------------------

void MENU_OPCIONES(void)
{
    // Ítems configurables
    const int CANT_ITEMS = 4;
    // 0: Paleta, 1: Resolución, 2: Velocidad inicial, 3: Guardar y volver
    int seleccion = 0;

    int alto  = CONFIG_ALTO();
    int ancho = CONFIG_ANCHO();
    int primerItemY = alto / 2 - 30;
    int separacion  = 20;

    // Copia local para editar sin afectar la config hasta guardar
    sConfig config_temp = config_actual;

    while (1)
    {
        // Recalcular por si cambia la resolución al aplicar config
        alto  = CONFIG_ALTO();
        ancho = CONFIG_ANCHO();
        primerItemY = alto / 2 - 30;

        gbt_procesar_entrada();

        if (gbt_tecla_presionada(GBTK_ARRIBA))
            seleccion = (seleccion - 1 + CANT_ITEMS) % CANT_ITEMS;

        if (gbt_tecla_presionada(GBTK_ABAJO))
            seleccion = (seleccion + 1) % CANT_ITEMS;

        // Cambiar valor con izquierda/derecha
        if (gbt_tecla_presionada(GBTK_IZQUIERDA))
        {
            if (seleccion == 0) // Paleta
                config_temp.paleta = (config_temp.paleta - 1 + CANT_PALETAS) % CANT_PALETAS;
            else if (seleccion == 1) // Resolución
                config_temp.resolucion = (config_temp.resolucion - 1 + 2) % 2;
            else if (seleccion == 2) // Velocidad
                config_temp.velocidad_inicial = (config_temp.velocidad_inicial - 1 + CANT_VELOCIDADES) % CANT_VELOCIDADES;
        }
        if (gbt_tecla_presionada(GBTK_DERECHA))
        {
            if (seleccion == 0)
                config_temp.paleta = (config_temp.paleta + 1) % CANT_PALETAS;
            else if (seleccion == 1)
                config_temp.resolucion = (config_temp.resolucion + 1) % 2;
            else if (seleccion == 2)
                config_temp.velocidad_inicial = (config_temp.velocidad_inicial + 1) % CANT_VELOCIDADES;
        }

        // Confirmar / guardar
        if (gbt_tecla_presionada(GBTK_ENTER))
        {
            if (seleccion == 3) // Guardar y volver
            {
                config_actual = config_temp;
                CONFIG_GUARDAR(&config_actual);
                CONFIG_APLICAR(&config_actual);
                return;
            }
        }

        // Escapar sin guardar
        if (gbt_tecla_presionada(GBTK_ESCAPE) || gbt_tecla_sostenida(GBTK_ESCAPE))
            return;
        // Q sale del juego completamente desde cualquier menú
        if (gbt_tecla_sostenida(GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        // --- Dibujado ---
        gbt_borrar_backbuffer(COLOR_NEGRO);
        DIBUJARFONDO();

        // Título
        int tituloX = (ancho - (int)strlen("OPCIONES") * anchoCaracter8) / 2;
        DIBUJARTEXTO(tituloX, primerItemY - 20, "OPCIONES", anchoCaracter8);

        // Ítem 0: Paleta
        {
            char linea[40];
            sprintf(linea, "PALETA  %s", NOMBRES_PALETAS[config_temp.paleta]);
            DIBUJAR_OPCION(primerItemY + 0 * separacion, linea, seleccion == 0);
        }

        // Ítem 1: Resolución
        {
            char linea[40];
            sprintf(linea, "RESOL   %s", NOMBRES_RESOLUCIONES[config_temp.resolucion]);
            DIBUJAR_OPCION(primerItemY + 1 * separacion, linea, seleccion == 1);
        }

        // Ítem 2: Velocidad
        {
            char linea[40];
            sprintf(linea, "VEL     %s", nombres_velocidades[config_temp.velocidad_inicial]);
            DIBUJAR_OPCION(primerItemY + 2 * separacion, linea, seleccion == 2);
        }

        // Ítem 3: Guardar y volver
        DIBUJAR_OPCION(primerItemY + 3 * separacion, "GUARDAR Y VOLVER", seleccion == 3);

        // Instrucción de escape
        DIBUJARTEXTO((ancho - (int)strlen("ESC VOLVER SIN GUARDAR") * anchoCaracter8) / 2,
                     alto - 20, "ESC VOLVER SIN GUARDAR", anchoCaracter8);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}

// -------------------------------------------------------
// ESTADISTICAS
// -------------------------------------------------------

void MENU_ESTADISTICAS(void)
{
    // Cargar estadísticas
    tEstadistica stats[5];
    int cant = STATS_CARGAR(stats, 5);

    while (1)
    {
        int alto  = CONFIG_ALTO();
        int ancho = CONFIG_ANCHO();
        gbt_procesar_entrada();

        if (gbt_tecla_presionada(GBTK_ESCAPE) || gbt_tecla_sostenida(GBTK_ESCAPE)
            || gbt_tecla_presionada(GBTK_ENTER))
        {
            return;
        }
        if (gbt_tecla_sostenida(GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        gbt_borrar_backbuffer(COLOR_NEGRO);
        DIBUJARFONDO();

        // Título
        int tituloX = (ancho - (int)strlen("ESTADISTICAS") * anchoCaracter8) / 2;
        DIBUJARTEXTO(tituloX, 20, "ESTADISTICAS", anchoCaracter8);

        // Encabezados
        // Columnas: NOMBRE a la izquierda, PUNTAJE separado por espacio fijo
        // Nombre ocupa hasta 13 chars * 8px = 104px; dejamos 20px de separación
        int colNombre  = ancho / 2 - 70;
        int colPuntaje = colNombre + 13 * anchoCaracter8 + 20; // siempre 124px después del inicio del nombre
        int filaY = 50;

        DIBUJARTEXTO(colNombre,  filaY, "NOMBRE", anchoCaracter8);
        DIBUJARTEXTO(colPuntaje, filaY, "PUNTAJE", anchoCaracter8);
        filaY += 14;

        // Línea separadora
        for (int x = colNombre - 4; x < colPuntaje + 7 * anchoCaracter8 + 4; x++)
            gbt_dibujar_pixel(x, filaY, COLOR_GRIS_CLARO);
        filaY += 6;

        if (cant == 0)
        {
            int sinDatosX = (ancho - (int)strlen("SIN PARTIDAS REGISTRADAS") * anchoCaracter8) / 2;
            DIBUJARTEXTO(sinDatosX, filaY + 20, "SIN PARTIDAS REGISTRADAS", anchoCaracter8);
        }
        else
        {
            for (int i = 0; i < cant; i++)
            {
                char puntajeStr[12];
                sprintf(puntajeStr, "%d", stats[i].puntaje);

                DIBUJARTEXTO(colNombre,  filaY, stats[i].nombre,  anchoCaracter8);
                DIBUJARTEXTO(colPuntaje, filaY, puntajeStr, anchoCaracter8);
                filaY += 14;
            }
        }

        // Instrucción
        int instrX = (ancho - (int)strlen("ENTER O ESC PARA VOLVER") * anchoCaracter8) / 2;
        DIBUJARTEXTO(instrX, alto - 20, "ENTER O ESC PARA VOLVER", anchoCaracter8);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}

// -------------------------------------------------------
// INSTRUCCIONES
// -------------------------------------------------------

void MENU_INSTRUCCIONES(void)
{
    while (1)
    {
        gbt_procesar_entrada();

        if (gbt_tecla_presionada(GBTK_ESCAPE) || gbt_tecla_sostenida(GBTK_ESCAPE)
            || gbt_tecla_presionada(GBTK_ENTER))
        {
            return;
        }
        if (gbt_tecla_sostenida(GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        // Recalcular resolución cada frame (puede cambiar desde opciones)
        int alto  = CONFIG_ALTO();
        int ancho = CONFIG_ANCHO();

        gbt_borrar_backbuffer(COLOR_NEGRO);
        DIBUJARFONDO();

        // Título
        int tituloX = (ancho - (int)strlen("INSTRUCCIONES") * anchoCaracter8) / 2;
        DIBUJARTEXTO(tituloX, 20, "INSTRUCCIONES", anchoCaracter8);

        // Layout adaptable: 9 ítems (1 enc + 2 obj + 1 enc + 6 ctrl) + 1 separador = ~10 saltos
        // En CGA área = 200-28-36 = 136px -> sep=13. En VGA área=416px -> sep=15 (cap).
        int areaTop    = 36;
        int areaBottom = alto - 28;
        int areaAltura = areaBottom - areaTop;
        int sep = areaAltura / 10;
        if (sep < 10) sep = 10;
        if (sep > 15) sep = 15;

        int margen = (ancho - 28 * anchoCaracter8) / 2;
        int y = areaTop;

        DIBUJARTEXTO(margen, y, "OBJETIVO", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "COMPLETAR FILAS PARA GANAR", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "PUNTOS SIN LLEGAR AL TOPE.", anchoCaracter8);
        y += sep + sep / 2;

        DIBUJARTEXTO(margen, y, "CONTROLES", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "IZQ DER  MOVER PIEZA", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "ABAJO    BAJAR MAS RAPIDO", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "A        ROTAR ANTIHORARIO", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "D        ROTAR HORARIO", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "P        PAUSAR", anchoCaracter8);
        y += sep;
        DIBUJARTEXTO(margen, y, "Q        SALIR AL MENU", anchoCaracter8);

        // Leyenda fija en el fondo
        int instrX = (ancho - (int)strlen("ENTER O ESC PARA VOLVER") * anchoCaracter8) / 2;
        DIBUJARTEXTO(instrX, alto - 20, "ENTER O ESC PARA VOLVER", anchoCaracter8);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}