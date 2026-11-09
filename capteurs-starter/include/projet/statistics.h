#ifndef PROJET_STATISTICS_H
#define PROJET_STATISTICS_H

#include <stddef.h>

#include "projet/series.h"

typedef enum {
    TEMPERATURE_INDOOR,
    TEMPERATURE_OUTDOOR
} TemperatureKind;

typedef struct {
    size_t count;
    double minimum;
    double maximum;
    double average;
} TemperatureStatistics;

/**
 * Calcule les statistiques de la temperature indiquee par `kind` dans
 * `series`, puis les ecrit dans `statistics` : nombre de valeurs, minimum,
 * maximum et moyenne. `kind` vaut TEMPERATURE_INDOOR ou TEMPERATURE_OUTDOOR.
 * Renvoie 1 en cas de succes ; renvoie 0 si un pointeur est NULL, si la serie
 * est vide ou incoherente, ou si `kind` est invalide.
 */
int temperature_statistics_compute(
    const MeasureSeries *series,
    TemperatureKind kind,
    TemperatureStatistics *statistics
);

/**
 * Calcule dans `gap` la moyenne de (temperature interieure - temperature
 * exterieure) pour toutes les mesures de `series`. Renvoie 1 en cas de succes,
 * 0 si `series` ou `gap` est NULL, ou si la serie est vide ou incoherente.
 */
int temperature_average_gap(const MeasureSeries *series, double *gap);

#endif
