#include "projet/series.h"
#include <stdlib.h>
#include "projet/statistics.h"
#include <string.h>

/* Capacité allouée à la toute première insertion. */
static const size_t TAILLE_SERIES_INITIALE = 20;

void measure_series_init(MeasureSeries *series)
{
    if (series == NULL){
        return;
    }
    series->items = NULL;
    series->count = 0;
    series->capacity = 0;
}

/* Ajoute measure à la fin de la série. Renvoie 1 si réussi, sinon 0 (série reste intacte et utilisable). */

int measure_series_append(MeasureSeries *series, TemperatureMeasure measure)
{
    (void)series;
    (void)measure;

    /* TODO S4: malloc lors de la premiere insertion, puis realloc si besoin. */

    if (series == NULL){
        return 0;
    }
    if (series->count == series->capacity) {
        TemperatureMeasure *nouveaux_items;
        size_t nouvelle_capa;
        /* Le malloc lors de la premiere insertion */
        if (series->items == NULL) {
            nouvelle_capa = TAILLE_SERIES_INITIALE;
            nouveaux_items = malloc(TAILLE_SERIES_INITIALE * sizeof *nouveaux_items);
        }

        /* Le realloc si besoin avec une capacite doublee */
        else {
            nouvelle_capa = series->capacity * 2;
            nouveaux_items = realloc(series->items, nouvelle_capa * sizeof *nouveaux_items);
        }

        /* En cas d'echec de l'allocation memoire*/
        if (nouveaux_items == NULL){
            return 0;
        }

        series->items = nouveaux_items;
        series->capacity = nouvelle_capa;
    }

    /* L'append de la valeur et incrementation de count */
    series->items[series->count] = measure;
    series->count++;
    return 1;
}

/* Libère la mémoire de la série et la vide (measure_series_init). */
void measure_series_clear(MeasureSeries *series)
{
    (void)series;

    /* TODO S4: liberer le tableau et remettre la serie dans son etat initial. */

    if (series == NULL) {
        return;
    }

    free(series->items);
    measure_series_init(series);
}
