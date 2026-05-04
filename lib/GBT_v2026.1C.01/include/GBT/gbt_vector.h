#ifndef GBT_VECTOR_H_INCLUDED
#define GBT_VECTOR_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>

typedef enum {
    GBT_BURBUJEO,
    GBT_SELECCION,
    GBT_INSERCION,
} eGBT_Ordenamiento;

typedef enum {
    GBT_VECTOR_TODO_OK,
    GBT_VECTOR_SIN_MEM,
    GBT_VECTOR_ERR_TAM,
    GBT_VECTOR_ERR_ARCH,
    GBT_VECTOR_DUPLICADO,
} eGBT_VectorRet;

typedef struct {
    void *vec;
    int32_t ce;
    size_t cap;
    size_t tamElem;
} tGBT_Vector;

typedef struct {
    tGBT_Vector *v;
    void *act;
    void *ult;
    size_t tamElem;
} tGBT_VectorIterador;

typedef int8_t (*tGBT_Cmp)(const void *e1, const void *e2);
typedef void (*tGBT_Actualizar)(void *actualizado, const void *actualizador);
typedef void (*tGBT_Accion)(void *elem, void *extra);

uint8_t gbt_vector_crear(tGBT_Vector *v, size_t tamElem);
void gbt_vector_vaciar(tGBT_Vector *v);
void gbt_vector_destruir(tGBT_Vector *v);
uint8_t gbt_vector_cargar_de_archivo(tGBT_Vector *v, const char *nombreArch, size_t tamElem);
void gbt_vector_recorrer(tGBT_Vector *v, tGBT_Accion accion, void *extra);
void gbt_vector_ordenar(tGBT_Vector *v, eGBT_Ordenamiento metodo, tGBT_Cmp cmp);
int32_t gbt_vector_ord_buscar(const tGBT_Vector *v, void *elem, tGBT_Cmp cmp);
int32_t gbt_vector_ord_buscar_binaria(const tGBT_Vector *v, void *elem, tGBT_Cmp cmp);
uint8_t gbt_vector_ord_insertar(tGBT_Vector *v, void *elem, tGBT_Cmp cmp, tGBT_Actualizar actualizar);
uint8_t gbt_vector_insertar_al_final(tGBT_Vector *v, void *elem);
size_t gbt_vector_obtener_cantidad_elem(const tGBT_Vector *v);

void gbt_vector_it_crear(tGBT_VectorIterador *it, tGBT_Vector *v);
void* gbt_vector_it_primero(tGBT_VectorIterador *it);
void* gbt_vector_it_siguiente(tGBT_VectorIterador *it);
uint8_t gbt_vector_it_es_fin(tGBT_VectorIterador *it);


#endif // GBT_VECTOR_H_INCLUDED
