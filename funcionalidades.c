#include "funcionalidades.h"
#include "configuracion.h"
#include <stdlib.h>

#define duracion_animacion 4 // Duración de cada paso de la animación de borrado

// Tablero implementado como array de punteros a filas
int columnasTablero = 10; // Ancho del tablero
int *filas_tablero [FILAS_TOTALES]; // Punteros que apuntan a las filas del tablero, para reordenarlas sin copiar
int celdas_tablero [FILAS_TOTALES][MAX_COLUMNAS]; // Datos del tablero completo
int **tablero = NULL; // Puntero al array de punteros. Accede a las celdas

static int tablero_inicializado = 0;

// Variables globales del juego
int puntaje = 0;
int nivel = 1;
int lineas_totales = 0;
int piezas_caidas = 0;
double duracion_caida = 1.0;
int velocidad = 1;
eEstadoJuego estado_juego = ESTADO_RUNNING;
char nombreJugador [21];
int filas_a_borrar[MAX_FILAS_BORRAR];
int cant_filas_borrar;
int animacion_borrado_activa = 0;
int animacion_frame;
int piezas_en_uso = 7;  // Modo clásico = 7, DX = 11
int cheat_activo = 0;
double cheat_tiempo_restante = 0.0;
double cheat_cooldown_restante = 0.0;
sPieza actual;
sPieza proxima;

int bolsa [MAX_PIEZAS]; // Bolsa de piezas
int bolsaIndice = MAX_PIEZAS; // Arranca en MAX_PIEZAS para forzar LLENARBOLSA() en el primer uso

// Inicializa el tablero poniendo todo en 0
static void INIT_TABLERO(void)
{
    int f, c;

    for (f = 0; f < FILAS_TOTALES; f ++)
    {
        filas_tablero [f] = celdas_tablero [f];
        for (c = 0; c < columnasTablero; c ++)
        {
            celdas_tablero[f][c] = 0;
        }
    }
    tablero = filas_tablero;
}

// Llena la bolsa con índices 0 / piezas_en_uso-1 y los desordena (Fisher-Yates shuffle)
void LLENARBOLSA ()
{
    int i, j, temp;
    for (i = 0; i < piezas_en_uso; i ++)
    {
        bolsa [i] = i;
    }
    for (i = piezas_en_uso - 1; i > 0; i --)
    {
        j = rand() % (i + 1);
        temp = bolsa [i];
        bolsa [i] = bolsa [j];
        bolsa [j] = temp;
    }
    bolsaIndice = 0;
}

// Retorna el siguiente tipo de pieza de la bolsa
int OBTENERPIEZABOLSA ()
{
    if (bolsaIndice >= piezas_en_uso)
    {
        LLENARBOLSA ();
    }
    return bolsa [bolsaIndice++];
}

// Inicializa una pieza con el tipo dado. Copia su forma de sprites [], la centra arriba del tablero
void INICIALIZARPIEZA (sPieza *p, int tipo)
{
    int colorPiezas [MAX_PIEZAS] = // Define los colores de las piezas
    {
        11 /*I*/,
        9 /*J*/,
        6 /*L*/,
        14 /*O*/,
        10 /*S*/,
        13 /*T*/,
        12 /*Z*/,
        11 /*x*/,
        11 /*c*/,
        11 /*p*/,
        11 /***/
    };
    p->tipo = tipo;
    p->rotacion = 0;
    COPIARPIEZA (p->forma, piezas [tipo]);
    p->fila = 0;
    p->columna = columnasTablero / 2 - 2; // Centra la pieza (aprox.) en el ancho actual
    p->color = colorPiezas [tipo];
}

// Reinicia todas las variables del juego a su estado inicial
void REINICIARJUEGO ()
{
    INIT_TABLERO ();
    tablero_inicializado = 1;
    puntaje = 0;
    nivel = 1;
    lineas_totales = 0;
    piezas_caidas = 0;
    duracion_caida = 1.0;
    velocidad = 1;
    cheat_activo = 0;
    cheat_tiempo_restante = 0.0;
    cheat_cooldown_restante = 0.0;
    estado_juego = ESTADO_RUNNING;
    bolsaIndice = 7; // Fuerza llenado de bolsa en la primera llamada a OBTENERPIEZABOLSA
    INICIALIZARPIEZA (&actual, OBTENERPIEZABOLSA());
    INICIALIZARPIEZA (&proxima, OBTENERPIEZABOLSA());
}

// Avanza a la siguiente pieza: la próxima pasa a ser la actual y se genera una nueva "próxima"
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
    if (piezas_caidas > 1 && (piezas_caidas - 1) % 10 == 0) // Cada 10 piezas caídas, acelera la caída un 3% (duracion_caida /= 1.03).
    {
        duracion_caida /= 1.03;
        velocidad ++;
    }

    if (COLISION(actual.fila, actual.columna, actual.forma)) // Si la nueva pieza colisiona al aparecer, es Game Over
    {
        estado_juego = ESTADO_GAMEOVER;
    }
}

// Copia la matriz 4x4 de una pieza de origen a destino
void COPIARPIEZA (int destino [4][4], int origen [4][4])
{
    int f, c;
    for (f = 0; f < 4; f ++)
    {
        for (c = 0; c < 4; c ++)
        {
            destino [f][c] = origen [f][c];
        }
    }
}

// Evalúa si la pieza en filaNueva, columnaNueva con la forma dada colisiona. 1 si colisiona, 0 si no.
int COLISION (int filaNueva, int columnaNueva, int forma [4][4])
{
    int filaPieza, columnaPieza, fTablero, cTablero;
    for (filaPieza = 0; filaPieza < 4; filaPieza ++)
    {
        for (columnaPieza = 0; columnaPieza < 4; columnaPieza ++)
        {
            if (forma [filaPieza][columnaPieza] == 1) // Verifica que haya un mino en la pieza en cuestión en dicha posición
            {
                fTablero = filaNueva + filaPieza;
                cTablero = columnaNueva + columnaPieza;

                if (fTablero >= FILAS_TOTALES) // Evita que la pieza vaya más abajo de la última fila
                {
                    return 1;
                }

                if (config_actual.modo_juego == MODO_DX) // Tablero circular para modo DX
                {
                    if (cTablero < 0)
                    {
                        cTablero += columnasTablero;
                    }
                    if (cTablero >= columnasTablero)
                    {
                        cTablero -= columnasTablero;
                    }
                }
                else // Colisiona con las paredes en modo Clásico
                {
                    if (cTablero < 0 || cTablero >= columnasTablero)
                    {
                        return 1;
                    }
                }

                if (fTablero >= 0 && tablero [fTablero][cTablero] != 0) // Colisiona si la posición en cuestión está ocupada
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Fija la pieza actual en el tablero
void FIJARPIEZA ()
{
    int filaPieza, columnaPieza, fTablero, cTablero;
    for (filaPieza = 0; filaPieza < 4; filaPieza ++)
    {
        for (columnaPieza = 0; columnaPieza < 4; columnaPieza ++)
        {
            if (actual.forma [filaPieza][columnaPieza] == 1)
            {
                fTablero = actual.fila + filaPieza;
                cTablero = actual.columna + columnaPieza;

                if (config_actual.modo_juego == MODO_DX)
                {
                    if (cTablero < 0) cTablero += columnasTablero;
                    if (cTablero >= columnasTablero) cTablero -= columnasTablero;
                }

                if (fTablero >= 0 && fTablero < FILAS_TOTALES && cTablero >= 0 && cTablero < columnasTablero)
                {
                    tablero [fTablero][cTablero] = actual.color;
                }
            }
        }
    }
    actual.fila = -1;
    actual.columna = columnasTablero / 2 - 2;
}

// Busca filas completas y las prepara para animación de borrado
void LIMPIARLINEAS ()
{
    int fila, columna, llena, lineas_en_esta_ronda, multiplicador, puntos_base, bonus_velocidad;

    cant_filas_borrar = 0;
    lineas_en_esta_ronda = 0;

    for (fila = FILAS_TOTALES - 1; fila >= 0; fila--) // Recorre tablero de abajo hacia arriba
    {
        llena = 1; // Inicialmente define que la fila está llena. Si no lo está, lo cambia después
        for (columna = 0; columna < columnasTablero && llena == 1; columna++) // Recorre las columnas de la fila para verificar si está o no llena
        {
            if (tablero[fila][columna] == 0)
            {
                llena = 0;
            }
        }

        if (llena == 1)
        {
            filas_a_borrar [cant_filas_borrar] = fila; // Guarda el número de fila en el array
            cant_filas_borrar++; // Aumenta contador de filas a borrar
            lineas_en_esta_ronda++;
        }
    }

    if (cant_filas_borrar == 0)
    {
        return;
    }

    animacion_borrado_activa = 1; // Activa animación
    animacion_frame = 0;

    // Calcula puntaje: base (100/400/900/2000) × multiplicador + bonus por velocidad
    multiplicador = (nivel / 2) + 1;
    if (multiplicador > 5)
    {
        multiplicador = 5;
    }

    puntos_base = 0;
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

    bonus_velocidad = (int) ((1.0 - duracion_caida) * 500);
    if (bonus_velocidad < 0)
    {
        bonus_velocidad = 0;
    }

    puntaje += (puntos_base * multiplicador) + (bonus_velocidad * lineas_en_esta_ronda);

    lineas_totales += lineas_en_esta_ronda;

    nivel = (lineas_totales / 10) + 1;
}

// Aplica rotación horaria (sentido = 1) o antihoraria (sentido = -1) con wall kicks
void APLICAR_ROTACION (int sentido)
{
    int i, j, test, nueva_rotacion, temporal [4][4], size, intentos [5][2], dx, dy;

    nueva_rotacion = (actual.rotacion + sentido + 4) % 4;
    
    // Definición del tamaño de la matriz de la pieza que sí contiene minos
    if (actual.tipo == 0) // Pieza I
    {
        size = 4;
    }
    else if (actual.tipo == 3 || actual.tipo == 7) // Piezas O y x
    {
        size = 2;
    }
    else // Resto de piezas que no sean I, O y x
    {
        size = 3;
    }

    if (actual.tipo == 3 || actual.tipo == 7) // Piezas O y x no rotan
    {
        actual.rotacion = nueva_rotacion;
        return;
    }

    // Matriz temporal de rotación
    for (i = 0; i < size; i ++)
    {
        for (j = 0; j < size; j ++)
        {
            if (sentido == 1) // Sentido horario
            {
                temporal [j][size - 1 - i] = actual.forma [i][j];
            }
            else // Sentido antihorario
            {
                temporal [size - 1 - j][i] = actual.forma [i][j];
            }
        }
    }

    // Limpia celdas que no tienen minos
    for (i = 0; i < 4; i ++)
    {
        for (j = 0; j < 4; j ++)
        {
            if (i >= size || j >= size)
            {
            temporal [i][j] = 0;
            }
        }
    }

    // Evalúa los posibles wall kicks para desplazar la pieza en caso de que pase
    intentos [0][0] =  0; intentos [0][1] =  0; // Original
    intentos [1][0] = -1; intentos [1][1] =  0; // Izquierda
    intentos [2][0] =  1; intentos [2][1] =  0; // Derecha
    intentos [3][0] =  0; intentos [3][1] = -1; // Arriba
    intentos [4][0] = -2; intentos [4][1] =  0; // Dos a la izquierda

    for (test = 0; test < 5; test ++) // Prueba todos los wall kicks
    {
        dx = intentos [test][0]; // Horizontal
        dy = intentos [test][1]; // Vertical (negativo = hacia arriba)

        if (COLISION (actual.fila + dy, actual.columna + dx, temporal) == 0) // Si no hay colisión, rota
        {
            actual.fila += dy;
            actual.columna += dx;
            if (config_actual.modo_juego == MODO_DX) // Evalúa rotación para modo DX en los bordes del tablero, para que la pieza traspase al otro lado por ser circular
            {
                if (actual.columna < 0) actual.columna += columnasTablero;
                if (actual.columna >= columnasTablero) actual.columna -= columnasTablero;
            }
            actual.rotacion = nueva_rotacion;
            COPIARPIEZA(actual.forma, temporal);
            return;
        }
    }
}

// Anima el borrado de filas: desde el centro hacia los bordes, se limpian las celdas
void ACTUALIZAR_ANIMACION_BORRADO()
{
    int centro, i, offset, fila, izq, der;

    if (!animacion_borrado_activa)
    {
        return;
    }

    animacion_frame ++;

    centro = columnasTablero / 2; // Centro para la animación

    for (i = 0; i < cant_filas_borrar; i ++) // Recorre filas a borrar
    {
        fila = filas_a_borrar [i];

        for (offset = 0; offset <= centro; offset ++) // Izquierda y derecha de forma simétrica
        {
            izq = centro - offset;
            der = centro + offset;

            if (animacion_frame >= offset * duracion_animacion)
            {
                if (izq >= 0)
                {
                    tablero [fila][izq] = 0;
                }
                if (der < columnasTablero)
                {
                    tablero [fila][der] = 0;
                }
            }
        }
    }

    if (animacion_frame >= centro * duracion_animacion + duracion_animacion) // Desactiva la animación cuando se completa la fila
    {
        animacion_borrado_activa = 0;
        COLAPSAR_FILAS ();
        animacion_frame = 0;
    }
}

// Reordena los punteros de fila para que las filas vacías borradas queden arriba
void COLAPSAR_FILAS ()
{
    int *nuevo_orden [FILAS_TOTALES], destino, fila, i, c, f, es_borrada;

    destino = FILAS_TOTALES - 1;

    for (fila = FILAS_TOTALES - 1; fila >= 0; fila --) // Recorre de abajo hacia arriba
    {
        es_borrada = 0;
        for (i = 0; i < cant_filas_borrar && es_borrada == 0; i ++) // Verifica si la fila en cuestión debe borrarse
        {
            if (filas_a_borrar[i] == fila)
            {
                es_borrada = 1;
            }
        }

        if (!es_borrada) // Si no debe borrarse, reordena de abajo hacia arriba
        {
            nuevo_orden [destino--] = filas_tablero [fila];
        }
    }

    for (fila = FILAS_TOTALES - 1; fila >= 0; fila --)
    {
        es_borrada = 0;
        for (i = 0; i < cant_filas_borrar && es_borrada == 0; i ++)
        {
            if (filas_a_borrar [i] == fila)
            {
                es_borrada = 1;
            }
        }

        if (es_borrada) // Si debe borrarse, vacía todas las posiciones de la fila
        {
            for (c = 0; c < columnasTablero; c ++)
            {
                filas_tablero [fila][c] = 0;
            }
            nuevo_orden [destino--] = filas_tablero [fila]; // Mueve esta nueva fila vacía arriba
        }
    }

    for (f = 0; f < FILAS_TOTALES; f ++)
    {
        filas_tablero [f] = nuevo_orden [f];
    }

    tablero = filas_tablero;
    cant_filas_borrar = 0;
}

void ROTARHORARIO ()
{
    APLICAR_ROTACION (1);
}

void ROTARANTIHORARIO ()
{
    APLICAR_ROTACION (-1);
}