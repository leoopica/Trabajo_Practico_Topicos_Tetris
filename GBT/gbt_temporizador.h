/**
 * @file gbt_temporizador.h
 * @author Maranzana, Rodrigo Ezequiel
 *
 * @brief Modulo para la gestión de temporizadores.
 *
 * Provee funciones para la creación y control de temporizadores.
 * Permiten la ejecucion de lógica condicionada a intervalos de tiempo
 * específicos mediante un sistema de tiempo acumulado.
 *
 * @version 1.0
 * @date 2026-03-13
 */

#ifndef GBT_TEMPORIZADOR_H_INCLUDED
#define GBT_TEMPORIZADOR_H_INCLUDED

#include <stdint.h>

/**
 * @typedef tGBT_Temporizador
 * @brief Tipo opaco del temporizador.
 */
typedef struct sGBT_Temporizador tGBT_Temporizador;

/**
 * @brief Crea e inicializa un nuevo temporizador.
 *
 * @param duracion Tiempo en segundos.
 * @return tGBT_Temporizador* Puntero al temporizador creado o NULL si falla su creación.
 */
tGBT_Temporizador* gbt_temporizador_crear(double duracion);

/**
 * @brief Libera la memoria asociada a un temporizador.
 *
 * @param t Puntero al temporizador a destruir.
 */
void gbt_temporizador_destruir(tGBT_Temporizador *t);

/**
 * @brief Actualiza el acumulador y verifica si se ha cumplido el tiempo.
 *
 * Calcula el tiempo transcurrido desde la última llamada. Si el tiempo
 * acumulado supera la duración definida retorna '1' y reinicia el temporizador.
 *
 * @param t Puntero al temporizador.
 * @return uint8_t '1' si se cumplió el tiempo, 0 en caso contrario.
 */
uint8_t gbt_temporizador_consumir(tGBT_Temporizador* t);


/**
 * @brief Pausa el temporizador.
 *
 * @param t Puntero al temporizador.
 * @note Solo surte efecto si el temporizador esta activo.
 */
void gbt_temporizador_pausar(tGBT_Temporizador* t);


/**
 * @brief Reanuda el temporizador desde donde se quedo la ultima vez.
 *
 * @param t Puntero al temporizador.
 * @note Solo surte efecto si el temporizador esta pausado.
 */
void gbt_temporizador_reanudar(tGBT_Temporizador* t);

#endif // GBT_TEMPORIZADOR_H_INCLUDED
