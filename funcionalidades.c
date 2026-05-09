#include "funcionalidades.h"

// Definición del tablero
int tablero [filasTablero][columnasTablero] = {0};

//Definición del estado inicial del juego
int puntaje = 0;
int nivel = 1;
int lineas_totales = 0;
int piezas_caidas = 0;
double duracion_caida = 1.0;
int velocidad = 1;
eEstadoJuego estado_juego = ESTADO_RUNNING;
char nombreJugador [21]; // HAY QUE CAMBIARLO, PROBABLEMENTE


/*
 *------------------------*
 *---------PIEZAS---------*
 *------------------------*
 */
sPieza actual;
sPieza proxima;

// Sistema de 7-bag
int bolsa [7];
int bolsaIndice = 7; // Forzamos llenado inicial

void LLENARBOLSA ()
{
    int i, j, temp;
    for (i = 0; i < 7; i++) bolsa[i] = i;
    for (i = 6; i > 0; i--)
    {
        j = rand() % (i + 1);
        temp = bolsa[i];
        bolsa[i] = bolsa[j];
        bolsa[j] = temp;
    }
    bolsaIndice = 0;
}

int OBTENERPIEZABOLSA ()
{
    if (bolsaIndice >= 7) LLENARBOLSA();
    return bolsa[bolsaIndice++];
}

void INICIALIZARPIEZA (sPieza *p, int tipo)
{
    int colorPiezas [cantPiezas] = {11, 9, 6, 14, 10, 13, 12};
    p->tipo = tipo;
    p->rotacion = 0;
    COPIARPIEZA (p->forma, piezas [tipo]);
    p->fila = 0;
    p->columna = columnasTablero / 2 - 2; // Revisar si este cálculo es correcto (atte mati)
    p->color = colorPiezas [tipo];
}

void REINICIARJUEGO ()
{
    int f, c;
    for (f = 0; f < filasTablero; f++) {
        for (c = 0; c < columnasTablero; c++) {
            tablero[f][c] = 0;
        }
    }
    puntaje = 0;
    nivel = 1;
    lineas_totales = 0;
    piezas_caidas = 0;
    duracion_caida = 1.0;
    velocidad = 1;
    estado_juego = ESTADO_RUNNING;
    bolsaIndice = 7; // Forzar llenado de bolsa
    INICIALIZARPIEZA(&actual, OBTENERPIEZABOLSA());
    INICIALIZARPIEZA(&proxima, OBTENERPIEZABOLSA());
}

void NUEVAPIEZA ()
{
    static int primeraVez = 1;

    if (primeraVez)
    {
        INICIALIZARPIEZA(&actual, OBTENERPIEZABOLSA());
        INICIALIZARPIEZA(&proxima, OBTENERPIEZABOLSA());
        primeraVez = 0;
    }
    else
    {
        actual = proxima;
        INICIALIZARPIEZA(&proxima, OBTENERPIEZABOLSA());
    }

    piezas_caidas ++;
    if (piezas_caidas > 1 && (piezas_caidas - 1) % 10 == 0)
    {
        duracion_caida /= 1.03;
        velocidad ++;
    }

    // Detección de Game Over
    if (COLISION(actual.fila, actual.columna, actual.forma)) {
        estado_juego = ESTADO_GAMEOVER;
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
    int fila, columna;
    int filaDestino = filasTablero - 1; // Indica en qué fila debe copiarse la próxima fila válida
    int llena; // Indica si la fila está completamente llena
    int lineas_en_esta_ronda = 0; // Cantidad de líneas eliminadas en esta jugada

    // Recorre las filas del tablero desde abajo hacia arriba
    for (fila = filasTablero - 1; fila >= 0; fila --)
    {
        llena = 1; // Asume inicialmente que la fila está llena

        // Recorre las columnas de la fila actual
        for (columna = 0; columna < columnasTablero; columna ++)
        {
            // Verifica si existe algún espacio vacío en la fila
            if (tablero [fila][columna] == 0)
            {
                llena = 0; // Si encuentra un vacío, la fila ya no está llena
            }
        }

        // Evalúa si la fila NO está llena
        if (llena == 0)
        {
            // Verifica si la fila necesita desplazarse hacia abajo
            if (filaDestino != fila)
            {
                // Copia la fila actual en la posición de destino
                for (columna = 0; columna < columnasTablero; columna ++)
                {
                    tablero [filaDestino][columna] = tablero [fila][columna];
                }
            }

            // Mueve la fila de destino una posición hacia arriba
            filaDestino --;
        }
        else
        {
            // Si la fila estaba llena, aumenta el contador de líneas eliminadas
            lineas_en_esta_ronda ++;
        }
    }

    // Limpia todas las filas restantes en la parte superior del tablero
    while (filaDestino >= 0)
    {
        // Recorre las columnas de la fila
        for (columna = 0; columna < columnasTablero; columna ++)
        {
            tablero [filaDestino][columna] = 0; // Borra el contenido de la celda
        }

        // Continúa con la fila superior
        filaDestino --;
    }

    // Verifica si se eliminó al menos una línea
    if (lineas_en_esta_ronda > 0)
    {
        int multiplicador = (nivel / 2) + 1; // Calcula multiplicador según nivel

        // Limita el multiplicador a un máximo de 5
        if (multiplicador > 5)
        {
            multiplicador = 5;
        }

        int puntos_base = 0;

        // Define el puntaje base según la cantidad de líneas eliminadas
        if (lineas_en_esta_ronda == 1)
        {
            puntos_base = 100;
        }
        else if (lineas_en_esta_ronda == 2)
        {
            puntos_base = 400;
        }
        else if (lineas_en_esta_ronda == 3)
        {
            puntos_base = 900;
        }
        else if (lineas_en_esta_ronda >= 4)
        {
            puntos_base = 2000;
        }

        // Calcula bonus adicional según la velocidad actual
        int bonus_velocidad = (int)((1.0 - duracion_caida) * 500);

        // Evita bonus negativos
        if (bonus_velocidad < 0)
        {
            bonus_velocidad = 0;
        }

        // Suma los puntos obtenidos
        puntaje += (puntos_base * multiplicador) + (bonus_velocidad * lineas_en_esta_ronda);

        // Suma la cantidad de líneas eliminadas al total
        lineas_totales += lineas_en_esta_ronda;

        // Actualiza el nivel cada 10 líneas
        nivel = (lineas_totales / 10) + 1;
    }
}

/* void LIMPIARLINEAS ()
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
} */


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