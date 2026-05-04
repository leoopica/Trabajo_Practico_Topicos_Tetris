/**
 * @file gbt_logger.h
 * @author Maranzana, Rodrigo Ezequiel
 *
 * @brief Gestión de registro de eventos y errores.
 *
 * Provee acceso al buffer de mensajes de la biblioteca.
 *
 * @version 1.0
 * @date 2026-03-15
 */

#ifndef GBT_LOGGER_H_INCLUDED
#define GBT_LOGGER_H_INCLUDED

/**
 * @brief Obtiene el contenido del buffer de mensajes.
 *
 * Retorna un puntero al string que contiene el mensaje del
 * último registro de log generado por las funciones de la biblioteca.
 *
 * @return const char* Puntero al string del log.
 */
const char* gbt_obtener_log(void);

#endif // GBT_LOGGER_H_INCLUDED
