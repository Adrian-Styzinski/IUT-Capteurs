#ifndef PROJET_SERIES_H
#define PROJET_SERIES_H

#include <stddef.h>

#include "projet/measure.h"

typedef struct {
    TemperatureMeasure *items;
    size_t count;
    size_t capacity;
} MeasureSeries;

/**
 * Initialise la serie pointee par `series` : elle devient vide et ne possede
 * aucun tableau alloue. `series` peut etre NULL ; la fonction ne fait alors
 * rien. Ne renvoie aucune valeur.
 */
void measure_series_init(MeasureSeries *series);

/**
 * Ajoute la mesure `measure` a la fin de la serie pointee par `series`.
 * La fonction alloue ou agrandit le tableau si necessaire. Renvoie 1 en cas de
 * succes, 0 si `series` est NULL ou si une allocation echoue. En cas d'echec,
 * la serie doit rester utilisable et son contenu existant doit etre conserve.
 */
int measure_series_append(MeasureSeries *series, TemperatureMeasure measure);

/**
 * Libere le tableau possede par la serie pointee par `series`, puis remet la
 * serie dans l'etat d'une serie vide. `series` peut etre NULL ; la fonction ne
 * fait alors rien. Ne renvoie aucune valeur.
 */
void measure_series_clear(MeasureSeries *series);

#endif
