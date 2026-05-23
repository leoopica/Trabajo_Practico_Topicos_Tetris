#include "menu.h"
#include "estadisticas.h"
#include "inicio.h"
#include "archivos.h"
#include <string.h>
#include <stdio.h>

// Flag global: si se presiona Q en cualquier submenú, propaga la salida hasta main
static int g_salir_juego = 0;

// Nombres para mostrar en el menú de opciones
static const char* NOMBRES_PALETAS[CANT_PALETAS] = {
    "CGA", "GAME BOY", "ATARI",
};

static const char* NOMBRES_RESOLUCIONES[2] = {
    "CGA 320X200", "VGA 640X480",
};

static const char* NOMBRES_MODOS[2] = {
    "CLASICO", "DX",
};

// -------------------------------------------------------
// Helpers internos de dibujo
// -------------------------------------------------------

// Dibuja una opción de menú centrada horizontalmente.
// Si seleccionada==1, pinta un rectángulo de fondo negro y un carácter 23 (X)
// a la izquierda como indicador de selección.
static void DIBUJAR_OPCION(int posY, const char *texto, int seleccionada)
{
    int ancho = CONFIG_ANCHO();
    int largo = (int)strlen(texto);
    int posX = (ancho - largo * anchoCaracter8) / 2;

    if (seleccionada)
    {
        // Fondo negro para resaltar la opción seleccionada
        DIBUJAR_RECTANGULO(posX - 4, posY - 1, largo * anchoCaracter8 + 8, altoCaracter + 2, COLOR_NEGRO);
        // Carácter 23 (X) como indicador de selección (visible en todas las paletas con color 7)
        DIBUJARCARACTER(posX - 12, posY, 23, anchoCaracter8, COLOR_GRIS_CLARO);
        DIBUJARTEXTO(posX, posY, (char*)texto, anchoCaracter8);
    }
    else
        DIBUJARTEXTO(posX, posY, (char*)texto, anchoCaracter8);
}

// Dibuja el logo TETRIS en la cabecera del menú, centrado en el tercio superior
static void DIBUJAR_CABECERA(void)
{
    int ancho = CONFIG_ANCHO();
    int alto  = CONFIG_ALTO();
    int logoAncho = 167;
    int logoAlto  = 110;
    int logoX = (ancho - logoAncho) / 2;
    int logoY = (alto / 2 - logoAlto) / 2;
    if (logoY < 3) logoY = 3;
    DIBUJAR_LOGO_COMPLETO(logoX, logoY);
}

// -------------------------------------------------------
// MENU PRINCIPAL
// -------------------------------------------------------

// Muestra el menú principal con opciones dinámicas (JUGAR, CONTINUAR si hay guardado,
// ESTADISTICAS, INSTRUCCIONES, OPCIONES, SALIR).
// Navegación con flechas arriba/abajo, Enter para seleccionar.
// ESC salta a SALIR. Q cierra todo el juego.
// Retorna: MENU_RESULTADO_JUGAR, MENU_RESULTADO_CONTINUAR o MENU_RESULTADO_SALIR.
eMenuResultado MENU_PRINCIPAL(void)
{
    g_salir_juego = 0;

    while (1)
    {
        int hay_guardado = PARTIDA_EXISTE();

        // Construye la lista de opciones según haya o no partida guardada
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
        resultados[CANT_OPCIONES] = (eMenuResultado)-1; // -1 indica submenú
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
        int rehacerMenu = 0;

        while (!rehacerMenu)
        {
            // Recalcula layout (puede cambiar tras opciones si se cambió resolución)
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

            // Navegación con flechas
            if (gbt_tecla_presionada(GBTK_ARRIBA))
                seleccion = (seleccion - 1 + CANT_OPCIONES) % CANT_OPCIONES;
            if (gbt_tecla_presionada(GBTK_ABAJO))
                seleccion = (seleccion + 1) % CANT_OPCIONES;

            // Enter para seleccionar
            if (gbt_tecla_presionada(GBTK_ENTER))
            {
                eMenuResultado r = resultados[seleccion];
                if (r == MENU_RESULTADO_JUGAR || r == MENU_RESULTADO_CONTINUAR)
                    return r;
                if (r == MENU_RESULTADO_SALIR)
                    return r;
                // Submenús: se ejecutan y al volver se rehace el menú por si cambió algo
                if (r == (eMenuResultado)-1) { MENU_ESTADISTICAS();  if (g_salir_juego) return MENU_RESULTADO_SALIR; rehacerMenu = 1; }
                if (r == (eMenuResultado)-2) { MENU_INSTRUCCIONES(); if (g_salir_juego) return MENU_RESULTADO_SALIR; rehacerMenu = 1; }
                if (r == (eMenuResultado)-3) { MENU_OPCIONES();      if (g_salir_juego) return MENU_RESULTADO_SALIR; rehacerMenu = 1; }
            }

            // Q = salida global, ESC = salta a la opción SALIR
            if (gbt_tecla_sostenida(GBTK_q))
                return MENU_RESULTADO_SALIR;
            if (gbt_tecla_presionada(GBTK_ESCAPE))
                seleccion = CANT_OPCIONES - 1;

            // Dibujado del frame
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

// Menú de opciones: paleta, resolución, velocidad, modo, ancho tablero, guardar y volver.
// Usa una copia temporal (config_temp) y solo guarda al seleccionar "GUARDAR Y VOLVER".
// ESC vuelve sin guardar, Q sale del juego.
void MENU_OPCIONES(void)
{
    const int CANT_ITEMS = 6;
    // 0=Paleta, 1=Resolución, 2=Velocidad, 3=Modo, 4=Ancho tablero, 5=Guardar
    int seleccion = 0;
    int alto  = CONFIG_ALTO();
    int ancho = CONFIG_ANCHO();
    int primerItemY = alto / 2 - 30;
    int separacion  = 20;

    sConfig config_temp = config_actual; // Copia de trabajo

    while (1)
    {
        // Recalcular layout por si cambió resolución
        alto  = CONFIG_ALTO();
        ancho = CONFIG_ANCHO();
        primerItemY = alto / 2 - 30;

        gbt_procesar_entrada();

        // Navegación
        if (gbt_tecla_presionada(GBTK_ARRIBA))
            seleccion = (seleccion - 1 + CANT_ITEMS) % CANT_ITEMS;
        if (gbt_tecla_presionada(GBTK_ABAJO))
            seleccion = (seleccion + 1) % CANT_ITEMS;

        // Izquierda/Derecha: cambian el valor del ítem seleccionado
        if (gbt_tecla_presionada(GBTK_IZQUIERDA))
        {
            if (seleccion == 0) // Paleta: ciclo
                config_temp.paleta = (config_temp.paleta - 1 + CANT_PALETAS) % CANT_PALETAS;
            else if (seleccion == 1) // Resolución: 0↔1
                config_temp.resolucion = (config_temp.resolucion - 1 + 2) % 2;
            else if (seleccion == 2) // Velocidad: ciclo
                config_temp.velocidad_inicial = (config_temp.velocidad_inicial - 1 + CANT_VELOCIDADES) % CANT_VELOCIDADES;
            else if (seleccion == 3) // Modo: 0↔1
                config_temp.modo_juego = (config_temp.modo_juego - 1 + 2) % 2;
            else if (seleccion == 4) // Ancho: decremento con límite inferior 8
            {
                config_temp.ancho_tablero--;
                if (config_temp.ancho_tablero < 8) config_temp.ancho_tablero = MAX_COLUMNAS;
            }
        }
        if (gbt_tecla_presionada(GBTK_DERECHA))
        {
            if (seleccion == 0)
                config_temp.paleta = (config_temp.paleta + 1) % CANT_PALETAS;
            else if (seleccion == 1)
                config_temp.resolucion = (config_temp.resolucion + 1) % 2;
            else if (seleccion == 2)
                config_temp.velocidad_inicial = (config_temp.velocidad_inicial + 1) % CANT_VELOCIDADES;
            else if (seleccion == 3)
                config_temp.modo_juego = (config_temp.modo_juego + 1) % 2;
            else if (seleccion == 4) // Ancho: incremento con límite superior MAX_COLUMNAS
            {
                config_temp.ancho_tablero++;
                if (config_temp.ancho_tablero > MAX_COLUMNAS) config_temp.ancho_tablero = 8;
            }
        }

        // Enter: si está en "GUARDAR Y VOLVER", persiste los cambios
        if (gbt_tecla_presionada(GBTK_ENTER))
        {
            if (seleccion == 5)
            {
                config_actual = config_temp;
                piezas_en_uso = (config_actual.modo_juego == MODO_DX) ? MAX_PIEZAS : 7;
                columnasTablero = (config_actual.modo_juego == MODO_DX) ? config_actual.ancho_tablero : 10;
                CONFIG_GUARDAR(&config_actual);
                CONFIG_APLICAR(&config_actual);
                return;
            }
        }

        // ESC: vuelve sin guardar
        if (gbt_tecla_presionada(GBTK_ESCAPE) || gbt_tecla_sostenida(GBTK_ESCAPE))
            return;
        // Q: sale del juego
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

        // Ítems: cada uno con su valor actual
        {
            char linea[40];
            sprintf(linea, "PALETA  %s", NOMBRES_PALETAS[config_temp.paleta]);
            DIBUJAR_OPCION(primerItemY + 0 * separacion, linea, seleccion == 0);
        }
        {
            char linea[40];
            sprintf(linea, "RESOL   %s", NOMBRES_RESOLUCIONES[config_temp.resolucion]);
            DIBUJAR_OPCION(primerItemY + 1 * separacion, linea, seleccion == 1);
        }
        {
            char linea[40];
            sprintf(linea, "VEL     %s", nombres_velocidades[config_temp.velocidad_inicial]);
            DIBUJAR_OPCION(primerItemY + 2 * separacion, linea, seleccion == 2);
        }
        {
            char linea[40];
            sprintf(linea, "MODO    %s", NOMBRES_MODOS[config_temp.modo_juego]);
            DIBUJAR_OPCION(primerItemY + 3 * separacion, linea, seleccion == 3);
        }
        // Ancho: editable solo en modo DX; en clásico muestra "10 (FIJO)"
        {
            char linea[40];
            if (config_temp.modo_juego == MODO_DX)
                sprintf(linea, "ANCHO   %d", config_temp.ancho_tablero);
            else
                sprintf(linea, "ANCHO   10 (FIJO)");
            DIBUJAR_OPCION(primerItemY + 4 * separacion, linea, seleccion == 4);
        }

        DIBUJAR_OPCION(primerItemY + 5 * separacion, "GUARDAR Y VOLVER", seleccion == 5);

        // Leyenda
        DIBUJARTEXTO((ancho - (int)strlen("ESC VOLVER SIN GUARDAR") * anchoCaracter8) / 2,
                     alto - 20, "ESC VOLVER SIN GUARDAR", anchoCaracter8);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}

// -------------------------------------------------------
// ESTADISTICAS
// -------------------------------------------------------

// Muestra la tabla de puntajes máximos (hasta 5 entradas) ordenados de mayor a menor.
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
            return;
        if (gbt_tecla_sostenida(GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        gbt_borrar_backbuffer(COLOR_NEGRO);
        DIBUJARFONDO();

        // Diseño de tabla de dos columnas: NOMBRE y PUNTAJE
        int anchoNombre = 13 * anchoCaracter8;
        int anchoPuntaje = 7 * anchoCaracter8;
        int gapColumnas = 20;
        int anchoTabla = anchoNombre + gapColumnas + anchoPuntaje;
        int colNombre = (ancho - anchoTabla) / 2;
        int colPuntaje = colNombre + anchoNombre + gapColumnas;

        // Altura total del contenido y centrado vertical
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

        // Datos del ranking
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

        // Instrucción al pie
        int instrX = (ancho - (int)strlen("ENTER O ESC PARA VOLVER") * anchoCaracter8) / 2;
        DIBUJARTEXTO(instrX, alto - 20, "ENTER O ESC PARA VOLVER", anchoCaracter8);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}

// -------------------------------------------------------
// INSTRUCCIONES
// -------------------------------------------------------

// Muestra el objetivo del juego y la tabla de controles en dos columnas centradas.
void MENU_INSTRUCCIONES(void)
{
    while (1)
    {
        gbt_procesar_entrada();

        if (gbt_tecla_presionada(GBTK_ESCAPE) || gbt_tecla_sostenida(GBTK_ESCAPE)
            || gbt_tecla_presionada(GBTK_ENTER))
            return;
        if (gbt_tecla_sostenida(GBTK_q))
        {
            g_salir_juego = 1;
            return;
        }

        int alto  = CONFIG_ALTO();
        int ancho = CONFIG_ANCHO();

        gbt_borrar_backbuffer(COLOR_NEGRO);
        DIBUJARFONDO();

        // Datos: objetivo + controles en dos columnas
        const char *obj[] = { "OBJETIVO", "COMPLETAR FILAS SIN LLEGAR AL TOPE", "" };
        int num_obj = sizeof(obj) / sizeof(obj[0]);

        const char *teclas[] = { "IZQ/DER", "ABAJO", "A", "D", "P", "C", "Q" };
        const char *desc[]   = { "MOVER PIEZA", "BAJAR MAS RAPIDO", "ROTAR ANTIHORARIO",
                                 "ROTAR HORARIO", "PAUSAR", "CHEAT (LENTITUD 5S)", "SALIR AL MENU" };
        int filas = sizeof(teclas) / sizeof(teclas[0]);

        const char *tit_ctrl = "CONTROLES";

        int total_lineas = num_obj + 1 + filas; // obj + título controles + filas
        int sep = (alto > 300) ? 12 : 6; // separación entre líneas (CGA=6, VGA=12)

        // Calcula el ancho de cada columna para centrar todo horizontalmente
        int max_tecla = 0, max_desc = 0;
        for (int i = 0; i < filas; i++)
        {
            int lt = (int)strlen(teclas[i]);
            int ld = (int)strlen(desc[i]);
            if (lt > max_tecla) max_tecla = lt;
            if (ld > max_desc) max_desc = ld;
        }

        int ancho_teclas = max_tecla * anchoCaracter8;
        int ancho_desc   = max_desc * anchoCaracter8;
        int gap = 16;
        int ancho_total  = ancho_teclas + gap + ancho_desc;
        int centro_cols  = (ancho - ancho_total) / 2;
        int col_tecla_x  = centro_cols;
        int col_desc_x   = centro_cols + ancho_teclas + gap;

        // Centrado vertical de todo el contenido
        int alto_texto = total_lineas * (altoCaracter + sep) - sep;
        int y_inicio = (alto - alto_texto) / 2;
        int y = y_inicio;

        // Sección OBJETIVO
        for (int i = 0; i < num_obj; i++)
        {
            if (obj[i][0] != '\0')
            {
                int x = (ancho - (int)strlen(obj[i]) * anchoCaracter8) / 2;
                DIBUJARTEXTO(x, y, (char*)obj[i], anchoCaracter8);
            }
            y += altoCaracter + sep;
        }

        // Título CONTROLES centrado
        {
            int x = (ancho - (int)strlen(tit_ctrl) * anchoCaracter8) / 2;
            DIBUJARTEXTO(x, y, (char*)tit_ctrl, anchoCaracter8);
        }
        y += altoCaracter + sep;

        // Tabla de controles (tecla | descripción)
        for (int i = 0; i < filas; i++)
        {
            DIBUJARTEXTO(col_tecla_x, y, (char*)teclas[i], anchoCaracter8);
            DIBUJARTEXTO(col_desc_x,  y, (char*)desc[i],   anchoCaracter8);
            y += altoCaracter + sep;
        }

        // Leyenda fija al pie
        int instrX = (ancho - (int)strlen("ENTER O ESC PARA VOLVER") * anchoCaracter8) / 2;
        DIBUJARTEXTO(instrX, alto - 20, "ENTER O ESC PARA VOLVER", anchoCaracter8);

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}

// -------------------------------------------------------
// MENU PAUSA
// -------------------------------------------------------

// Menú de pausa superpuesto al juego: CONTINUAR, GUARDAR Y SALIR AL MENU, SALIR SIN GUARDAR.
// Flechas para navegar, Enter para seleccionar, ESC/P para continuar.
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

        // Tamaño de la caja de pausa (suficiente para la opción más larga)
        int anchoCaja = (int)strlen("SALIR AL MENU SIN GUARDAR") * anchoCaracter8 + 72;
        int altoCaja  = 16 + CANT_OPCIONES * 16 + 8;
        int x0 = centroX - anchoCaja / 2;
        int y0 = centroY - altoCaja  / 2;

        gbt_procesar_entrada();

        if (gbt_tecla_presionada(GBTK_ARRIBA))
            seleccion = (seleccion - 1 + CANT_OPCIONES) % CANT_OPCIONES;
        if (gbt_tecla_presionada(GBTK_ABAJO))
            seleccion = (seleccion + 1) % CANT_OPCIONES;

        // Enter confirma selección; P/ESC también continúa
        if (gbt_tecla_presionada(GBTK_ENTER) || gbt_tecla_presionada(GBTK_p))
        {
            if (seleccion == 0) return PAUSA_RESULTADO_CONTINUAR;
            if (seleccion == 1) return PAUSA_RESULTADO_GUARDAR_Y_SALIR;
            if (seleccion == 2) return PAUSA_RESULTADO_SALIR_SIN_GUARDAR;
        }
        if (gbt_tecla_presionada(GBTK_ESCAPE))
            return PAUSA_RESULTADO_CONTINUAR;

        // Dibuja el juego de fondo y el panel de pausa encima
        DIBUJAR();
        DIBUJARMARCOGENERICO(x0, y0, anchoCaja, altoCaja, 0);

        char *titulo = "PAUSA";
        DIBUJARTEXTO(centroX - ((int)strlen(titulo) * anchoCaracter8) / 2, y0 + 4, titulo, anchoCaracter8);

        for (int i = 0; i < CANT_OPCIONES; i++)
        {
            int opY = y0 + 16 + i * 16;
            DIBUJAR_OPCION(opY, opciones[i], i == seleccion);
        }

        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}
