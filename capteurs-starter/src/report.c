#include "projet/report.h"
#include <string.h>
#include <stdlib.h>
#include "projet/statistics.h"

/* Valide le label puis options->label. Renvoie 1 si réussi, 0 sinon (options->label vaut NULL. */
int sensor_report_options_init(SensorReportOptions *options, const char *label)
{
    if (options != NULL) {
        options->label = NULL;
    }
    else {
        return 0;
    }

    /* TODO S4 : verifier le libelle, le copier avec strlen/malloc/memcpy,
     * puis affecter la copie a options->label. Refuser ',', '"', '\n' et
     * '\r'. Ce stub accepte un libelle non nul pour que le chargement des
     * donnees des jalons precedents reste executable avant l'etape S4. */
    if (label == NULL || label[0] == '\0'){
        return 0;
    }
    size_t longeurLabel = strlen(label);
    for (size_t i = 0; i < longeurLabel; i++) {
        if (label[i] == ',' || label[i] == '"' || label[i] == '\n' || label[i] == '\r') {
            return 0;
        }
    }
    options->label = malloc(longeurLabel + 1);
    if (options->label == NULL) {
        return 0;
    }
    memcpy(options->label, label, longeurLabel + 1);
    return 1;
}

/* Libère libellé et remet pointeur à NULL. */
void sensor_report_options_clear(SensorReportOptions *options)
{
    (void)options;
    /* TODO S4 : liberer options->label et remettre le pointeur a NULL. */
    if (options != NULL) {
        free(options->label);
        options->label = NULL;
    }
}

/* Affiche le rapport lisible. */
void sensor_report_print(
    FILE *output,
    const SensorDataset *dataset,
    const SensorReportOptions *options
)
{
    (void)dataset;
    (void)options;


    /* calcule les statistiques de TEMPERATURE_INDOOR et TEMPERATURE_OUTDOOR */
    TemperatureStatistics stats_in;
    TemperatureStatistics stats_out;

    temperature_statistics_compute(&dataset->points, TEMPERATURE_INDOOR, &stats_in);
    temperature_statistics_compute(&dataset->points, TEMPERATURE_OUTDOOR, &stats_out);

    double ecart_moyen = stats_in.average - stats_out.average;
    /* TODO S4: afficher le rapport a partir des statistiques. */
    fprintf(output, "Rapport : %s\n", options->label);
    fprintf(output, "Mesures: %zu\n", dataset->points.count);
    fprintf(output, "Mise a jour: %s\n", dataset->updated_at);
    fprintf(output, "Temperature interieure: min=%.2f ",stats_in.minimum);
    fprintf(output,"max=%.2f ",stats_in.maximum);
    fprintf(output,"moyenne=%.2f\n",stats_in.average);
    fprintf(output, "Temperature exterieure: min=%.2f ",stats_out.minimum);
    fprintf(output,"max=%.2f ",stats_out.maximum);
    fprintf(output,"moyenne=%.2f\n",stats_out.average);
    fprintf(output, "Ecart interieur-exterieur moyen: %.2f\n", ecart_moyen);
}

/* Écrit l'en-tête CSV puis une ligne de synthèse. */
int sensor_report_write_csv(
    FILE *output,
    const SensorDataset *dataset,
    const SensorReportOptions *options
)
{
    (void)output;
    (void)dataset;
    (void)options;

    /* TODO S4 : calculer les statistiques et ecrire l'en-tete CSV puis la
     * ligne de synthese. Utiliser fprintf et verifier ses valeurs de retour. */
    TemperatureStatistics indoor, outdoor;
    double gap;

    if (output == NULL || dataset == NULL || options == NULL
        || options->label == NULL) {
        return 0;
    }
    if (!temperature_statistics_compute(&dataset->points, TEMPERATURE_INDOOR, &indoor)
        || !temperature_statistics_compute(&dataset->points, TEMPERATURE_OUTDOOR, &outdoor)
        || !temperature_average_gap(&dataset->points, &gap)) {
        return 0;
    }

    if (fprintf(output,
            "label,updated_at,count,indoor_minimum,indoor_maximum,indoor_average,"
            "outdoor_minimum,outdoor_maximum,outdoor_average,average_gap\n") < 0) {
        return 0;
    }
    if (fprintf(output, "%s,%s,%zu,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
            options->label, dataset->updated_at, indoor.count,
            indoor.minimum, indoor.maximum, indoor.average,
            outdoor.minimum, outdoor.maximum, outdoor.average, gap) < 0) {
        return 0;
    }
    return 1;
}
