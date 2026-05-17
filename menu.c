#include "menu.h"
#include "estadisticas.h"
#include "inicio.h"
#include "archivos.h"
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
        // Fondo resaltado: color 0 (negro en todas las paletas) para que contraste siempre
        DIBUJAR_RECTANGULO(posX - 4, posY - 1, largo * anchoCaracter8 + 8, altoCaracter + 2, COLOR_NEGRO);
        // Flecha indicadora: color 7 (gris claro/verde claro) para que sea visible en todas las paletas
        DIBUJARCARACTER(posX - 12, posY, 17, anchoCaracter8, COLOR_GRIS_CLARO); // '>'  (R en la fuente)
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
    g_salir_juego = 0; // Resetear flag al entrar al menú principal

    while (1)
    {
        // Detectar si hay partida guardada (puede cambiar entre frames si se borró)
        int hay_guardado = PARTIDA_EXISTE();

        // Construir lista de opciones dinámica
        const char *opciones[6];
        eMenuResultado resultados[6];
        int CANT_OPCIONES = 0;

        opciones[CANT_OPCIONES]   = "JUGAR";
        resultados[CANT_OPCIONES] = MENU_RESULTADO_JUGAR;
        CANT_OPCIONES++;

        if (hay_guardado)
        {
            opciones[CANT_OPCIONES]   = "CONTINUAR";
            resultados[CANT_OPCIONES] = MENU_RESULTADO_CONTINUAR;
            CANT_OPCIONES++;
        }

        opciones[CANT_OPCIONES]   = "ESTADISTICAS";
        resultados[CANT_OPCIONES] = (eMenuResultado)-1; // submenú
        CANT_OPCIONES++;

        opciones[CANT_OPCIONES]   = "INSTRUCCIONES";
        resultados[CANT_OPCIONES] = (eMenuResultado)-2;
        CANT_OPCIONES++;

        opciones[CANT_OPCIONES]   = "OPCIONES";
        resultados[CANT_OPCIONES] = (eMenuResultado)-3;
        CANT_OPCIONES++;

        opciones[CANT_OPCIONES]   = "SALIR";
        resultados[CANT_OPCIONES] = MENU_RESULTADO_SALIR;
        CANT_OPCIONES++;

        int seleccion = 0;

        // Loop de este frame (re-entra si cambia hay_guardado)
        int rehacerMenu = 0;
        while (!rehacerMenu)
        {
            // Recalcular layout
            int alto = CONFIG_ALTO();
            int logoAlto  = 110;
            int logoY     = (alto / 2 - logoAlto) / 2;
            if (logoY < 3) logoY = 3;
            int logoBottom = logoY + logoAlto;
            int areaDisponible = alto - logoBottom;
            int totalOpciones  = CANT_OPCIONES * 14;
            int primerItemY = logoBottom + (areaDisponible - totalOpciones) / 2;
            if (primerItemY < logoBottom + 4) primerItemY = logoBottom + 4;
            int separacion  = 14 + (alto - 200) / 20;

            gbt_procesar_entrada();

            if (gbt_tecla_presionada(GBTK_ARRIBA))
                seleccion = (seleccion - 1 + CANT_OPCIONES) % CANT_OPCIONES;
            if (gbt_tecla_presionada(GBTK_ABAJO))
                seleccion = (seleccion + 1) % CANT_OPCIONES;

            if (gbt_tecla_presionada(GBTK_ENTER))
            {
                eMenuResultado r = resultados[seleccion];
                if (r == MENU_RESULTADO_JUGAR || r == MENU_RESULTADO_CONTINUAR)
                    return r;
                if (r == MENU_RESULTADO_SALIR)
                    return r;
                // Submenús
                if (r == (eMenuResultado)-1) { MENU_ESTADISTICAS();  if (g_salir_juego) return MENU_RESULTADO_SALIR; rehacerMenu = 1; }
                if (r == (eMenuResultado)-2) { MENU_INSTRUCCIONES(); if (g_salir_juego) return MENU_RESULTADO_SALIR; rehacerMenu = 1; }
                if (r == (eMenuResultado)-3) { MENU_OPCIONES();      if (g_salir_juego) return MENU_RESULTADO_SALIR; rehacerMenu = 1; }
            }

            if (gbt_tecla_sostenida(GBTK_q))
                return MENU_RESULTADO_SALIR;
            if (gbt_tecla_presionada(GBTK_ESCAPE))
                seleccion = CANT_OPCIONES - 1; // Mover a SALIR

            // Dibujado
            gbt_borrar_backbuffer(COLOR_NEGRO);
            DIBUJARFONDO();
            DIBUJAR_CABECERA();

            for (int i = 0; i < CANT_OPCIONES; i++)
                DIBUJAR_OPCION(primerItemY + i * separacion, opciones[i], i == seleccion);

            gbt_volcar_backbuffer();
            gbt_esperar(16);
        }
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

        // Calcular ancho total de la tabla
        int anchoNombre = 13 * anchoCaracter8;
        int anchoPuntaje = 7 * anchoCaracter8;
        int gapColumnas = 20;
        int anchoTabla = anchoNombre + gapColumnas + anchoPuntaje;
        int colNombre = (ancho - anchoTabla) / 2;
        int colPuntaje = colNombre + anchoNombre + gapColumnas;

        // Altura total del contenido: titulo + gap + headers + linea + gap + (max 5 filas) + gap + instruccion
        int altoContenido = 8 + 10 + 8 + 8 + 6 + (cant > 0 ? cant * 14 : 8 + 20) + 10 + 8;
        int grupoY = (alto - altoContenido) / 2;
        if (grupoY < 10) grupoY = 10;
        int filaY = grupoY;

        // Título
        int tituloX = (ancho - (int)strlen("ESTADISTICAS") * anchoCaracter8) / 2;
        DIBUJARTEXTO(tituloX, filaY, "ESTADISTICAS", anchoCaracter8);
        filaY += 18;

        // Encabezados
        DIBUJARTEXTO(colNombre, filaY, "NOMBRE", anchoCaracter8);
        DIBUJARTEXTO(colPuntaje, filaY, "PUNTAJE", anchoCaracter8);
        filaY += 14;

        // Línea separadora
        for (int x = colNombre - 4; x < colPuntaje + anchoPuntaje + 4; x++)
            gbt_dibujar_pixel(x, filaY, COLOR_GRIS_CLARO);
        filaY += 6;

        // Datos
        if (cant == 0)
        {
            int sinDatosX = (ancho - (int)strlen("SIN PARTIDAS REGISTRADAS") * anchoCaracter8) / 2;
            DIBUJARTEXTO(sinDatosX, filaY + 10, "SIN PARTIDAS REGISTRADAS", anchoCaracter8);
        }
        else
        {
            for (int i = 0; i < cant; i++)
            {
                char puntajeStr[12];
                sprintf(puntajeStr, "%d", stats[i].puntaje);

                DIBUJARTEXTO(colNombre, filaY, stats[i].nombre, anchoCaracter8);
                DIBUJARTEXTO(colPuntaje, filaY, puntajeStr, anchoCaracter8);
                filaY += 14;
            }
        }

        // Instrucción al fondo
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
// -------------------------------------------------------
// MENU PAUSA (con navegación por flechas y Enter)
// -------------------------------------------------------

ePausaResultado MENU_PAUSA(void)
{
    const char *opciones[] = {
        "CONTINUAR",
        "GUARDAR Y SALIR AL MENU",
        "SALIR AL MENU SIN GUARDAR"
    };
    const int CANT_OPCIONES = 3;
    int seleccion = 0;

    while (1)
    {
        int ancho = CONFIG_ANCHO();
        int alto  = CONFIG_ALTO();
        int centroX = ancho / 2;
        int centroY = alto  / 2;

        // Caja: ancho suficiente para la opción más larga
        int anchoCaja = (int)strlen("SALIR AL MENU SIN GUARDAR") * anchoCaracter8 + 72;
        int altoCaja  = 16 + CANT_OPCIONES * 16 + 8;
        int x0 = centroX - anchoCaja / 2;
        int y0 = centroY - altoCaja  / 2;

        gbt_procesar_entrada();

        if (gbt_tecla_presionada(GBTK_ARRIBA))
            seleccion = (seleccion - 1 + CANT_OPCIONES) % CANT_OPCIONES;
        if (gbt_tecla_presionada(GBTK_ABAJO))
            seleccion = (seleccion + 1) % CANT_OPCIONES;

        if (gbt_tecla_presionada(GBTK_ENTER) || gbt_tecla_presionada(GBTK_p))
        {
            if (seleccion == 0) return PAUSA_RESULTADO_CONTINUAR;
            if (seleccion == 1) return PAUSA_RESULTADO_GUARDAR_Y_SALIR;
            if (seleccion == 2) return PAUSA_RESULTADO_SALIR_SIN_GUARDAR;
        }

        // P también continúa (tecla de pausa original)
        if (gbt_tecla_presionada(GBTK_ESCAPE))
            return PAUSA_RESULTADO_CONTINUAR;

        // Dibujar el juego de fondo + el panel de pausa encima
        DIBUJAR();
        DIBUJARMARCOGENERICO(x0, y0, anchoCaja, altoCaja, 0);

        // Título
        char *titulo = "PAUSA";
        DIBUJARTEXTO(centroX - ((int)strlen(titulo) * anchoCaracter8) / 2, y0 + 4, titulo, anchoCaracter8);

        // Opciones
        for (int i = 0; i < CANT_OPCIONES; i++)
        {
            int opY = y0 + 16 + i * 16;
            DIBUJAR_OPCION(opY, opciones[i], i == seleccion);
        }

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}