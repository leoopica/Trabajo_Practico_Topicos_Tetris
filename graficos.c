#include "graficos.h"
#include "sprites.h"
#include <stdlib.h>
#include <time.h>

// Definición del tablero
int tablero [filasTablero][columnasTablero] = {0};

// Paleta de colores
tGBT_ColorRGB paletaCGA [cantColores] =
{
    {0x00, 0x00, 0x00}, // 0:   Negro
    {0x00, 0x00, 0xAA}, // 1:   Azul
    {0x00, 0xAA, 0x00}, // 2:   Verde
    {0x00, 0xAA, 0xAA}, // 3:   Cian
    {0xAA, 0x00, 0x00}, // 4:   Rojo
    {0xAA, 0x00, 0xAA}, // 5:   Magenta
    {0xAA, 0x55, 0x00}, // 6:   Marron
    {0xAA, 0xAA, 0xAA}, // 7:   Gris claro
    {0x55, 0x55, 0x55}, // 8:   Gris oscuro
    {0x55, 0x55, 0xFF}, // 9:   Azul brillante
    {0x55, 0xFF, 0x55}, // 10:  Verde brillante
    {0x55, 0xFF, 0xFF}, // 11:  Cian brillante
    {0xFF, 0x55, 0x55}, // 12:  Rojo brillante
    {0xFF, 0x55, 0xFF}, // 13:  Magenta brillante
    {0xFF, 0xFF, 0x55}, // 14:  Amarillo
    {0xFF, 0xFF, 0xFF}  // 15:  Usado como transparente por GBT
};

int colorBrillo [cantColores] = {0,9,10,11,12,13,15,15,7,15,15,15,15,15,15,15}; // Paleta de colores para la parte de brillo
int colorSombra [cantColores] = {0,1,2,3,4,5,4,8,0,1,2,3,4,5,6,7}; // Paleta de colores para la parte de sombra

int puntaje = 0;
int nivel = 1;
int lineas_totales = 0;
int piezas_caidas = 0;
double duracion_caida = 1.0;

char nombreJugador [21]; // HAY QUE CAMBIARLO, PROBABLEMENTE

sPieza actual;

void NUEVAPIEZA ()
{
    int tipo; // Pieza que va a salir
    int colorPiezas [cantPiezas] = {11, 9, 6, 14, 10, 13, 12}; // Asigna un color a cada pieza
    tipo = rand () % cantPiezas; // Randomiza la pieza
    actual.tipo = tipo;
    actual.rotacion = 0;
    COPIARPIEZA (actual.forma, piezas [tipo]); // Busca la pieza en el catálogo de acuerdo al número randomizado en TIPO, y copia la forma de dicha pieza en la pieza actual
    actual.fila = -1; // Ubica la pieza ligeramente arriba (SRS prefiere esto para el I)
    actual.columna = columnasTablero / 2 - 2; // Ubica la pieza en la mitad horizontal del tablero
    actual.color = colorPiezas [tipo]; // Pone el color

    piezas_caidas ++;
    if (piezas_caidas > 1 && (piezas_caidas - 1) % 10 == 0)
    {
        duracion_caida /= 1.03;
    }
}

void COPIARPIEZA (int destino [4][4], int origen [4][4])
{
    int f, c;
    for (f = 0; f < 4; f ++) // Recorre filasTablero de la matriz de la forma
    {
        for (c = 0; c < 4; c ++) // Recorre columnasTablero de la matriz de la forma
        {
            destino [f][c] = origen [f][c]; // Copia el pixel de la posición de origen en el destino
        }
    }
}

void DIBUJAR ()
{
    int fTablero, cTablero, fPieza, cPieza; // filasTablero y columnasTablero de tablero y de matriz de pieza
    int posXPantalla, posYPantalla; // Posición horizontal y vertical
    int pixelXBloque, pixelYBloque; // Pixel horizontal y vertical de la pieza
    int ocupado = 0, colorBase, colorFinal; // Ubicación ocupada, color original del bloque, color que va a dibujarse en el pixel

    gbt_borrar_backbuffer (0); // Limpia pantalla (negro)

    DIBUJARFONDO ();
    DIBUJARMARCO ();
    DIBUJARGRILLA ();
    DIBUJARPUNTAJE ();
    DIBUJARTEXTO(offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20 - 10, "PLAYER ", anchoCaracter8); // MODIFICAR PARA QUE SEA MÁS SIMPLE?
    DIBUJARTEXTO(offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + filasTablero * tamMino - 20, nombreJugador, anchoCaracter8); // MODIFICAR PARA QUE SEA MÁS SIMPLE?

    for (fTablero = 0; fTablero < filasTablero; fTablero ++) // Recorre filasTablero del tablero
    {
        for (cTablero = 0; cTablero < columnasTablero; cTablero ++) // Recorre columnasTablero del tablero
        {
            ocupado = tablero [fTablero][cTablero]; // Verifica si ya hay un mino en esa posición
            
            for (fPieza = 0; fPieza < 4; fPieza ++) // Recorre filasTablero de matriz de la pieza
            {
                for (cPieza = 0; cPieza < 4; cPieza ++) // Recorre columnasTablero de matriz de la pieza
                {
                    if (actual.forma [fPieza][cPieza] == 1) // Verifica si la matriz de la pieza tiene un mino en esa posición
                    {
                        if (actual.fila + fPieza == fTablero && actual.columna + cPieza == cTablero) // Verifica si el mino en cuestión está en cierta posición del tablero
                        {
                            ocupado = actual.color; // Indica que está ocupado por la pieza
                        }
                    }
                }
            }

            if (ocupado != 0) // Si está ocupado, lo dibuja
            {
                colorBase = ocupado; // Guarda el color base de la pieza a dibujar
                // Dibujar bloque
                posXPantalla = offsetHorizontal + cTablero * tamMino -1; // Convierte coordenadas horizontales del tablero en coordenadas en pantalla
                posYPantalla = offsetVertical + fTablero * tamMino -1; // Convierte coordenadas verticales del tablero en coordenadas en pantalla

                for (pixelYBloque = 0; pixelYBloque <= tamMino; pixelYBloque ++) // Recorre filasTablero de píxeles dentro del bloque
                {
                    for (pixelXBloque = 0; pixelXBloque <= tamMino; pixelXBloque ++) // Recorre columnasTablero de píxeles dentro del bloque
                    {
                        colorFinal = colorBase; // Pone el pixel en el color de la base
                        
                        if (pixelYBloque <= 1 || pixelXBloque <= 1) // Verifica la posición del pixel para ver si está en la esquina superior izquierda
                        {
                            colorFinal = colorBrillo [colorBase]; // Pone brillo en la esquina superior izquierda (borde de 2 píxeles)
                        }
                        else 
                        {
                            if (pixelYBloque >= tamMino - 2 || pixelXBloque >= tamMino - 2) // Verifica la posición del pixel para ver si está en la esquina inferior derecha
                            {
                                colorFinal = colorSombra [colorBase]; // Pone sombra en esquina inferior derecha (borde de 2 píxeles)
                            }
                        }

                        gbt_dibujar_pixel(posXPantalla + pixelXBloque, posYPantalla + pixelYBloque, colorFinal); // Dibuja el pixel en cuestión en la posición correspondiente
                    }
                }
            }
        }
    }
}

int COLISION (int filaNueva, int columnaNueva, int forma [4][4])
{
    int filaPieza, columnaPieza, fTablero, cTablero;
    for (filaPieza = 0; filaPieza < 4; filaPieza ++) // Recorre matriz de la pieza por filasTablero
    {
        for (columnaPieza = 0; columnaPieza < 4; columnaPieza ++) // Recorre matriz de la pieza por columnasTablero
        {
            if (forma [filaPieza][columnaPieza] == 1) // Verifica la matriz de la pieza para ver si tiene o no ocupado (para poder dibujarla)
            {
                fTablero = filaNueva + filaPieza; // Pone la pieza en el tablero
                cTablero = columnaNueva + columnaPieza; // Pone la pieza en el tablero

                // Permitir que la pieza esté por encima del tablero (fTablero < 0), pero no por debajo ni a los costados
                if (fTablero >= filasTablero || cTablero < 0 || cTablero >= columnasTablero) 
                {
                    return 1; // Colisión con bordes
                }
                
                if (fTablero >= 0 && tablero [fTablero][cTablero] != 0) // Evalua colisión con otros bloques
                {
                    return 1;
                }
            }
        }
    }
    return 0; // No hay colisión
}

void FIJARPIEZA ()
{
    int filaPieza, columnaPieza, fTablero, cTablero;
    for (filaPieza = 0; filaPieza < 4; filaPieza ++) // Recorre matriz de la pieza por filasTablero
    {
        for (columnaPieza = 0; columnaPieza < 4; columnaPieza ++) // Recorre matriz de la pieza por columnasTablero
        {
            if (actual.forma [filaPieza][columnaPieza] == 1) // Verifica si en dicha posición de la matriz de la pieza hay o no un mino
            {
                fTablero = actual.fila + filaPieza;
                cTablero = actual.columna + columnaPieza;
                if (fTablero >= 0 && fTablero < filasTablero && cTablero >= 0 && cTablero < columnasTablero)
                {
                    tablero [fTablero][cTablero] = actual.color; // Si hay un mino y está dentro del tablero, lo fija
                }
            }
        }
    }
}

void LIMPIARLINEAS ()
{
    int fila, columna, filaAux, llena;
    int lineas_en_esta_ronda = 0;

    for (fila = filasTablero - 1; fila >= 0; fila --) // Recorre las filasTablero del tablero, desde abajo hasta arriba
    {
        llena = 1;

        for (columna = 0; columna < columnasTablero; columna ++) // Recorre las columnasTablero del tablero
        {
            if (tablero[fila][columna] == 0) // Evalúa si está ocupado el espacio en el tablero. Si encuentra un vacío, la fila no está llena
            {
                llena = 0;
            }
        }

        if (llena == 1) // Evalúa si llena quedó en 1. Si quedó en 1, significa que toda la fila está llena
        {
            lineas_en_esta_ronda ++;
            for (filaAux = fila; filaAux > 0; filaAux --) // Recorre desde la fila llena hacia arriba
            {
                for (columna = 0; columna < columnasTablero; columna ++) // Recorre las columnasTablero
                {
                    tablero[filaAux][columna] = tablero[filaAux - 1][columna]; // Copia el contenido de la fila de arriba en la fila actual
                }
            }

            for (columna = 0; columna < columnasTablero; columna ++)
            {
                tablero[0][columna] = 0; // Borra la fila superior
            }

            fila ++; // Vuelve a evaluar la misma fila, porque bajó una nueva
        }
    }

    if (lineas_en_esta_ronda > 0)
    {
        int multiplicador = (nivel / 2) + 1;
        if (multiplicador > 5)
        {
            multiplicador = 5;
        }

        int puntos_base = 0;
        if (lineas_en_esta_ronda == 1) puntos_base = 100;
        else if (lineas_en_esta_ronda == 2) puntos_base = 400;
        else if (lineas_en_esta_ronda == 3) puntos_base = 900;
        else if (lineas_en_esta_ronda >= 4) puntos_base = 2000;

        int bonus_velocidad = (int)((1.0 - duracion_caida) * 500);
        if (bonus_velocidad < 0) bonus_velocidad = 0;

        puntaje += (puntos_base * multiplicador) + (bonus_velocidad * lineas_en_esta_ronda);
        lineas_totales += lineas_en_esta_ronda;
        nivel = (lineas_totales / 10) + 1;
    }
}

// Sistema de Rotación Simplificado (Alternativa a SRS)
// Intenta rotar en la posición original. Si falla, intenta desplazamientos simples
// a los lados o hacia arriba (para evitar quedarse trabado contra paredes o el suelo).
void APLICAR_ROTACION (int sentido) // 1 horario, -1 antihorario
{
    int i, j, test;
    int nueva_rotacion = (actual.rotacion + sentido + 4) % 4;
    int temporal [4][4];
    int size = (actual.tipo == 0) ? 4 : (actual.tipo == 3 ? 2 : 3);
    
    // Si es pieza O (tipo 3), no rota físicamente
    if (actual.tipo == 3) {
        actual.rotacion = nueva_rotacion;
        return;
    }

    // 1. Generar la matriz rotada en 'temporal'
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (sentido == 1) // Horario
                temporal[j][size - 1 - i] = actual.forma[i][j];
            else // Antihorario
                temporal[size - 1 - j][i] = actual.forma[i][j];
        }
    }
    // Limpiar el resto de la matriz temporal 4x4
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i >= size || j >= size) temporal[i][j] = 0;
        }
    }

    // 2. Definir una lista de intentos de desplazamiento (Wall Kicks simples)
    // {X, Y} -> Desplazamiento relativo
    int intentos[5][2] = {
        { 0,  0}, // Intento 1: Posición original
        {-1,  0}, // Intento 2: Un espacio a la izquierda
        { 1,  0}, // Intento 3: Un espacio a la derecha
        { 0, -1}, // Intento 4: Un espacio hacia arriba
        {-2,  0}  // Intento 5: Dos espacios a la izquierda (útil para la pieza I)
    };

    
    for (test = 0; test < 5; test++) {
        int dx = intentos[test][0];
        int dy = intentos[test][1];

        if (COLISION(actual.fila + dy, actual.columna + dx, temporal) == 0) {
            actual.fila += dy;
            actual.columna += dx;
            actual.rotacion = nueva_rotacion;
            COPIARPIEZA(actual.forma, temporal);
            return;
        }
    }
}

void ROTARHORARIO ()
{
    APLICAR_ROTACION(1);
}

void ROTARANTIHORARIO ()
{
    APLICAR_ROTACION(-1);
}

void DIBUJARFONDO ()
{
   int x, y; // Horizontal, vertical
   int color; // Variable color

    for (y = 0; y < altoVentana; y ++) // Recorre verticalmente la ventana
    {
        for (x = 0; x < anchoVentana; x ++) // Recorre horizontalmente la ventana
        {
            color = 1; // Define el color del fondo como azul oscuro

            if ((x % 16 == 0) || (y % 16 == 0)) // Genera la cuadrícula en color gris
            {
                color = 8;
            }
            gbt_dibujar_pixel(x, y, color); // Dibuja
        }
    } 
}

void DIBUJARMARCO ()
{
    int x, y; // Horizontal, vertical
    int x0, y0; // Esquina superior izquierda del marco
    int ancho, alto; // Ancho y alto del marco
    x0 = offsetHorizontal - 4; // Agranda el marco horizontalmente hacia afuera del tablero
    y0 = offsetVertical - 4; // Agranda el marco verticalmente hacia afuera del tablero
    ancho = columnasTablero * tamMino + 8; // Calcula el ancho del marco en pixeles de acuerdo a la cantidad de columnasTablero del tablero. Agrega 8 por el borde
    alto  = filasTablero * tamMino + 8; // Calcula el alto del marco en pixeles de acuerdo a la cantidad de columnasTablero del tablero. Agrega 8 por el borde
    int color;
    for (y = y0; y < y0 + alto; y ++) // Recorre horizontalmente el marco
    {
        for (x = x0; x < x0 + ancho; x ++) // Recorre verticalmente el marco
        {
            color = 0; // Fondo gris

            if (y <= y0 + 1 || x <= x0 + 1) // Evalúa si el pixel en cuestión está arriba o a la izquierda para ponerle brillo al marco
                {
                    color = 7; // Brillo
                }

            if (y >= y0 + alto - 2 || x >= x0 + ancho - 2) // Evalúa si el pixel en cuestión está abajo o a la derecha para ponerle sombra al marco
                {
                    color = 8; // Sombra
                }

            gbt_dibujar_pixel(x, y, color); // Dibuja el pixel
        }
    }
}

void DIBUJARGRILLA ()
{
    int fila, columna;
    int x, y;

    for (fila = 0; fila <= filasTablero; fila ++) // Líneas horizontales
    {
        y = offsetVertical + fila * tamMino;

        for (x = offsetHorizontal; x <= offsetHorizontal + columnasTablero * tamMino; x ++)
        {
            gbt_dibujar_pixel(x, y, 8); // gris oscuro
        }
    }

    for (columna = 0; columna <= columnasTablero; columna ++) // Líneas verticales
    {
        x = offsetHorizontal + columna * tamMino;

        for (y = offsetVertical; y <= offsetVertical + filasTablero * tamMino; y ++)
        {
            gbt_dibujar_pixel(x, y, 8); // gris oscuro
        }
    }
}

void DIBUJARCARACTER (int posXPantalla, int posYPantalla, int caracter, int anchoCaracter, int color) // PONER COMENTARIOS
{
    int filaCaracter, columnaCaracter;
    for (filaCaracter = 0; filaCaracter < altoCaracter; filaCaracter ++)
    {
        for (columnaCaracter = 0; columnaCaracter < anchoCaracter; columnaCaracter ++)
        {
            if (anchoCaracter == anchoCaracter8)
            {
                if (fuente8x8 [caracter][filaCaracter][columnaCaracter] == 1)
                {
                    gbt_dibujar_pixel (posXPantalla + columnaCaracter, posYPantalla + filaCaracter, color);
                }
            }
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

void DIBUJARTEXTO (int posXPantalla, int posYPantalla, char *texto, int anchoCaracter) // PONER COMENTARIOS
{
    int i = 0, caracter;
    while (texto [i] != '\0')
    {
        if (texto [i] >= 'A' && texto [i] <= 'Z')
        {
            caracter = texto [i] - 'A';
        }
        else if (texto [i] >= '0' && texto [i] <= '9')
        {
            caracter = 26 + (texto [i] - '0');
        }
        else if (texto [i] == ' ')
        {
            caracter = 36;
        }
        DIBUJARCARACTER (posXPantalla + i * anchoCaracter, posYPantalla, caracter, anchoCaracter, 7);
        i++ ;
    }
}

void DIBUJARPUNTAJE () // PONER COMENTARIOS
{
    char textoPuntaje [20];
    sprintf (textoPuntaje, "SCORE %d", puntaje);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "LEVEL %d", nivel);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 20, textoPuntaje, anchoCaracter8);

    sprintf (textoPuntaje, "LINES %d", lineas_totales);
    DIBUJARTEXTO (offsetHorizontal + columnasTablero * tamMino + tamMino, offsetVertical + 40, textoPuntaje, anchoCaracter8);

}

void DIBUJARTITULO () // PONER COMENTARIOS
{
    char titulo [] = "TETRIS";
    int colores [] = {12, 14, 10, 11, 13, 9};
    int i, caracter;
    for (i = 0; i < 6; i ++) // 6 = cantidad letras TETRIS
    {
        if (titulo [i] >= 'A' && titulo [i] <= 'Z')
        {
            caracter = titulo [i] - 'A';
        }
        DIBUJARCARACTER (100 + i * anchoCaracter16, 50, caracter, anchoCaracter16, colores [i]);
    } 
}

void DIBUJARINICIO(char *nombre) // PONER COMENTARIOS
{
    int i = 0;
    int terminado = 0;
    eGBT_Tecla tecla;

    int colores[6] = {12, 9, 10, 14, 13, 11}; // colores para TETRIS
    char titulo[] = "TETRIS";

    nombre[0] = '\0';

    while (!terminado)
    {
        gbt_procesar_entrada();
        tecla = gbt_obtener_tecla_presionada();

        if (tecla != GBTK_DESCONOCIDA)
        {
            if (tecla == GBTK_ENTER)
            {
                terminado = 1;
            }
            else if (tecla == GBTK_RETROCESO)
            {
                if (i > 0)
                {
                    i--;
                    nombre[i] = '\0';
                }
            }
            else if (tecla >= 'a' && tecla <= 'z')
            {
                if (i < 21 - 1)
                {
                    nombre[i] = tecla - 32; // mayúscula
                    i++;
                    nombre[i] = '\0';
                }
            }
            else if (tecla >= '0' && tecla <= '9')
            {
                if (i < 21 - 1)
                {
                    nombre[i] = tecla;
                    i++;
                    nombre[i] = '\0';
                }
            }
            else if (tecla == GBTK_ESPACIO)
            {
                if (i < 21 - 1)
                {
                    nombre[i] = ' ';
                    i++;
                    nombre[i] = '\0';
                }
            }
        }

        // --- DIBUJADO ---
        gbt_borrar_backbuffer(0);
        DIBUJARFONDO();

        // 🔥 TETRIS (8x16 con colores)
        int xInicio = 100;
        int yInicio = 40;

        for (int j = 0; j < 6; j++)
        {
            int caracter = titulo[j] - 'A';

            DIBUJARCARACTER(
                xInicio + j * anchoCaracter16,
                yInicio,
                caracter,
                anchoCaracter16,
                colores[j]
            );
        }

        // Texto
        DIBUJARTEXTO(80, 120, "INGRESE SU NOMBRE:", anchoCaracter8);

        // Nombre
        DIBUJARTEXTO(80, 140, nombre, anchoCaracter8);

        // Cursor
        if (i < 21 - 1)
        {
            DIBUJARCARACTER(
                80 + i * anchoCaracter8,
                140,
                36, // índice del carácter VACÍO o usá uno tipo "_"
                anchoCaracter8,
                7
            );
        }
        gbt_volcar_backbuffer();
        gbt_esperar(16);
    }
}
