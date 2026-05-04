/**
 * @file gbt_entrada.h
 * @author Maranzana, Rodrigo Ezequiel
 *
 * @brief Gestión de entrada por teclado y estados de teclas.
 *
 * Este módulo permite procesar eventos de teclado y consultar el estado
 * de las teclas bajo tres modalidades:
 * -Sostenida
 * -Presionada (flanco ascendente)
 * -Soltada (flanco de descendente)
 *
 * Utiliza una mascara de bits para diferenciar los caracteres ASCII
 * de los códigos de teclas extendidas.
 *
 * @version 1.0
 * @date 2026-03-04
 */

#ifndef GBT_ENTRADA_H_INCLUDED
#define GBT_ENTRADA_H_INCLUDED

#include <stdint.h>
#define GBT_CODIGO_A_TECLA(x) ((int32_t)(x | (1 << 30)))

/**
 * @brief Enumeración de códigos de teclas soportados.
 * * Incluye caracteres ASCII estándar y teclas extendidas.
 */
typedef enum {
    GBTK_DESCONOCIDA = 0,
    GBTK_ENTER = '\r',
    GBTK_ESCAPE = '\x1B',
    GBTK_RETROCESO = '\b',
    GBTK_TAB = '\t',
    GBTK_ESPACIO = ' ',
    GBTK_EXCLAMACION = '!',
    GBTK_COMILLAS_DOBLES = '"',
    GBTK_NUMERAL = '#',
    GBTK_PORCENTAJE = '%',
    GBTK_PESOS = '$',
    GBTK_AMPERSAND = '&',
    GBTK_COMILLA = '\'',
    GBTK_PARENTESIS_IZQ = '(',
    GBTK_PARENTESIS_DER = ')',
    GBTK_ASTERISCO = '*',
    GBTK_MAS = '+',
    GBTK_COMA = ',',
    GBTK_MENOS = '-',
    GBTK_PUNTO = '.',
    GBTK_BARRA = '/',
    GBTK_0 = '0',
    GBTK_1 = '1',
    GBTK_2 = '2',
    GBTK_3 = '3',
    GBTK_4 = '4',
    GBTK_5 = '5',
    GBTK_6 = '6',
    GBTK_7 = '7',
    GBTK_8 = '8',
    GBTK_9 = '9',
    GBTK_DOS_PUNTOS = ':',
    GBTK_PUNTO_COMA = ';',
    GBTK_MENOR = '<',
    GBTK_IGUAL = '=',
    GBTK_MAYOR = '>',
    GBTK_INTERROGACION = '?',
    GBTK_ARROBA = '@',
    GBTK_CORCHETE_IZQ = '[',
    GBTK_BARRA_INVERTIDA = '\\',
    GBTK_CORCHETE_DER = ']',
    GBTK_ACENTO_CIRCUNFLEJO = '^',
    GBTK_GUION_BAJO = '_',
    GBTK_ACENTO_INVERTIDO = '`',
    GBTK_a = 'a',
    GBTK_b = 'b',
    GBTK_c = 'c',
    GBTK_d = 'd',
    GBTK_e = 'e',
    GBTK_f = 'f',
    GBTK_g = 'g',
    GBTK_h = 'h',
    GBTK_i = 'i',
    GBTK_j = 'j',
    GBTK_k = 'k',
    GBTK_l = 'l',
    GBTK_m = 'm',
    GBTK_n = 'n',
    GBTK_o = 'o',
    GBTK_p = 'p',
    GBTK_q = 'q',
    GBTK_r = 'r',
    GBTK_s = 's',
    GBTK_t = 't',
    GBTK_u = 'u',
    GBTK_v = 'v',
    GBTK_w = 'w',
    GBTK_x = 'x',
    GBTK_y = 'y',
    GBTK_z = 'z',

    // Teclas especiales
    GBTK_ARRIBA = GBT_CODIGO_A_TECLA(82),
    GBTK_ABAJO = GBT_CODIGO_A_TECLA(81),
    GBTK_IZQUIERDA = GBT_CODIGO_A_TECLA(80),
    GBTK_DERECHA = GBT_CODIGO_A_TECLA(79),
} eGBT_Tecla;

/**
 * @brief Actualiza el estado de la entrada por teclado.
 * * Procesa la cola de eventos del sistema actualizando el estado de las teclas.
 *
 * @note Debe llamarse al inicio del ciclo del programa con el fin
 * de obtener el estado actual de las teclas.
 */
void gbt_procesar_entrada(void);

/**
 * @brief Indica si una tecla permanece pulsada entre dos llamados
 * a la función 'gbt_procesar_entrada'.
 *
 * @param tecla Código de la tecla a consultar.
 * @return uint8_t '1' si la tecla esta sostenida, '0' si no lo está.
 */
uint8_t gbt_tecla_sostenida(eGBT_Tecla tecla);

/**
 * @brief Indica si una tecla ha sido presionada luego del llamado a
 * la función 'gbt_procesar_entrada'.
 *
 * @param tecla Código de la tecla a consultar.
 * @return uint8_t 1 si la tecla cambio de sin pulsar a pulsada, 0 en caso contrario.
 */
uint8_t gbt_tecla_presionada(eGBT_Tecla tecla);

/**
 * @brief Indica si una tecla ha sido soltada luego del llamado a
 * la función 'gbt_procesar_entrada'.
 *
 * @param tecla Código de la tecla a consultar.
 * @return uint8_t 1 si la tecla cambio de pulsada a sin pulsar, 0 en caso contrario.
 */
uint8_t gbt_tecla_soltada(eGBT_Tecla tecla);

/**
 * @brief Indica el codigo de la última tecla presionada.
 * * Realiza un escaneo en los buffers de teclas y devuelve el código de la
 * tecla que ha experimentado un flanco ascendente.
 *
 * @return eGBT_Tecla Código de la tecla detectada.
 */
eGBT_Tecla gbt_obtener_tecla_presionada(void);

#endif // GBT_ENTRADA_H_INCLUDED
