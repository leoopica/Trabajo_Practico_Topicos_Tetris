#include "funcionalidades.h"
#include "configuracion.h"
#include <stdlib.h>

// Duración de cada paso de la animación de borrado (frames)
#define duracion_animacion 4

// Tablero implementado como array de punteros a filas (requisito de promoción)
// int **tablero apunta a filas_tablero[], cuyos elementos apuntan a celdas_tablero[][].
// LIMPIARLINEAS + COLAPSAR_FILAS reordenan punteros en lugar de copiar celdas.
int columnasTablero = 10;
int *filas_tablero[FILAS_TOTALES];
int celdas_tablero[FILAS_TOTALES][MAX_COLUMNAS];
int **tablero = NULL;

// Inicializa el tablero: apunta cada filas_tablero[f] a celdas_tablero[f] y pone todo en 0
static void INIT_TABLERO(void)
{
    for (int f = 0; f < FILAS_TOTALES; f++)
    {
        filas_tablero[f] = celdas_tablero[f];
        for (int c = 0; c < columnasTablero; c++)
            celdas_tablero[f][c] = 0;
    }
    tablero = filas_tablero;
}

// Flag para asegurar que INIT_TABLERO se ejecuta antes del primer uso
static int tablero_inicializado = 0;

// ---- Variables globales del juego ----
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

// Bolsa de piezas (Fisher-Yates): baraja los tipos y los entrega en orden aleatorio
int bolsa[MAX_PIEZAS];
int bolsaIndice = MAX_PIEZAS; // Arranca en MAX_PIEZAS para forzar LLENARBOLSA() en el primer uso

// Llena la bolsa con índices 0..piezas_en_uso-1 y los desordena (Fisher-Yates shuffle)
void LLENARBOLSA ()
{
    int i, j, temp;
    for (i = 0; i < piezas_en_uso; i++) bolsa[i] = i;
    for (i = piezas_en_uso - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        temp = bolsa[i];
        bolsa[i] = bolsa[j];
        bolsa[j] = temp;
    }
    bolsaIndice = 0;
}

// Retorna el siguiente tipo de pieza de la bolsa
int OBTENERPIEZABOLSA ()
{
    if (bolsaIndice >= piezas_en_uso) LLENARBOLSA();
    return bolsa[bolsaIndice++];
}

// Inicializa una pieza con el tipo dado: copia su forma de sprites[], la centra arriba del tablero
void INICIALIZARPIEZA (sPieza *p, int tipo)
{
    int colorPiezas [MAX_PIEZAS] = {
         11 /*I*/,  9 /*J*/,  6 /*L*/, 14 /*O*/, 10 /*S*/, 13 /*T*/, 12 /*Z*/,
         11 /*x*/, 11 /*c*/, 11 /*p*/, 11 /***/
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
    INIT_TABLERO();
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
    INICIALIZARPIEZA(&actual, OBTENERPIEZABOLSA());
    INICIALIZARPIEZA(&proxima, OBTENERPIEZABOLSA());
}

// Avanza a la siguiente pieza: la próxima pasa a ser la actual y se genera una nueva "próxima"
// Cada 10 piezas caídas, acelera la caída un 3% (duracion_caida /= 1.03)
// Si la nueva pieza colisiona al aparecer, es Game Over
void NUEVAPIEZA ()
{
    static int primeraVez = 1;

    if (primeraVez)
    {
        // La primera vez inicializa ambas piezas desde la bolsa
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
        duracion_caida /= 1.03; // Acelera 3%
        velocidad ++;
    }

    // Game Over si la pieza nueva colisiona en su posición inicial
    if (COLISION(actual.fila, actual.columna, actual.forma)) {
        estado_juego = ESTADO_GAMEOVER;
    }
}

// Copia la matriz 4x4 de una pieza de origen a destino
void COPIARPIEZA (int destino [4][4], int origen [4][4])
{
    int f, c;
    for (f = 0; f < 4; f ++)
        for (c = 0; c < 4; c ++)
            destino [f][c] = origen [f][c];
}

// Evalúa si la pieza en (filaNueva, columnaNueva) con la forma dada colisiona
// Retorna 1 si colisiona, 0 si no.
// En modo DX, wrappea coordenadas horizontales (tablero circular: sale por izquierda, entra por derecha y viceversa)
int COLISION (int filaNueva, int columnaNueva, int forma [4][4])
{
    int filaPieza, columnaPieza, fTablero, cTablero;
    for (filaPieza = 0; filaPieza < 4; filaPieza ++)
    {
        for (columnaPieza = 0; columnaPieza < 4; columnaPieza ++)
        {
            if (forma [filaPieza][columnaPieza] == 1)
            {
                fTablero = filaNueva + filaPieza;
                cTablero = columnaNueva + columnaPieza;

                // Límite inferior: siempre colisión (la pieza tocó el piso)
                if (fTablero >= FILAS_TOTALES)
                    return 1;

                // En modo DX las columnas wrappean; en clásico los bordes laterales son colisión
                if (config_actual.modo_juego == MODO_DX)
                {
                    if (cTablero < 0) cTablero += columnasTablero;
                    if (cTablero >= columnasTablero) cTablero -= columnasTablero;
                }
                else
                {
                    if (cTablero < 0 || cTablero >= columnasTablero)
                        return 1;
                }

                // Colisión con otro bloque ya fijado en el tablero
                if (fTablero >= 0 && tablero [fTablero][cTablero] != 0)
                    return 1;
            }
        }
    }
    return 0; // Sin colisión
}

// Fija la pieza actual en el tablero (escribe su color en cada celda ocupada)
// En modo DX, wrappea las coordenadas horizontales
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
                    tablero [fTablero][cTablero] = actual.color; // Si hay un mino y está dentro del tablero, lo fija
                }
            }
        }
    }
    // Mueve la pieza fuera del tablero para que no se dibuje más
    actual.fila = -1;
    actual.columna = columnasTablero / 2 - 2;
}

// Busca filas completas y las prepara para animación de borrado
// Calcula puntaje: base (100/400/900/2000) × multiplicador + bonus por velocidad
void LIMPIARLINEAS ()
{
    int fila, columna;
    int llena;
    int lineas_en_esta_ronda = 0;

    cant_filas_borrar = 0;

    // Recorre de abajo hacia arriba para detectar filas completas
    // (incluye filas invisibles para detectar game over al compactar)
    for (fila = FILAS_TOTALES - 1; fila >= 0; fila--)
    {
        llena = 1;
        for (columna = 0; columna < columnasTablero; columna++)
        {
            if (tablero[fila][columna] == 0)
            {
                llena = 0;
                break;
            }
        }

        if (llena == 1)
        {
            filas_a_borrar[cant_filas_borrar] = fila;
            cant_filas_borrar++;
            lineas_en_esta_ronda++;
        }
    }

    if (cant_filas_borrar == 0) return;

    animacion_borrado_activa = 1;
    animacion_frame = 0;

    // Cálculo de puntaje
    int multiplicador = (nivel / 2) + 1;
    if (multiplicador > 5) multiplicador = 5;

    int puntos_base = 0;
    if (lineas_en_esta_ronda == 1)
        puntos_base = 100;
    else if (lineas_en_esta_ronda == 2)
        puntos_base = 400;
    else if (lineas_en_esta_ronda == 3)
        puntos_base = 900;
    else if (lineas_en_esta_ronda >= 4)
        puntos_base = 2000;

    // Bonus por velocidad: a mayor velocidad (menor duracion_caida), más puntos
    int bonus_velocidad = (int)((1.0 - duracion_caida) * 500);
    if (bonus_velocidad < 0) bonus_velocidad = 0;

    puntaje += (puntos_base * multiplicador) + (bonus_velocidad * lineas_en_esta_ronda);

    lineas_totales += lineas_en_esta_ronda;

    // Cada 10 líneas se sube un nivel
    nivel = (lineas_totales / 10) + 1;
}

// Aplica rotación horaria (sentido=1) o antihoraria (sentido=-1) con wall kicks
// Intenta 5 posiciones: original, izquierda, derecha, arriba, 2 a la izquierda
// Las piezas O (tipo 3) y x (tipo 7) no rotan visualmente
void APLICAR_ROTACION (int sentido)
{
    int i, j, test;
    int nueva_rotacion = (actual.rotacion + sentido + 4) % 4;
    int temporal [4][4];
    int size = (actual.tipo == 0) ? 4 : (actual.tipo == 3 || actual.tipo == 7) ? 2 : 3;

    // Piezas O y x: solo actualizan el contador de rotación, no cambian forma
    if (actual.tipo == 3 || actual.tipo == 7) {
        actual.rotacion = nueva_rotacion;
        return;
    }

    // Genera la matriz rotada en 'temporal'
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (sentido == 1) // Horario: fila i → columna size-1-i, columna j → fila j
                temporal[j][size - 1 - i] = actual.forma[i][j];
            else // Antihorario: fila i → columna i, columna j → fila size-1-j
                temporal[size - 1 - j][i] = actual.forma[i][j];
        }
    }
    // Limpia el resto de la matriz 4x4 (fuera del tamaño activo)
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (i >= size || j >= size) temporal[i][j] = 0;

    // Wall kicks: prueba hasta 5 desplazamientos para encontrar una posición válida
    int intentos[5][2] = {
        { 0,  0}, // Sin desplazamiento
        {-1,  0}, // Una columna a la izquierda
        { 1,  0}, // Una columna a la derecha
        { 0, -1}, // Una fila hacia arriba
        {-2,  0}  // Dos columnas a la izquierda (útil para pieza I)
    };

    for (test = 0; test < 5; test++) {
        int dx = intentos[test][0];
        int dy = intentos[test][1];

        if (COLISION(actual.fila + dy, actual.columna + dx, temporal) == 0) {
            actual.fila += dy;
            actual.columna += dx;
            // En modo DX, wrappea la columna si queda fuera del tablero
            if (config_actual.modo_juego == MODO_DX)
            {
                if (actual.columna < 0) actual.columna += columnasTablero;
                if (actual.columna >= columnasTablero) actual.columna -= columnasTablero;
            }
            actual.rotacion = nueva_rotacion;
            COPIARPIEZA(actual.forma, temporal);
            return;
        }
    }
    // Si ningún intento funcionó, la rotación no se realiza
}

/*
// Versión anterior de ACTUALIZAR_ANIMACION_BORRADO (reemplazada)
void ACTUALIZAR_ANIMACION_BORRADO()
{
    if (!animacion_borrado_activa) return;
    animacion_frame++;
    int centro = columnasTablero / 2;
    int paso = animacion_frame % duracion_animacion;
    for (int i = 0; i < cant_filas_borrar; i++)
    {
        int fila = filas_a_borrar[i];
        for (int offset = 0; offset <= centro; offset++)
        {
            int izq = centro - offset;
            int der = centro + offset;
            if (paso >= offset)
            {
                if (izq >= 0) tablero[fila][izq] = 0;
                if (der < columnasTablero) tablero[fila][der] = 0;
            }
        }
    }
    if (animacion_frame > duracion_animacion)
    {
        animacion_borrado_activa = 0;
        COLAPSAR_FILAS();
        animacion_frame = 0;
    }
}
*/

// Anima el borrado de filas: desde el centro hacia los bordes, se limpian las celdas
// Cada offset se limpia después de offset * duracion_animacion frames
// Al terminar (frame ≥ centro * duracion_animacion + duracion_animacion), colapsa filas
void ACTUALIZAR_ANIMACION_BORRADO()
{
    if (!animacion_borrado_activa) return;

    animacion_frame++;

    int centro = columnasTablero / 2;

    for (int i = 0; i < cant_filas_borrar; i++)
    {
        int fila = filas_a_borrar[i];

        for (int offset = 0; offset <= centro; offset++)
        {
            int izq = centro - offset;
            int der = centro + offset;

            // Limpia desde el centro hacia afuera: el offset 0 se limpia en frame 0,
            // offset 1 en frame >= duracion_animacion, offset 2 en frame >= 2*duracion_animacion...
            if (animacion_frame >= offset * duracion_animacion)
            {
                if (izq >= 0)
                    tablero[fila][izq] = 0;
                if (der < columnasTablero)
                    tablero[fila][der] = 0;
            }
        }
    }

    if (animacion_frame >= centro * duracion_animacion + duracion_animacion)
    {
        animacion_borrado_activa = 0;
        COLAPSAR_FILAS();
        animacion_frame = 0;
    }
}

// Reordena los punteros de fila para que las filas vacías (borradas) queden arriba
// y las filas con bloques queden abajo. No copia celdas, solo intercambia punteros.
void COLAPSAR_FILAS ()
{
    int *nuevo_orden[FILAS_TOTALES];
    int destino = FILAS_TOTALES - 1;

    // Primero: coloca filas NO borradas desde abajo
    for (int fila = FILAS_TOTALES - 1; fila >= 0; fila--)
    {
        int es_borrada = 0;
        for (int i = 0; i < cant_filas_borrar; i++)
            if (filas_a_borrar[i] == fila) { es_borrada = 1; break; }

        if (!es_borrada)
            nuevo_orden[destino--] = filas_tablero[fila];
    }

    // Segundo: coloca filas borradas (vacías) arriba
    for (int fila = FILAS_TOTALES - 1; fila >= 0; fila--)
    {
        int es_borrada = 0;
        for (int i = 0; i < cant_filas_borrar; i++)
            if (filas_a_borrar[i] == fila) { es_borrada = 1; break; }

        if (es_borrada)
        {
            for (int c = 0; c < columnasTablero; c++)
                filas_tablero[fila][c] = 0;
            nuevo_orden[destino--] = filas_tablero[fila];
        }
    }

    for (int f = 0; f < FILAS_TOTALES; f++)
        filas_tablero[f] = nuevo_orden[f];

    tablero = filas_tablero;
    cant_filas_borrar = 0;
}

void ROTARHORARIO ()
{
    APLICAR_ROTACION(1);
}

void ROTARANTIHORARIO ()
{
    APLICAR_ROTACION(-1);
}
