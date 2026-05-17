#include "funcionalidades.h"
#include "configuracion.h"
#include <stdlib.h>
#define duracion_animacion 4

// Tablero implementado como array de punteros a filas (requisito de promoción)
// En lugar de int tablero[20][10], usamos int **tablero donde cada fila es un puntero independiente.
// Esto permite limpiar líneas completas con solo intercambiar punteros (sin copiar memoria).
int columnasTablero = 10; // Ancho real del tablero (por defecto clásico)
int *filas_tablero[filasTablero]; // Array de punteros (uno por fila)
int celdas_tablero[filasTablero][MAX_COLUMNAS]; // Memoria real de las celdas
int **tablero = NULL; // Puntero al array de punteros (para acceso como tablero[f][c])

// Inicializa el tablero dinámico: apunta cada fila al bloque de celdas correspondiente
static void INIT_TABLERO(void)
{
    for (int f = 0; f < filasTablero; f++)
    {
        filas_tablero[f] = celdas_tablero[f];
        for (int c = 0; c < columnasTablero; c++)
            celdas_tablero[f][c] = 0;
    }
    tablero = filas_tablero;
}

// Inicialización automática al arrancar (tablero apuntado correctamente desde el inicio)
static int tablero_inicializado = 0;
//Definición del estado inicial del juego
int puntaje = 0;
int nivel = 1;
int lineas_totales = 0;
int piezas_caidas = 0;
double duracion_caida = 1.0;
int velocidad = 1;
eEstadoJuego estado_juego = ESTADO_RUNNING;
char nombreJugador [21]; // HAY QUE CAMBIARLO, PROBABLEMENTE
int filas_a_borrar[MAX_FILAS_BORRAR];
int cant_filas_borrar;
int animacion_borrado_activa = 0;
int animacion_frame;
int piezas_en_uso = 7; // Por defecto modo clásico (7 piezas)
int cheat_activo = 0;
double cheat_tiempo_restante = 0.0;
double cheat_cooldown_restante = 0.0;
sPieza actual;
sPieza proxima;

int bolsa[MAX_PIEZAS];
int bolsaIndice = MAX_PIEZAS; // Forzamos llenado inicial

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

int OBTENERPIEZABOLSA ()
{
    if (bolsaIndice >= piezas_en_uso) LLENARBOLSA();
    return bolsa[bolsaIndice++];
}

void INICIALIZARPIEZA (sPieza *p, int tipo)
{
    int colorPiezas [MAX_PIEZAS] = {11, 9, 6, 14, 10, 13, 12, 11, 11, 11, 11};
    p->tipo = tipo;
    p->rotacion = 0;
    COPIARPIEZA (p->forma, piezas [tipo]);
    p->fila = 0;
    p->columna = columnasTablero / 2 - 2; // Revisar si este cálculo es correcto (atte mati)
    p->color = colorPiezas [tipo];
}

void REINICIARJUEGO ()
{
    INIT_TABLERO(); // Reinicializar punteros y limpiar tablero
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

                // Límite inferior: siempre colisión
                if (fTablero >= filasTablero)
                {
                    return 1;
                }

                // Tablero circular (solo en modo DX): wrappear coordenada horizontal
                if (config_actual.modo_juego == MODO_DX)
                {
                    if (cTablero < 0) cTablero += columnasTablero;
                    if (cTablero >= columnasTablero) cTablero -= columnasTablero;
                }
                else
                {
                    // Modo clásico: rechazar bordes laterales
                    if (cTablero < 0 || cTablero >= columnasTablero)
                    {
                        return 1;
                    }
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

                // Tablero circular en modo DX: wrappear coordenada horizontal
                if (config_actual.modo_juego == MODO_DX)
                {
                    if (cTablero < 0) cTablero += columnasTablero;
                    if (cTablero >= columnasTablero) cTablero -= columnasTablero;
                }

                if (fTablero >= 0 && fTablero < filasTablero && cTablero >= 0 && cTablero < columnasTablero)
                {
                    tablero [fTablero][cTablero] = actual.color; // Si hay un mino y está dentro del tablero, lo fija
                }
            }
        }
    }
    actual.fila = -1;
    actual.columna = columnasTablero / 2 - 2;
}

void LIMPIARLINEAS ()
{
    int fila, columna;
    int llena; // Indica en qué fila debe copiarse la próxima fila válida
    int lineas_en_esta_ronda = 0; // Cantidad de líneas eliminadas en esta jugada

    // Reset de la lista de filas a borrar
    cant_filas_borrar = 0;

    // Recorre las filas del tablero desde abajo hacia arriba
    for (fila = filasTablero - 1; fila >= 0; fila--)
    {
        llena = 1; // Asume inicialmente que la fila está llena

        // Recorre las columnas de la fila actual
        for (columna = 0; columna < columnasTablero; columna++)
        {
            // Verifica si existe algún espacio vacío en la fila
            if (tablero[fila][columna] == 0)
            {
                llena = 0; // Si encuentra un vacío, la fila ya no está llena
            }
        }

        // Si la fila está completa, se guarda para animación
        if (llena == 1)
        {
            filas_a_borrar[cant_filas_borrar] = fila; // Guarda en el vector de filas a borrar la fila que hay que borrar
            cant_filas_borrar++; // Aumenta la cantidad de filas a borrar
            lineas_en_esta_ronda++; // Aumenta la cantidad de filas que se borran en esta ronda
        }
    }

    // Si no hay filas a borrar, sale de la función
    if (cant_filas_borrar == 0)
    {
        return;
    }

    animacion_borrado_activa = 1; // Activa la animación
    animacion_frame = 0;

    // Calculo de puntaje
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

// Intenta rotar en la posición original. Si falla, intenta desplazamientos simples
// a los lados o hacia arriba (para evitar quedarse trabado contra paredes o el suelo).
void APLICAR_ROTACION (int sentido) // 1 horario, -1 antihorario
{
    int i, j, test;
    int nueva_rotacion = (actual.rotacion + sentido + 4) % 4;
    int temporal [4][4];
    int size = (actual.tipo == 0) ? 4 : (actual.tipo == 3 || actual.tipo == 7) ? 2 : 3;

    // Si es pieza O (tipo 3) o x (tipo 7), no rota físicamente
    if (actual.tipo == 3 || actual.tipo == 7) {
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
}

/*
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
                if (izq >= 0)
                    tablero[fila][izq] = 0;

                if (der < columnasTablero)
                    tablero[fila][der] = 0;
            }
        }
    }

    // cuando termina la animación
    if (animacion_frame > duracion_animacion)
    {
        animacion_borrado_activa = 0;
        COLAPSAR_FILAS(); // <- reacomoda el tablero
        animacion_frame = 0;
    }
}
*/

void ACTUALIZAR_ANIMACION_BORRADO() // NUEVA
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

void COLAPSAR_FILAS ()
{
    // Implementación con intercambio de punteros (requisito de promoción):
    // En lugar de copiar la memoria de cada celda, reordenamos los punteros de fila.
    // Las filas completas (borradas) se vaciaron durante la animación (celdas en 0).
    // Las movemos al tope del tablero reordenando el array de punteros filas_tablero[].

    // Construir el nuevo orden: primero las filas NO borradas (de abajo a arriba),
    // luego las filas borradas (ya vacías) arriba.
    int *nuevo_orden[filasTablero];
    int destino = filasTablero - 1; // Colocamos filas válidas desde abajo

    for (int fila = filasTablero - 1; fila >= 0; fila--)
    {
        int es_borrada = 0;
        for (int i = 0; i < cant_filas_borrar; i++)
            if (filas_a_borrar[i] == fila) { es_borrada = 1; break; }

        if (!es_borrada)
            nuevo_orden[destino--] = filas_tablero[fila];
    }

    // Las filas borradas (ya vacías) van al tope
    for (int fila = filasTablero - 1; fila >= 0; fila--)
    {
        int es_borrada = 0;
        for (int i = 0; i < cant_filas_borrar; i++)
            if (filas_a_borrar[i] == fila) { es_borrada = 1; break; }

        if (es_borrada)
        {
            // Vaciar la fila por si acaso
            for (int c = 0; c < columnasTablero; c++)
                filas_tablero[fila][c] = 0;
            nuevo_orden[destino--] = filas_tablero[fila];
        }
    }

    // Aplicar el nuevo orden: solo intercambiamos punteros, sin copiar celdas
    for (int f = 0; f < filasTablero; f++)
        filas_tablero[f] = nuevo_orden[f];

    // Actualizar el puntero tablero (apunta siempre a filas_tablero)
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