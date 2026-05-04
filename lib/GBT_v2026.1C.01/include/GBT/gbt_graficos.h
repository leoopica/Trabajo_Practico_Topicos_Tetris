/**
 * @file gbt_graficos.h
 * @author Maranzana, Rodrigo Ezequiel
 *
 * @brief Subsistema de video y renderizado.
 *
 * Este módulo gestiona la creación de la ventana, su backbuffer,
 * dibujado de píxeles en el mismo, uso de paletas de colores indexadas,
 * y algortimos de dithering para el postprocesado.
 *
 * @version 1.0
 * @date 2026-03-04
 */

#ifndef GBT_GRAFICOS_H_INCLUDED
#define GBT_GRAFICOS_H_INCLUDED
#include <stdint.h>

/**
 * @brief Cantidad máxima de colores indexados que puede tener la paleta.
 */
#define PALETA_MAX_COLORES 256

/**
 * @brief Indice de color reservado para el color transparente.
 * * Siempre será el último indice de la paleta de colores.
 */
extern uint8_t GBT_INDICE_TRANSPARENTE;

typedef struct {
   uint8_t r;
   uint8_t g;
   uint8_t b;
}tGBT_ColorRGB;

/**
 * @enum eGBT_PaletaFormato
 * @brief Formatos de color válidos para los colores de la paleta.
 */
typedef enum {
    GBT_FORMATO_666, /**< Formato VGA, 6 bits por canal, valores 0-63. */
    GBT_FORMATO_888, /**< Formato RGB, 8 bits por canal, valores 0-255. */
}eGBT_PaletaFormato;

/**
 * @brief Crea la ventana y el contexto de renderizado.
 *
 * @param titulo String para el título de la ventana.
 * @param ancho Resolución horizontal lógica.
 * @param alto Resolución vertical lógica.
 * @param escala Factor de escala para el tamaño de la ventana.
 * @return int8_t 0 en caso de éxito, valor negativo en caso de falla.
 */
int8_t gbt_crear_ventana(const char *titulo, uint16_t ancho, uint16_t alto, uint8_t escala);

/**
 * @brief Libera la memoria utilizada por la ventana y la cierra.
 */
void gbt_destruir_ventana(void);

/**
 * @brief Vuelca los píxeles del backbuffer en la ventana.
 *
 * Realiza la copia de los píxeles del backbuffer en la ventana.
 */
void gbt_volcar_backbuffer(void);

/**
 * @brief Limpia el backbuffer.
 *
 * @param colorIndice Índice de la paleta de colores con el cuál llenar el backbuffer.
 */
void gbt_borrar_backbuffer(uint8_t colorIndice);

/**
 * @brief Escribe un índice de color de la palete en la
 * coordenada indicada.
 *
 * @param x Coordenada horizontal.
 * @param y Coordenada vertical.
 * @param colorIndice Índice de la paleta de colores.
 */
void gbt_dibujar_pixel(uint16_t x, uint16_t y, uint8_t colorIndice);

/**
 * @brief Obtiene el índice de color de un píxel del
 * backbuffer en la coordenada indicada.
 *
 * @param x Coordenada horizontal.
 * @param y Coordenada vertical.
 * @return uint8_t Indice del color.
 */
uint8_t gbt_obtener_color_pixel(uint16_t x, uint16_t y);

/**
 * @brief Obtiene el color RGB asociado a un índice de la paleta actual.
 *
 * @param colorIndice Índice a consultar.
 * @return tGBT_ColorRGB Estructura con los componentes del color.
 */
tGBT_ColorRGB gbt_obtener_color_paleta(uint8_t colorIndice);

/**
 * @brief Aplica una nueva paleta de colores.
 *
 * @param paleta Puntero al array de colores.
 * @param cantidadColores Número de colores de la nueva paleta (máximo 256 colores).
 * @param formato Indica si los colores de entrada son de 6 u 8 bits por canal.
 * @return int8_t 0 en éxito, valor negativo en caso de falla.
 *
 * @note Si se pasa 'NULL' en el parametro 'paleta' se aplicara la paleta VGA.
 */
int8_t gbt_aplicar_paleta(tGBT_ColorRGB *paleta, uint16_t cantidadColores, eGBT_PaletaFormato formato);

#endif // GBT_GRAFICOS_H_INCLUDED
