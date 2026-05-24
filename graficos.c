#include "graficos.h"
#include "inicio.h"
#include "gbt.h"
#include <stdio.h>
#include <string.h>

// Índices de colores para el brillo de las piezas
int colorBrillo [cantColores] = {0,9,10,11,12,13,15,15,7,15,15,15,15,15,15,15};

// Índices de colores para la sombra de las piezas
int colorSombra [cantColores] = {0,1,2,3,4,5,4,8,0,1,2,3,4,5,6,7};

extern int pieza_fijada_sin_nueva; // Indica si la pieza actual ya se fijó pero no se generó nueva

void DIBUJAR () // Dibuja el frame del juego
{
    int fTablero, cTablero; // Índices del tablero (fila, columna)
    int fPieza, cPieza; // Índices de la pieza actual (4×4)
    int posXPantalla, posYPantalla; // Posición en la ventana donde se dibuja el bloque
    int pixelXBloque, pixelYBloque; // Píxeles internos de cada mino
    int ocupado = 0; // Color de la celda ocupada (0 = vacía)
    int colorBase, colorFinal; // Color del bloque y color resultante con brillo y sombra
    int colPiezaMundo; // Columna del tablero donde está el bloque de la pieza actual

    gbt_borrar_backbuffer (0); // Limpia toda la ventana

    // Capas de dibujo
    DIBUJARFONDO (); // Fondo
    DIBUJARMARCO (); // Marco alrededor del tablero
    DIBUJARGRILLA (); // Líneas divisorias entre celdas del tablero
    DIBUJARPUNTAJE (); // Texto: SCORE, LEVEL, LINES, SPEED
    DIBUJARPROXIMA (); // Panel NEXT con la próxima pieza
    DIBUJARCHEAT (); // Indicador "CHEAT"

    // Leyenda PLAYER + nombre del jugador ingresado
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20 - 10, "PLAYER ", anchoCaracter8);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20, nombreJugador, anchoCaracter8);

    for (fTablero = FILAS_INVISIBLES; fTablero < FILAS_TOTALES; fTablero ++) // Recorre las filas visibles
    {
        for (cTablero = 0; cTablero < columnasTablero; cTablero ++) // Recorre las columnas de la fila actual
        {
            ocupado = tablero [fTablero][cTablero]; // Toma el color de la celda en cuestión. 0 = vacío, > 0 tiene color

            if (!pieza_fijada_sin_nueva) // Si la pieza actual sigue activa, superpone la posición del tablero
            {
                // Recorre la matriz de la pieza actual
                for (fPieza = 0; fPieza < 4; fPieza ++)
                {
                    for (cPieza = 0; cPieza < 4; cPieza ++)
                    {
                        if (actual.forma [fPieza][cPieza] == 1) // Evalúa si hay un mino en la matriz de la pieza
                        {
                            // Calcula la columna del tablero donde está este bloque de la pieza
                            colPiezaMundo = actual.columna + cPieza;

                            // Evaluación para tablero circular de modo DX
                            if (config_actual.modo_juego == MODO_DX)
                            {
                                if (colPiezaMundo < 0)
                                {
                                    colPiezaMundo += columnasTablero;
                                }
                                if (colPiezaMundo >= columnasTablero)
                                {
                                    colPiezaMundo -= columnasTablero;
                                }
                            }

                            // Si la pieza está en esta celda del tablero visible, sobreescribe con el color de la pieza
                            if (actual.fila + fPieza == fTablero && colPiezaMundo == cTablero)
                            {
                                ocupado = actual.color;
                            }
                        }
                    }
                }
            }

            // Si la celda está ocupada dibuja el bloque
            if (ocupado != 0)
            {
                colorBase = ocupado;  // Color del bloque

                // Calculo de posición del bloque
                posXPantalla = offsetHorizontal + cTablero * tamMino - 1;
                posYPantalla = offsetVertical + (fTablero - FILAS_INVISIBLES) * tamMino - 1;

                // Dibuja cada píxel del mino (tamMino+1 × tamMino+1). +1 para solaparse con la grilla
                for (pixelYBloque = 0; pixelYBloque <= tamMino; pixelYBloque ++)
                {
                    for (pixelXBloque = 0; pixelXBloque <= tamMino; pixelXBloque ++)
                    {
                        colorFinal = colorBase; // Color base del mino

                        // Brillo en borde superior e izquierdo (2 píxeles)
                        if (pixelYBloque <= 1 || pixelXBloque <= 1)
                        {
                            colorFinal = colorBrillo [colorBase];
                        }
                        // Sombra en borde inferior y derecho (2 píxeles)
                        else if (pixelYBloque >= tamMino - 2 || pixelXBloque >= tamMino - 2)
                        {
                            colorFinal = colorSombra [colorBase];
                        }

                        // Dibuja el píxel en el backbuffer
                        gbt_dibujar_pixel (posXPantalla + pixelXBloque, posYPantalla + pixelYBloque, colorFinal);
                    }
                }
            }
        }
    }
}

void DIBUJARFONDO () // Fondo del juego (color + grilla)
{
    int x, y, color;

    // Recorre todos los píxeles de la ventana
    for (y = 0; y < altoVentana; y ++)
    {
        for (x = 0; x < anchoVentana; x ++)
        {
            color = 1;  // Color base del fondo

            // Cuadrícula decorativa cada múltiplos de 16
            if ((x % 16 == 0) || (y % 16 == 0))
            {
                color = 8; // Color cuadrícula
            }

            gbt_dibujar_pixel(x, y, color); // Pinta el píxel en el backbuffer
        }
    }
}

void DIBUJARMARCO () // Dibuja el marco alrededor del tablero
{
    // El marco envuelve el tablero con 4 píxeles de padding a cada lado
    int x0 = offsetHorizontal - 4;
    int y0 = offsetVertical - 4;
    int ancho = columnasTablero * tamMino + 8;  // +4 izq +4 der
    int alto  = filasTablero * tamMino + 8;     // +4 sup +4 inf
    DIBUJARMARCOGENERICO(x0, y0, ancho, alto, 0);
}

void DIBUJARMARCOGENERICO (int x0, int y0, int ancho, int alto, int colorFondo) // Dibuja marco rectangular
{
    int x, y, color;

    // Recorre todos los píxeles del rectángulo
    for (y = y0; y < y0 + alto; y ++)
    {
        for (x = x0; x < x0 + ancho; x ++)
        {
            color = colorFondo;  // Color del interior

            // Bordes superior (y ≤ y0 + 1) e izquierdo (x ≤ x0 + 1): brillo
            if (y <= y0 + 1 || x <= x0 + 1)
            {
                color = 7;
            }
            // Bordes inferior (y ≥ y0 + alto - 2) y derecho (x ≥ x0 + ancho - 2): sombra
            else if (y >= y0 + alto - 2 || x >= x0 + ancho - 2)
            {
                color = 8;
            }

            gbt_dibujar_pixel(x, y, color); // Pinta el píxel en el backbuffer
        }
    }
}

void DIBUJARGRILLA () // Dibuja la grilla interna del tablero (líneas divisorias entre las celdas)
{
    int fila, columna, x, y;

    // Líneas horizontales cada tamMino píxeles
    for (fila = 0; fila <= filasTablero; fila ++)
    {
        y = offsetVertical + fila * tamMino;
        for (x = offsetHorizontal; x <= offsetHorizontal + columnasTablero * tamMino; x ++)
        {
            gbt_dibujar_pixel (x, y, 8);
        }
    }

    // Líneas verticales cada tamMino píxeles
    for (columna = 0; columna <= columnasTablero; columna ++)
    {
        x = offsetHorizontal + columna * tamMino;
        for (y = offsetVertical; y <= offsetVertical + filasTablero * tamMino; y ++)
        {
            gbt_dibujar_pixel(x, y, 8);
        }
    }
}

void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color) // Dibuja un carácter individual en la pantalla
{
    int filaCaracter, columnaCaracter;
    // Recorre matriz caracter
    for (filaCaracter = 0; filaCaracter < altoCaracter; filaCaracter ++)
    {
        for (columnaCaracter = 0; columnaCaracter < anchoCaracter; columnaCaracter ++)
        {
            // Si es fuente 8×8 usa el array fuente8x8
            if (anchoCaracter == anchoCaracter8)
            {
                if (fuente8x8 [caracter][filaCaracter][columnaCaracter] == 1)
                    gbt_dibujar_pixel (posXPantalla + columnaCaracter, posYPantalla + filaCaracter, color);
            }
            // Si es fuente 8×16 usa el array fuente8x16
            if (anchoCaracter == anchoCaracter16)
            {
                if (fuente8x16 [caracter][filaCaracter][columnaCaracter] == 1)
                {
                    gbt_dibujar_pixel (posXPantalla + columnaCaracter, posYPantalla + filaCaracter, color);
                }
            }
        }
    }
}

void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter) // Dibuja una cadena de texto en pantalla
{
    int i = 0, caracter;
    while (texto [i] != '\0')
    {
        caracter = -1;  // -1 = carácter no soportado (no se dibuja)

        // Convierte el carácter ASCII a índice en el array de fuentes
        if (texto [i] >= 'A' && texto [i] <= 'Z') // Letras
        {
            caracter = texto [i] - 'A';
        }
        else if (texto [i] >= '0' && texto [i] <= '9') // Números
        {
            caracter = 26 + (texto [i] - '0');
        }
        else if (texto [i] == ' ') // Espacio
        {
            caracter = 36;
        }
        
        if (caracter != -1) // Si el carácter es válido, lo dibuja
        {
            DIBUJARCARACTER (posXPantalla + i * anchoCaracter, posYPantalla, caracter, anchoCaracter, 7);
        }
        i ++ ;
    }
}

// Dibuja el indicador "CHEAT" en el panel izquierdo
void DIBUJARCHEAT ()
{
    int ancho = (int) strlen ("CHEAT") * anchoCaracter8; // Ancho de la palabra en píxeles
    int x0 = offsetHorizontal - 74; // Posición X fija a la izquierda del tablero
    int y0 = offsetVertical + 4 * tamMino + 26 + 10; // Posición Y debajo del panel NEXT
    int color; // Color del texto (gris claro u oscuro según estado del cheat)
    const char *txt; // Puntero al texto "CHEAT"
    int i; // Índice para recorrer el texto

    // Fondo negro para que el texto se lea bien
    DIBUJAR_RECTANGULO (x0 - 2, y0 - 2, ancho + 4, altoCaracter + 4, COLOR_NEGRO);

    // Decide el color según el estado del cheat
    if (cheat_cooldown_restante > 0.0 && !cheat_activo)
    {
        color = 8;
    }
    else 
    {
        color = 7;
    }

    // Dibuja letra por letra
    txt = "CHEAT";
    for (i = 0; txt[i] != '\0'; i++)
    {
        if (txt[i] >= 'A' && txt[i] <= 'Z')
        {
            DIBUJARCARACTER(x0 + i * anchoCaracter8, y0, txt[i] - 'A', anchoCaracter8, color);
        }
    }
}

// Dibuja texto con fuente no monoespaciada
void DIBUJARTEXTOPROP (int x, int y, const char *texto, int color)
{
    int px = x;  // Posición X actual del cursor de dibujo
    const char *p; // Puntero para recorrer el texto
    int idx; // Índice en el array de fuentes
    int w; // Ancho real del carácter recortado
    int fil, col; // Píxeles internos del carácter

    for (p = texto; *p != '\0'; p++) // Recorre el string
    {
        idx = -1; // Índice en el array de fuentes

        // Convierte carácter a índice
        if (*p >= 'A' && *p <= 'Z') // Letras
        {
            idx = *p - 'A';
        }
        else if (*p >= '0' && *p <= '9') // Números
        {
            idx = 26 + (*p - '0');
        }
        else if (*p == ' ') // Espacio
        {
            idx = 36;
        }
        else if (*p == '_') // Guión
        {
            idx = 37;
        }
        if (idx >= 0) // Si el caracter es válido, pone el ancho real de este carácter
        {
            w = anchoProp [idx];  

            // Dibuja los píxeles del carácter
            for (fil = 0; fil < altoCaracter; fil++)
            {
                for (col = 0; col < w; col++)
                {
                    if (fuenteProp[idx][fil][col])
                    {
                        gbt_dibujar_pixel(px + col, y + fil, color);
                    }
                }
            }

            px += w + 1;  // Avanza: ancho del carácter + 1 píxel de interletrado
        }
        else
        {
            px += 5;  // Carácter desconocido: salto fijo de 5 píxeles
        }
    }
}

void DIBUJARPROXIMA () // Dibuja el panel NEXT (próxima pieza) a la izquierda del tablero
{
    int f, c, px, py, x0, y0, colorBase, colorFinal, pixelX, pixelY, anchoPanel, altoPanel, minC, maxC, minF, maxF, anchoInterior, anchoPieza, altoPieza, offsetX, offsetY;

    // Posición del panel: a la izquierda del tablero, alineado con el borde superior
    x0 = offsetHorizontal - 80;
    y0 = offsetVertical;

    // Tamaño del panel: 4×4 minos (tamMino) + bordes + etiqueta
    anchoPanel = 4 * tamMino + 16;
    altoPanel  = 4 * tamMino + 26;

    // Dibuja el marco del panel
    DIBUJARMARCOGENERICO (x0 - 4, y0 - 4, anchoPanel, altoPanel, 0);

    // Etiqueta NEXT arriba del panel
    DIBUJARTEXTO (x0, y0, "NEXT", anchoCaracter8);

    // Durante la animación de borrado de líneas no se dibuja la pieza
    if (animacion_borrado_activa)
    {
        return;
    }

    // Calcula la caja delimitadora de la pieza para centrarla
    minC = 4; // Valor máximo para asegurarse de que entre
    maxC = -1; // Valor mínimo para que se actualice después
    minF = 4; // Valor máximo para asegurarse de que entre
    maxF = -1; // Valor mínimo para que se actualice después

    // Recorre matriz de próxima pieza
    for (f = 0; f < 4; f ++) 
    {
        for (c = 0; c < 4; c ++)
        {
            if (proxima.forma [f][c] == 1) // Evalúa si hay un mino en la posición actual para actualizar los valores de la caja delimitadora
            {
                if (c < minC)
                {
                    minC = c;
                }
                if (c > maxC)
                {
                    maxC = c;
                }
                if (f < minF)
                {
                    minF = f;
                }
                if (f > maxF)
                {
                    maxF = f;
                }
            }
        }
    }

    // Dimensiones ocupadas por la pieza en píxeles
    anchoInterior = anchoPanel - 16;   // Espacio disponible dentro del marco
    anchoPieza = (maxC - minC + 1) * tamMino;  // Ancho real de la pieza
    altoPieza  = (maxF - minF + 1) * tamMino;  // Alto real de la pieza

    // Offset para centrar la pieza dentro del panel
    offsetX = (anchoInterior - anchoPieza) / 2 - minC * tamMino;
    offsetY = (anchoInterior - altoPieza)  / 2 - minF * tamMino;

    // Recorre la matriz de la pieza para dibujarla
    for (f = 0; f < 4; f ++)
    {
        for (c = 0; c < 4; c ++)
        {
            if (proxima.forma [f][c] == 1) // Si hay un mino en la posición actual, lo dibuja
            {
                colorBase = proxima.color;  // Color de la pieza

                // Posición del bloque en pantalla
                px = x0 + offsetX + c * tamMino;
                py = y0 + 15 + offsetY + f * tamMino;  // +15 por la etiqueta NEXT

                // Dibuja cada píxel del mino
                for (pixelY = 0; pixelY <= tamMino; pixelY++)
                {
                    for (pixelX = 0; pixelX <= tamMino; pixelX++)
                    {
                        colorFinal = colorBase; // Color de la pieza
                        if (pixelY <= 1 || pixelX <= 1) // Evalúa posición del píxel para poner brillo
                        {
                            colorFinal = colorBrillo [colorBase];
                        }
                        else if (pixelY >= tamMino - 2 || pixelX >= tamMino - 2) // Evalúa posición del píxel para poner sombra
                        {
                            colorFinal = colorSombra[colorBase];
                        }

                        gbt_dibujar_pixel(px + pixelX, py + pixelY, colorFinal);
                    }
                }
            }
        }
    }
}

void DIBUJARPUNTAJE () // Dibuja el panel de estadísticas a la derecha del tablero. 20 píxeles entre cada línea de texto
{
    char textoPuntaje [20];  // Buffer temporal para formatear el texto

    sprintf (textoPuntaje, "SCORE %d", puntaje);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "LEVEL %d", nivel);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 20, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "LINES %d", lineas_totales);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 40, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "SPEED %d", velocidad);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 60, textoPuntaje, anchoCaracter8);
}

void DIBUJARPAUSA () // Dibuja la superposición de pausa centrada en la pantalla
{
    int anchoCaja, altoCaja, centroX, centroY, x0, y0;
    char *linea1, *linea2; // Texto "PAUSA", texto "P CONTINUAR / Q SALIR"

    anchoCaja = 200;
    altoCaja = 50;
    centroX = anchoVentana / 2;
    centroY = altoVentana / 2;
    x0 = centroX - anchoCaja / 2;
    y0 = centroY - altoCaja / 2;

    DIBUJARMARCOGENERICO(x0, y0, anchoCaja, altoCaja, 0); // Marco

    // Texto del título y las opciones
    linea1 = "PAUSA";
    linea2 = "P CONTINUAR   Q SALIR";

    // Dibuja el texto centrado
    DIBUJARTEXTO (centroX - ((int) strlen (linea1) * anchoCaracter8) / 2, y0 + 10, linea1, anchoCaracter8);
    DIBUJARTEXTO (centroX - ((int) strlen (linea2) * anchoCaracter8) / 2, y0 + 30, linea2, anchoCaracter8);
}

void DIBUJARGAMEOVER () // Dibuja la superposición de Game Over centrada en la pantalla
{
    int anchoCaja, altoCaja, centroX, centroY, x0, y0;
    char puntajeFinal [32]; // Buffer para el texto del puntaje final
    char *linea1, *linea2, *linea3, *linea4, *linea5; // "GAME OVER", "SCORE <puntaje>", "R REINICIAR", "ENTER MENU PRINCIPAL", "Q SALIR"
    anchoCaja = 180;
    altoCaja  = 90;
    centroX = anchoVentana / 2;
    centroY = altoVentana  / 2;
    x0 = centroX - anchoCaja / 2;
    y0 = centroY - altoCaja  / 2;

    DIBUJARMARCOGENERICO(x0, y0, anchoCaja, altoCaja, 0);
    
    sprintf (puntajeFinal, "SCORE %d", puntaje); // Prepara el texto del puntaje final

    // Líneas de texto
    linea1 = "GAME OVER";
    linea2 = puntajeFinal;
    linea3 = "R     REINICIAR";
    linea4 = "ENTER MENU PRINCIPAL";
    linea5 = "Q     SALIR";

    // Dibuja el texto centrado
    DIBUJARTEXTO (centroX - ((int) strlen (linea1) * anchoCaracter8) / 2, y0 + 8,  linea1, anchoCaracter8);
    DIBUJARTEXTO (centroX - ((int) strlen (linea2) * anchoCaracter8) / 2, y0 + 22, linea2, anchoCaracter8);
    DIBUJARTEXTO (centroX - ((int) strlen (linea3) * anchoCaracter8) / 2, y0 + 42, linea3, anchoCaracter8);
    DIBUJARTEXTO (centroX - ((int) strlen (linea4) * anchoCaracter8) / 2, y0 + 55, linea4, anchoCaracter8);
    DIBUJARTEXTO (centroX - ((int) strlen (linea5) * anchoCaracter8) / 2, y0 + 68, linea5, anchoCaracter8);
}

void DIBUJARINICIO(char *nombre) // Pantalla de ingreso de nombre del jugador (al iniciar una nueva partida)
{
    int i = 0, terminado = 0, logoY, textY, nameY, escY;
    eGBT_Tecla tecla;

    nombre [0] = '\0';   // Inicializa el nombre como cadena vacía

    // Loop principal de ingreso de nombre
    while (!terminado)
    {
        // Procesa los eventos de entrada de GBT
        gbt_procesar_entrada();
        tecla = gbt_obtener_tecla_presionada();

        if (tecla != GBTK_DESCONOCIDA) // Evalúa si se presionó una tecla
        {
            if (tecla == GBTK_ESCAPE) // ESC: cancela el ingreso
            {
                nombre[0] = '\0'; // Señal de cancelación para el llamador
                return;
            }
            else if (tecla == GBTK_ENTER) // Enter: confirma (solo si hay al menos 1 carácter)
            {
                if (i > 0)
                {
                    terminado = 1;
                }
            }
            else if (tecla == GBTK_RETROCESO) // Retroceso: borra el último carácter ingresado
            {
                if (i > 0)
                {
                    i--;
                    nombre [i] = '\0';
                }
            }
            else if (tecla >= 'a' && tecla <= 'z') // Letras minúsculas: convertir a mayúscula
            {
                if (i < 13) // Máximo 13 caracteres de largo
                {
                    nombre [i] = tecla - 32; // ASCII: 'a' (97) - 32 = 'A' (65)
                    i ++;
                    nombre [i] = '\0';
                }
            }
            else if (tecla >= '0' && tecla <= '9') // Números 0-9
            {
                if (i < 13)
                {
                    nombre [i] = tecla;
                    i ++;
                    nombre [i] = '\0';
                }
            }
            else if (tecla == GBTK_ESPACIO) // Espacio: solo permitido si ya hay al menos 1 carácter (no empezar con espacio)
            {
                if (i < 13 && i > 0)
                {
                    nombre[i] = ' ';
                    i ++;
                    nombre [i] = '\0';
                }
            }
        }

        // Dibujado de pantalla
        gbt_borrar_backbuffer(0);
        DIBUJARFONDO();

        // Logo TETRIS centrado en el tercio superior de la pantalla
        logoY = (altoVentana / 2 - 110) / 2;
        if (logoY < 3)
        {
            logoY = 3;  // Mínimo 3 píxeles del borde superior
        }
        DIBUJAR_LOGO_COMPLETO ((anchoVentana - 167) / 2, logoY);

        // Grupo de texto debajo del logo
        textY = logoY + 110 + (altoVentana - (logoY + 110) - 8 - 10 - 8 - 10 - 8) / 2;
        nameY = textY + 18;
        escY = nameY + 18;

        DIBUJARTEXTO ((anchoVentana - 18 * anchoCaracter8) / 2, textY, "INGRESE SU NOMBRE:", anchoCaracter8);
        DIBUJARTEXTO ((anchoVentana - 13 * anchoCaracter8) / 2, nameY, nombre, anchoCaracter8);

        // Cursor: carácter 37 (guión bajo '_' en la fuente). Solo se muestra si no se llegó al límite de 13 caracteres
        if (i < 13)
        {
            DIBUJARCARACTER ((anchoVentana - 13 * anchoCaracter8) / 2 + i * anchoCaracter8,nameY, 37, anchoCaracter8, 7);
        }

        // Instrucción al pie
        DIBUJARTEXTO ((anchoVentana - 19 * anchoCaracter8) / 2, escY, "ESC VOLVER AL MENU", anchoCaracter8);

        // Vuelca el backbuffer a la pantalla y espera 16ms (~60 FPS)
        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}