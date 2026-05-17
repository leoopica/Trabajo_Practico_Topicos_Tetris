#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gbt_vector.h"

/* Crea un vector dinamico para elementos de tamano tamElem */
uint8_t gbt_vector_crear (tGBT_Vector *v, size_t tamElem)
{
    if (!v) { return GBT_VECTOR_SIN_MEM; }

    v->vec = malloc (10 * tamElem);
    if (!v->vec)
    {
        return GBT_VECTOR_SIN_MEM;
    }

    v->ce = 0;
    v->cap = 10;
    v->tamElem = tamElem;
    return GBT_VECTOR_TODO_OK;
}

/* Vacia el vector (no libera memoria) */
void gbt_vector_vaciar (tGBT_Vector *v)
{
    if (v) { v->ce = 0; }
}

/* Destruye el vector y libera memoria */
void gbt_vector_destruir (tGBT_Vector *v)
{
    if (!v) { return; }
    free (v->vec);
    v->vec = NULL;
    v->ce = 0;
    v->cap = 0;
}

/* Carga datos desde un archivo binario */
uint8_t gbt_vector_cargar_de_archivo (tGBT_Vector *v, const char *nombreArch, size_t tamElem)
{
    FILE *f;
    void *buff;
    size_t leidos;

    if (!v || !nombreArch) { return GBT_VECTOR_ERR_ARCH; }

    f = fopen (nombreArch, "rb");
    if (!f) { return GBT_VECTOR_ERR_ARCH; }

    buff = malloc (tamElem);
    if (!buff)
    {
        fclose (f);
        return GBT_VECTOR_SIN_MEM;
    }

    while ((leidos = fread (buff, tamElem, 1, f)) == 1)
    {
        if (gbt_vector_insertar_al_final (v, buff) != GBT_VECTOR_TODO_OK)
        {
            free (buff);
            fclose (f);
            return GBT_VECTOR_SIN_MEM;
        }
    }

    free (buff);
    fclose (f);
    return GBT_VECTOR_TODO_OK;
}

/* Recorre el vector ejecutando una accion por cada elemento */
void gbt_vector_recorrer (tGBT_Vector *v, tGBT_Accion accion, void *extra)
{
    size_t i;

    if (!v || !accion) { return; }

    for (i = 0; i < v->ce; i++)
    {
        accion ((char*)v->vec + i * v->tamElem, extra);
    }
}

/* Ordena el vector usando el metodo especificado y la funcion de comparacion */
void gbt_vector_ordenar (tGBT_Vector *v, eGBT_Ordenamiento metodo, tGBT_Cmp cmp)
{
    size_t i;
    size_t j;
    size_t min_idx;
    void *tmp;

    if (!v || !cmp || v->ce <= 1) { return; }

    tmp = malloc (v->tamElem);
    if (!tmp) { return; }

    switch (metodo)
    {
        case GBT_BURBUJEO:
            for (i = 0; i < v->ce - 1; i++)
            {
                for (j = 0; j < v->ce - 1 - i; j++)
                {
                    void *e1 = (char*)v->vec + j * v->tamElem;
                    void *e2 = (char*)v->vec + (j + 1) * v->tamElem;

                    if (cmp (e1, e2) > 0)
                    {
                        memcpy (tmp, e1, v->tamElem);
                        memcpy (e1, e2, v->tamElem);
                        memcpy (e2, tmp, v->tamElem);
                    }
                }
            }
            break;

        case GBT_SELECCION:
            for (i = 0; i < v->ce - 1; i++)
            {
                min_idx = i;
                for (j = i + 1; j < v->ce; j++)
                {
                    void *e_act = (char*)v->vec + j * v->tamElem;
                    void *e_min = (char*)v->vec + min_idx * v->tamElem;
                    if (cmp (e_act, e_min) < 0)
                    {
                        min_idx = j;
                    }
                }
                if (min_idx != i)
                {
                    void *ei = (char*)v->vec + i * v->tamElem;
                    void *emin = (char*)v->vec + min_idx * v->tamElem;
                    memcpy (tmp, ei, v->tamElem);
                    memcpy (ei, emin, v->tamElem);
                    memcpy (emin, tmp, v->tamElem);
                }
            }
            break;

        case GBT_INSERCION:
            for (i = 1; i < v->ce; i++)
            {
                memcpy (tmp, (char*)v->vec + i * v->tamElem, v->tamElem);
                j = i;
                while (j > 0 && cmp ((char*)v->vec + (j - 1) * v->tamElem, tmp) > 0)
                {
                    memcpy ((char*)v->vec + j * v->tamElem, (char*)v->vec + (j - 1) * v->tamElem, v->tamElem);
                    j--;
                }
                memcpy ((char*)v->vec + j * v->tamElem, tmp, v->tamElem);
            }
            break;
    }

    free (tmp);
}

/* Busqueda secuencial en vector ordenado */
int32_t gbt_vector_ord_buscar (const tGBT_Vector *v, void *elem, tGBT_Cmp cmp)
{
    size_t i;

    if (!v || !cmp) { return -1; }

    for (i = 0; i < v->ce; i++)
    {
        if (cmp ((char*)v->vec + i * v->tamElem, elem) == 0)
        {
            return (int32_t)i;
        }
    }

    return -1;
}

/* Busqueda binaria en vector ordenado */
int32_t gbt_vector_ord_buscar_binaria (const tGBT_Vector *v, void *elem, tGBT_Cmp cmp)
{
    int32_t inicio;
    int32_t fin;
    int32_t medio;

    if (!v || !cmp || v->ce == 0) { return -1; }

    inicio = 0;
    fin = (int32_t)v->ce - 1;

    while (inicio <= fin)
    {
        medio = (inicio + fin) / 2;
        int res = cmp ((char*)v->vec + medio * v->tamElem, elem);

        if (res == 0) { return medio; }
        if (res < 0) { inicio = medio + 1; }
        else { fin = medio - 1; }
    }

    return -1;
}

/* Inserta un elemento en orden en el vector */
uint8_t gbt_vector_ord_insertar (tGBT_Vector *v, void *elem, tGBT_Cmp cmp, tGBT_Actualizar actualizar)
{
    size_t i;
    size_t pos;
    void *dest;

    if (!v || !cmp) { return GBT_VECTOR_ERR_TAM; }

    /* Buscar posicion de insercion */
    pos = v->ce;
    for (i = 0; i < v->ce; i++)
    {
        int res = cmp ((char*)v->vec + i * v->tamElem, elem);
        if (res == 0)
        {
            if (actualizar)
            {
                actualizar ((char*)v->vec + i * v->tamElem, elem);
            }
            return GBT_VECTOR_DUPLICADO;
        }
        if (res > 0)
        {
            pos = i;
            break;
        }
    }

    /* Redimensionar si es necesario */
    if (v->ce >= v->cap)
    {
        size_t nueva_cap = v->cap ? v->cap * 2 : 10;
        void *nuevo_vec = realloc (v->vec, nueva_cap * v->tamElem);

        if (!nuevo_vec) { return GBT_VECTOR_SIN_MEM; }
        v->vec = nuevo_vec;
        v->cap = nueva_cap;
    }

    /* Desplazar elementos */
    dest = (char*)v->vec + (pos + 1) * v->tamElem;
    memmove (dest, (char*)v->vec + pos * v->tamElem, (v->ce - pos) * v->tamElem);

    /* Insertar */
    memcpy ((char*)v->vec + pos * v->tamElem, elem, v->tamElem);
    v->ce++;

    return GBT_VECTOR_TODO_OK;
}

/* Inserta un elemento al final del vector */
uint8_t gbt_vector_insertar_al_final (tGBT_Vector *v, void *elem)
{
    if (!v) { return GBT_VECTOR_ERR_TAM; }

    if (v->ce >= v->cap)
    {
        size_t nueva_cap = v->cap ? v->cap * 2 : 10;
        void *nuevo_vec = realloc (v->vec, nueva_cap * v->tamElem);

        if (!nuevo_vec) { return GBT_VECTOR_SIN_MEM; }
        v->vec = nuevo_vec;
        v->cap = nueva_cap;
    }

    memcpy ((char*)v->vec + v->ce * v->tamElem, elem, v->tamElem);
    v->ce++;

    return GBT_VECTOR_TODO_OK;
}

/* Retorna la cantidad de elementos almacenados */
size_t gbt_vector_obtener_cantidad_elem (const tGBT_Vector *v)
{
    if (!v) { return 0; }
    return v->ce;
}

/* Crea un iterador para el vector */
void gbt_vector_it_crear (tGBT_VectorIterador *it, tGBT_Vector *v)
{
    if (!it || !v) { return; }

    it->v = v;
    it->tamElem = v->tamElem;
    it->act = v->vec;
    it->ult = (char*)v->vec + v->ce * v->tamElem;
}

/* Retorna puntero al primer elemento y avanza el iterador */
void *gbt_vector_it_primero (tGBT_VectorIterador *it)
{
    if (!it || !it->v || it->v->ce == 0) { return NULL; }

    it->act = it->v->vec;
    return it->act;
}

/* Retorna puntero al siguiente elemento y avanza */
void *gbt_vector_it_siguiente (tGBT_VectorIterador *it)
{
    if (!it || !it->v) { return NULL; }

    it->act = (char*)it->act + it->tamElem;
    if (it->act >= it->ult) { return NULL; }

    return it->act;
}

/* Verifica si el iterador llego al final */
uint8_t gbt_vector_it_es_fin (tGBT_VectorIterador *it)
{
    if (!it || !it->v) { return 1; }
    return (it->act >= it->ult);
}
