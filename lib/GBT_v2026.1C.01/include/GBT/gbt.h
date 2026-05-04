/**
 * @file gbt.h
 * @author Maranzana, Rodrigo Ezequiel
 *
 * @brief Header principal de la biblioteca GBT.
 * * Este archivo centraliza el acceso a todos los
 * modulos de la biblioteca.
 *
 * @version 1.0
 * @date 2026-03-03
 */

#ifndef GBT_H_INCLUDED
#define GBT_H_INCLUDED

#include "gbt_entrada.h"
#include "gbt_graficos.h"
#include "gbt_logger.h"
#include "gbt_temporizador.h"
#include "gbt_vector.h"

/**
 * @brief Inicializa los subsistemas de la biblioteca.
 * * Esta función debe llamarse al inicio del programa, antes de
 * cualquier otra funcion de la biblioteca.
 *
 * * @return int32_t 0 si la inicialización fue exitosa, o un valor negativo si
 * ocurrió un error durante la misma.
 */
int32_t gbt_iniciar(void);

/**
 * @brief Cierra los subsistemas de la biblioteca.
 *
 * @note Esta función debe llamarse al finalizar la ejecución del programa.
 */
void gbt_cerrar(void);

/**
 * @brief Garantiza un intervalo de tiempo constante entre sus llamadas.
 *
 * Suspende la ejecucion del programa si el tiempo transcurrido entre
 * su llamado anterior y el actual es menor al intervalo especificado.
 *
 * @note Debe ser usada en el ciclo principal del programa con el fin de asegurar
 * que el tiempo de cada ciclo sea predecible.
 *
 * @param ms Tiempo objetivo en milisegundos.
 */
void gbt_esperar(uint32_t ms);

#endif // GBT_H_INCLUDED
