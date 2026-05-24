#include "menu.h"
#include "configuracion.h"
#include "graficos.h"
#include "estadisticas.h"
#include "inicio.h"
#include "archivos.h"
#include <string.h>
#include <stdio.h>

// Constantes de espaciado para la pantalla de estadísticas (altoContenido)
#define STATS_PAD_TOP 8
#define STATS_ALTO_TITULO 10
#define STATS_GAP_TITULO 8
#define STATS_ALTO_ENCABEZADO 8
#define STATS_GAP_SEPARADOR 6
#define STATS_ALTO_FILA 14
#define STATS_GAP_SIN_DATOS 8
#define STATS_ALTO_SIN_DATOS 20
#define STATS_GAP_DATOS 10
#define STATS_PAD_BOTTOM 8

// Propaga la señal de salida (Q) desde cualquier submenú hasta el main
static int g_salir_juego;
static const char* NOMBRES_PALETAS [CANT_PALETAS] = {"CGA", "GAME BOY", "ATARI"};
static const char* NOMBRES_RESOLUCIONES [2] = {"CGA 320X200", "VGA 640X480"};
static const char* NOMBRES_MODOS [2] = {"CLASICO", "DX"};

// Dibuja una opción centrada; si está seleccionada la resalta con fondo negro e indicador
static void DIBUJAR_OPCION (int posY, const char *texto, int seleccionada)
{
    int ancho, largo, posX;

    ancho = CONFIG_ANCHO ();
    largo = (int) strlen (texto);
    posX = (ancho - largo * anchoCaracter8) / 2;

    if (seleccionada)
    {
        DIBUJAR_RECTANGULO (posX - 4, posY - 1, largo * anchoCaracter8 + 8, altoCaracter + 2, COLOR_NEGRO);
        DIBUJARCARACTER (posX - 12, posY, 23, anchoCaracter8, COLOR_GRIS_CLARO);
        DIBUJARTEXTO (posX, posY, (char*) texto, anchoCaracter8);
    }
    else
    {
        DIBUJARTEXTO (posX, posY, (char*) texto, anchoCaracter8);
    }
}

// Dibuja el logo TETRIS centrado en el tercio superior de la pantalla
static void DIBUJAR_CABECERA ()
{
    int ancho, alto, logoAncho, logoAlto, logoX, logoY;

    ancho = CONFIG_ANCHO ();
    alto = CONFIG_ALTO ();
    logoAncho = 167;
    logoAlto = 110;
    logoX = (ancho - logoAncho) / 2;
    logoY = (alto / 2 - logoAlto) / 2;

    if (logoY < 3)
    {
        logoY = 3;
    }

    DIBUJAR_LOGO_COMPLETO (logoX, logoY);
}

// Menú principal: JUGAR, CONTINUAR (si hay guardado), ESTADISTICAS, INSTRUCCIONES, OPCIONES, SALIR
eMenuResultado MENU_PRINCIPAL()
{
    int hay_guardado, CANT_OPCIONES, seleccion, rehacerMenu, i;
    int alto, logoAlto, logoY, logoBottom, areaDisponible, totalOpciones, primerItemY, separacion;
    const char *opciones [6];
    eMenuResultado resultados [6];
    eMenuResultado r;

    g_salir_juego = 0;

    while (1)
    {
        hay_guardado = PARTIDA_EXISTE ();
        CANT_OPCIONES = 0;

        opciones [CANT_OPCIONES] = "JUGAR";
        resultados [CANT_OPCIONES] = MENU_RESULTADO_JUGAR;
        CANT_OPCIONES++;

        if (hay_guardado)
        {
            opciones [CANT_OPCIONES] = "CONTINUAR";
            resultados [CANT_OPCIONES] = MENU_RESULTADO_CONTINUAR;
            CANT_OPCIONES++;
        }

        opciones [CANT_OPCIONES] = "ESTADISTICAS";
        resultados [CANT_OPCIONES] = (eMenuResultado)-1;
        CANT_OPCIONES++;

        opciones [CANT_OPCIONES] = "INSTRUCCIONES";
        resultados [CANT_OPCIONES] = (eMenuResultado)-2;
        CANT_OPCIONES++;

        opciones [CANT_OPCIONES] = "OPCIONES";
        resultados [CANT_OPCIONES] = (eMenuResultado)-3;
        CANT_OPCIONES++;

        opciones [CANT_OPCIONES] = "SALIR";
        resultados [CANT_OPCIONES] = MENU_RESULTADO_SALIR;
        CANT_OPCIONES ++;

        seleccion = 0;
        rehacerMenu = 0;

        while (!rehacerMenu)
        {
            alto = CONFIG_ALTO ();
            logoAlto = 110;
            logoY = (alto / 2 - logoAlto) / 2;

            if (logoY < 3)
            {
                logoY = 3;
            }

            logoBottom = logoY + logoAlto;
            areaDisponible = alto - logoBottom;
            totalOpciones = CANT_OPCIONES * 14;
            primerItemY = logoBottom + (areaDisponible - totalOpciones) / 2;

            if (primerItemY < logoBottom + 4)
            {
                primerItemY = logoBottom + 4;
            }

            separacion = 14 + (alto - 200) / 20;

            gbt_procesar_entrada ();

            if (gbt_tecla_presionada (GBTK_ARRIBA))
            {
                seleccion = (seleccion - 1 + CANT_OPCIONES) % CANT_OPCIONES;
            }

            if (gbt_tecla_presionada (GBTK_ABAJO))
            {
                seleccion = (seleccion + 1) % CANT_OPCIONES;
            }

            if (gbt_tecla_presionada (GBTK_ENTER))
            {
                r = resultados [seleccion];

                if (r == MENU_RESULTADO_JUGAR || r == MENU_RESULTADO_CONTINUAR)
                {
                    return r;
                }

                if (r == MENU_RESULTADO_SALIR)
                {
                    return r;
                }

                if (r == (eMenuResultado)-1)
                {
                    MENU_ESTADISTICAS ();
                    if (g_salir_juego)
                    {
                        return MENU_RESULTADO_SALIR;
                    }
                    rehacerMenu = 1;
                }

                if (r == (eMenuResultado)-2)
                {
                    MENU_INSTRUCCIONES ();
                    if (g_salir_juego)
                    {
                        return MENU_RESULTADO_SALIR;
                    }
                    rehacerMenu = 1;
                }

                if (r == (eMenuResultado)-3)
                {
                    MENU_OPCIONES ();
                    if (g_salir_juego)
                    {
                        return MENU_RESULTADO_SALIR;
                    }
                    rehacerMenu = 1;
                }
            }

            if (gbt_tecla_sostenida (GBTK_q))
            {
                return MENU_RESULTADO_SALIR;
            }

            if (gbt_tecla_presionada (GBTK_ESCAPE))
            {
                seleccion = CANT_OPCIONES - 1;
            }

            gbt_borrar_backbuffer (COLOR_NEGRO);
            DIBUJARFONDO ();
            DIBUJAR_CABECERA ();

            for (i = 0; i < CANT_OPCIONES; i ++)
            {
                DIBUJAR_OPCION (primerItemY + i * separacion, opciones[i], i == seleccion);
            }

            gbt_volcar_backbuffer ();
            gbt_esperar (16);
        }
    }
}

// Menú de configuración: paleta, resolución, velocidad, modo, ancho de tablero
void MENU_OPCIONES ()
{
    int CANT_ITEMS, seleccion, alto, ancho, primerItemY, separacion;
    sConfig config_temp;
    char linea [40];
    int tituloX;

    CANT_ITEMS = 6;
    seleccion = 0;
    alto = CONFIG_ALTO ();
    ancho = CONFIG_ANCHO ();
    primerItemY = alto / 2 - 30;
    separacion = 20;
    config_temp = config_actual;

    while (1)
    {
        alto = CONFIG_ALTO ();
        ancho = CONFIG_ANCHO ();
        primerItemY = alto / 2 - 30;

        gbt_procesar_entrada ();

        if (gbt_tecla_presionada (GBTK_ARRIBA))
        {
            seleccion = (seleccion - 1 + CANT_ITEMS) % CANT_ITEMS;
        }

        if (gbt_tecla_presionada (GBTK_ABAJO))
        {
            seleccion = (seleccion + 1) % CANT_ITEMS;
        }

        if (gbt_tecla_presionada (GBTK_IZQUIERDA))
        {
            if (seleccion == 0)
            {
                config_temp.paleta = (config_temp.paleta - 1 + CANT_PALETAS) % CANT_PALETAS;
            }
            else if (seleccion == 1)
            {
                config_temp.resolucion = (config_temp.resolucion - 1 + 2) % 2;
            }
            else if (seleccion == 2)
            {
                config_temp.velocidad_inicial = (config_temp.velocidad_inicial - 1 + CANT_VELOCIDADES) % CANT_VELOCIDADES;
            }
            else if (seleccion == 3)
            {
                config_temp.modo_juego = (config_temp.modo_juego - 1 + 2) % 2;
            }
            else if (seleccion == 4)
            {
                config_temp.ancho_tablero --;

                if (config_temp.ancho_tablero < 8)
                {
                    config_temp.ancho_tablero = MAX_COLUMNAS;
                }
            }
        }

        if (gbt_tecla_presionada (GBTK_DERECHA))
        {
            if (seleccion == 0)
            {
                config_temp.paleta = (config_temp.paleta + 1) % CANT_PALETAS;
            }
            else if (seleccion == 1)
            {
                config_temp.resolucion = (config_temp.resolucion + 1) % 2;
            }
            else if (seleccion == 2)
            {
                config_temp.velocidad_inicial = (config_temp.velocidad_inicial + 1) % CANT_VELOCIDADES;
            }
            else if (seleccion == 3)
            {
                config_temp.modo_juego = (config_temp.modo_juego + 1) % 2;
            }
            else if (seleccion == 4)
            {
                config_temp.ancho_tablero ++;

                if (config_temp.ancho_tablero > MAX_COLUMNAS)
                {
                    config_temp.ancho_tablero = 8;
                }
            }
        }

        if (gbt_tecla_presionada (GBTK_ENTER))
        {
            if (seleccion == 5)
            {
                config_actual = config_temp;

                if (config_actual.modo_juego == MODO_DX)
                {
                    piezas_en_uso = MAX_PIEZAS;
                }
                else
                {
                    piezas_en_uso = 7;
                }

                if (config_actual.modo_juego == MODO_DX)
                {
                    columnasTablero = config_actual.ancho_tablero;
                }
                else
                {
                    columnasTablero = 10;
                }

                CONFIG_GUARDAR (&config_actual);
                CONFIG_APLICAR (&config_actual);
                return;
            }
        }

        if (gbt_tecla_presionada (GBTK_ESCAPE) || gbt_tecla_sostenida (GBTK_ESCAPE))
        {
            return;
        }

        if (gbt_tecla_sostenida (GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        gbt_borrar_backbuffer (COLOR_NEGRO);
        DIBUJARFONDO ();

        tituloX = (ancho - (int) strlen ("OPCIONES") * anchoCaracter8) / 2;
        DIBUJARTEXTO (tituloX, primerItemY - 20, "OPCIONES", anchoCaracter8);

        sprintf (linea, "PALETA  %s", NOMBRES_PALETAS[config_temp.paleta]);
        DIBUJAR_OPCION (primerItemY + 0 * separacion, linea, seleccion == 0);

        sprintf (linea, "RESOL   %s", NOMBRES_RESOLUCIONES [config_temp.resolucion]);
        DIBUJAR_OPCION (primerItemY + 1 * separacion, linea, seleccion == 1);

        sprintf (linea, "VEL     %s", nombres_velocidades[config_temp.velocidad_inicial]);
        DIBUJAR_OPCION (primerItemY + 2 * separacion, linea, seleccion == 2);

        sprintf (linea, "MODO    %s", NOMBRES_MODOS[config_temp.modo_juego]);
        DIBUJAR_OPCION (primerItemY + 3 * separacion, linea, seleccion == 3);

        if (config_temp.modo_juego == MODO_DX)
        {
            sprintf (linea, "ANCHO   %d", config_temp.ancho_tablero);
        }
        else
        {
            sprintf (linea, "ANCHO   10 (FIJO)");
        }
        DIBUJAR_OPCION (primerItemY + 4 * separacion, linea, seleccion == 4);

        DIBUJAR_OPCION (primerItemY + 5 * separacion, "GUARDAR Y VOLVER", seleccion == 5);

        tituloX = (ancho - (int) strlen ("ESC VOLVER SIN GUARDAR") * anchoCaracter8) / 2;
        DIBUJARTEXTO (tituloX, alto - 20, "ESC VOLVER SIN GUARDAR", anchoCaracter8);

        gbt_volcar_backbuffer ();
        gbt_esperar (16);
    }
}

// Muestra el ranking de puntajes (top 5) en formato NOMBRE | PUNTAJE
void MENU_ESTADISTICAS ()
{
    tEstadistica stats [5];
    int cant, alto, ancho, i, x;
    int anchoNombre, anchoPuntaje, gapColumnas, anchoTabla, colNombre, colPuntaje;
    int altoContenido, grupoY, filaY, tituloX, sinDatosX, instrX;
    char puntajeStr [12];

    cant = STATS_CARGAR (stats, 5);

    while (1)
    {
        alto = CONFIG_ALTO ();
        ancho = CONFIG_ANCHO ();

        gbt_procesar_entrada ();

        if (gbt_tecla_presionada (GBTK_ESCAPE) || gbt_tecla_sostenida (GBTK_ESCAPE) || gbt_tecla_presionada (GBTK_ENTER))
        {
            return;
        }

        if (gbt_tecla_sostenida (GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        gbt_borrar_backbuffer (COLOR_NEGRO);
        DIBUJARFONDO ();

        anchoNombre = 13 * anchoCaracter8;
        anchoPuntaje = 7 * anchoCaracter8;
        gapColumnas = 20;
        anchoTabla = anchoNombre + gapColumnas + anchoPuntaje;
        colNombre = (ancho - anchoTabla) / 2;
        colPuntaje = colNombre + anchoNombre + gapColumnas;

        if (cant > 0)
        {
            altoContenido = STATS_PAD_TOP + STATS_ALTO_TITULO + STATS_GAP_TITULO + STATS_ALTO_ENCABEZADO + STATS_GAP_SEPARADOR + cant * STATS_ALTO_FILA + STATS_GAP_DATOS + STATS_PAD_BOTTOM;
        }
        else
        {
            altoContenido = STATS_PAD_TOP + STATS_ALTO_TITULO + STATS_GAP_TITULO + STATS_ALTO_ENCABEZADO + STATS_GAP_SEPARADOR + STATS_GAP_SIN_DATOS + STATS_ALTO_SIN_DATOS + STATS_GAP_DATOS + STATS_PAD_BOTTOM;
        }
        grupoY = (alto - altoContenido) / 2;

        if (grupoY < 10)
        {
            grupoY = 10;
        }

        filaY = grupoY;

        tituloX = (ancho - (int) strlen ("ESTADISTICAS") * anchoCaracter8) / 2;
        DIBUJARTEXTO (tituloX, filaY, "ESTADISTICAS", anchoCaracter8);
        filaY += 18;

        DIBUJARTEXTO (colNombre, filaY, "NOMBRE", anchoCaracter8);
        DIBUJARTEXTO (colPuntaje, filaY, "PUNTAJE", anchoCaracter8);
        filaY += 14;

        for (x = colNombre - 4; x < colPuntaje + anchoPuntaje + 4; x++)
        {
            gbt_dibujar_pixel (x, filaY, COLOR_GRIS_CLARO);
        }

        filaY += 6;

        if (cant == 0)
        {
            sinDatosX = (ancho - (int) strlen ("SIN PARTIDAS REGISTRADAS") * anchoCaracter8) / 2;
            DIBUJARTEXTO (sinDatosX, filaY + 10, "SIN PARTIDAS REGISTRADAS", anchoCaracter8);
        }
        else
        {
            for (i = 0; i < cant; i++)
            {
                sprintf (puntajeStr, "%d", stats[i].puntaje);
                DIBUJARTEXTO (colNombre, filaY, stats[i].nombre, anchoCaracter8);
                DIBUJARTEXTO (colPuntaje, filaY, puntajeStr, anchoCaracter8);
                filaY += 14;
            }
        }

        instrX = (ancho - (int) strlen ("ENTER O ESC PARA VOLVER") * anchoCaracter8) / 2;
        DIBUJARTEXTO (instrX, alto - 20, "ENTER O ESC PARA VOLVER", anchoCaracter8);

        gbt_volcar_backbuffer ();
        gbt_esperar (16);
    }
}

// Muestra los controles del juego en dos columnas (tecla | descripción)
void MENU_INSTRUCCIONES ()
{
    int alto, ancho, i, lt, ld, num_obj, filas, sep;
    int max_tecla, max_desc, ancho_teclas, ancho_desc, gap, ancho_total, centro_cols;
    int col_tecla_x, col_desc_x, alto_texto, y_inicio, y, x, instrX;
    const char *obj [3];
    const char *teclas [7], *desc [7];
    const char *tit_ctrl;

    obj [0] = "OBJETIVO";
    obj [1] = "COMPLETAR FILAS SIN LLEGAR AL TOPE";
    obj [2] = "";
    teclas [0] = "IZQ/DER";
    teclas [1] = "ABAJO";
    teclas [2] = "A";
    teclas [3] = "D";
    teclas [4] = "P";
    teclas [5] = "C";
    teclas [6] = "Q";
    desc [0] = "MOVER PIEZA";
    desc [1] = "BAJAR MAS RAPIDO";
    desc [2] = "ROTAR ANTIHORARIO";
    desc [3] = "ROTAR HORARIO";
    desc [4] = "PAUSAR";
    desc [5] = "CHEAT (LENTITUD 5S)";
    desc [6] = "SALIR AL MENU";
    tit_ctrl = "CONTROLES";

    num_obj = 3;
    filas = 7;

    while (1)
    {
        gbt_procesar_entrada ();

        if (gbt_tecla_presionada (GBTK_ESCAPE) || gbt_tecla_sostenida (GBTK_ESCAPE) || gbt_tecla_presionada (GBTK_ENTER))
        {
            return;
        }

        if (gbt_tecla_sostenida (GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        alto = CONFIG_ALTO ();
        ancho = CONFIG_ANCHO ();

        gbt_borrar_backbuffer (COLOR_NEGRO);
        DIBUJARFONDO ();

        if (alto > 300)
        {
            sep = 12;
        }
        else
        {
            sep = 6;
        }

        max_tecla = 0;
        max_desc = 0;

        for (i = 0; i < filas; i++)
        {
            lt = (int) strlen (teclas[i]);
            ld = (int) strlen (desc[i]);

            if (lt > max_tecla)
            {
                max_tecla = lt;
            }

            if (ld > max_desc)
            {
                max_desc = ld;
            }
        }

        ancho_teclas = max_tecla * anchoCaracter8;
        ancho_desc = max_desc * anchoCaracter8;
        gap = 16;
        ancho_total = ancho_teclas + gap + ancho_desc;
        centro_cols = (ancho - ancho_total) / 2;
        col_tecla_x = centro_cols;
        col_desc_x = centro_cols + ancho_teclas + gap;

        alto_texto = (num_obj + 1 + filas) * (altoCaracter + sep) - sep;
        y_inicio = (alto - alto_texto) / 2;
        y = y_inicio;

        for (i = 0; i < num_obj; i ++)
        {
            if (obj [i][0] != '\0')
            {
                x = (ancho - (int) strlen (obj [i]) * anchoCaracter8) / 2;
                DIBUJARTEXTO (x, y, (char*) obj [i], anchoCaracter8);
            }

            y += altoCaracter + sep;
        }

        x = (ancho - (int) strlen (tit_ctrl) * anchoCaracter8) / 2;
        DIBUJARTEXTO (x, y, (char*) tit_ctrl, anchoCaracter8);
        y += altoCaracter + sep;

        for (i = 0; i < filas; i ++)
        {
            DIBUJARTEXTO (col_tecla_x, y, (char*) teclas[i], anchoCaracter8);
            DIBUJARTEXTO (col_desc_x,  y, (char*) desc[i],   anchoCaracter8);
            y += altoCaracter + sep;
        }

        instrX = (ancho - (int) strlen ("ENTER O ESC PARA VOLVER") * anchoCaracter8) / 2;
        DIBUJARTEXTO (instrX, alto - 20, "ENTER O ESC PARA VOLVER", anchoCaracter8);

        gbt_volcar_backbuffer ();
        gbt_esperar (16);
    }
}

// Menú de pausa superpuesto al juego: CONTINUAR, GUARDAR Y SALIR, SALIR SIN GUARDAR
ePausaResultado MENU_PAUSA ()
{
    int i, seleccion, CANT_OPCIONES;
    int ancho, alto, centroX, centroY, anchoCaja, altoCaja, x0, y0, tituloX;
    const char *opciones [3];
    int opY;

    opciones [0] = "CONTINUAR";
    opciones [1] = "GUARDAR Y SALIR AL MENU";
    opciones [2] = "SALIR AL MENU SIN GUARDAR";
    CANT_OPCIONES = 3;
    seleccion = 0;

    while (1)
    {
        ancho = CONFIG_ANCHO ();
        alto = CONFIG_ALTO ();
        centroX = ancho / 2;
        centroY = alto / 2;
        anchoCaja = (int) strlen ("SALIR AL MENU SIN GUARDAR") * anchoCaracter8 + 72;
        altoCaja = 16 + CANT_OPCIONES * 16 + 8;
        x0 = centroX - anchoCaja / 2;
        y0 = centroY - altoCaja / 2;

        gbt_procesar_entrada ();

        if (gbt_tecla_presionada (GBTK_ARRIBA))
        {
            seleccion = (seleccion - 1 + CANT_OPCIONES) % CANT_OPCIONES;
        }

        if (gbt_tecla_presionada (GBTK_ABAJO))
        {
            seleccion = (seleccion + 1) % CANT_OPCIONES;
        }

        if (gbt_tecla_presionada (GBTK_ENTER) || gbt_tecla_presionada (GBTK_p))
        {
            if (seleccion == 0)
            {
                return PAUSA_RESULTADO_CONTINUAR;
            }

            if (seleccion == 1)
            {
                return PAUSA_RESULTADO_GUARDAR_Y_SALIR;
            }

            if (seleccion == 2)
            {
                return PAUSA_RESULTADO_SALIR_SIN_GUARDAR;
            }
        }

        if (gbt_tecla_presionada (GBTK_ESCAPE))
        {
            return PAUSA_RESULTADO_CONTINUAR;
        }

        DIBUJAR ();
        DIBUJARMARCOGENERICO (x0, y0, anchoCaja, altoCaja, 0);

        tituloX = centroX - ((int) strlen ("PAUSA") * anchoCaracter8) / 2;
        DIBUJARTEXTO (tituloX, y0 + 4, "PAUSA", anchoCaracter8);

        for (i = 0; i < CANT_OPCIONES; i ++)
        {
            opY = y0 + 16 + i * 16;
            DIBUJAR_OPCION (opY, opciones [i], i == seleccion);
        }

        gbt_volcar_backbuffer ();
        gbt_esperar (16);
    }
}
