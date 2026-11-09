#include "projet/statistics.h"
#include <string.h>
#include <stdlib.h>
#include "projet/report.h"

/**
 * Calcule les statistiques d'une seule temperature dans une serie deja
 * construite. Cette fonction ne doit ni allouer de memoire ni modifier la
 * serie : au jalon S3, les mesures sont fournies par un tableau sur la pile.
 *
 * Etapes attendues :
 *
 * 1. Refuser `series == NULL`, `statistics == NULL`, une serie vide, ou une
 *    serie incoherente (`series->items == NULL` alors que `series->count > 0`).
 * 2. Refuser une valeur de `kind` differente de TEMPERATURE_INDOOR et
 *    TEMPERATURE_OUTDOOR.
 * 3. Pour chaque element de `series->items`, choisir soit
 *    `indoor_temperature`, soit `outdoor_temperature` selon `kind`.
 * 4. Initialiser minimum, maximum et la somme avec la premiere valeur, puis
 *    parcourir les valeurs restantes pour mettre a jour minimum, maximum et
 *    la somme.
 * 5. En cas de succes, affecter `statistics->count`, `statistics->minimum`,
 *    `statistics->maximum` et `statistics->average` (somme / count), puis
 *    retourner 1. Retourner 0 dans tous les cas invalides.
 */
int temperature_statistics_compute(
    const MeasureSeries *series,
    TemperatureKind kind,
    TemperatureStatistics *statistics
)
{
    (void)series;
    (void)kind;
    (void)statistics;

    double minimum;
    double maximum;
    double somme;

    /* Étape 1 : refuser tout ce qui est invalide. */
    if (series == NULL || statistics == NULL || series->items == NULL || series->count == 0 || series->count > series->capacity)
    {
        return 0;
    }

    /* Étapes 2 et 4 : on initialise min, max et somme avec la première valeur */
    if (kind == TEMPERATURE_INDOOR){
        minimum = series->items[0].indoor_temperature;
        maximum = series->items[0].indoor_temperature;
        somme   = series->items[0].indoor_temperature;
    }
    else if (kind == TEMPERATURE_OUTDOOR){
        minimum = series->items[0].outdoor_temperature;
        maximum = series->items[0].outdoor_temperature;
        somme   = series->items[0].outdoor_temperature;
    }
    else {
        return 0;
    }

    /* Étape 3 : on parcourt les mesures RESTANTES (i=1) */
    for (size_t i = 1; i < series->count; i++)
    {
        if (kind == TEMPERATURE_INDOOR)
        {
            if (series->items[i].indoor_temperature > maximum)
            {
                maximum = series->items[i].indoor_temperature;
            } 
            if (series->items[i].indoor_temperature < minimum)
            {
                minimum = series->items[i].indoor_temperature;
            }
            somme += series->items[i].indoor_temperature;
        }
        else 
        {
            if (series->items[i].outdoor_temperature > maximum)
            {
                maximum = series->items[i].outdoor_temperature;
            } 
            if (series->items[i].outdoor_temperature < minimum)
            {
                minimum = series->items[i].outdoor_temperature;
            }
            somme += series->items[i].outdoor_temperature;
        }
    }
     /* Étape 5 : on écrit les résultats */
    statistics->minimum = minimum;
    statistics->maximum = maximum;
    statistics->count   = series->count;
    statistics->average = (double)somme / series->count; // Force la division flottante

    return 1;
}

/* Calcule l'écart moyen (intérieur - extérieur) dans *gap. Renvoie 1 si calcul est possible 0 sinon. */
int temperature_average_gap(const MeasureSeries *series, double *gap)
{
    if (series == NULL || gap == NULL) {
        return 0;
    }

    TemperatureStatistics statiqueInterieure;
    TemperatureStatistics statiqueExterieure;
    
    if (temperature_statistics_compute(series, TEMPERATURE_INDOOR, &statiqueInterieure) == 1 &&
        temperature_statistics_compute(series, TEMPERATURE_OUTDOOR, &statiqueExterieure) == 1 )
    {
        *gap = statiqueInterieure.average - statiqueExterieure.average;
        return 1;
    }
    
    return 0;
}
