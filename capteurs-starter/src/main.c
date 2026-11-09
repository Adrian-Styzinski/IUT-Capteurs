#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "projet/report.h"
#include "projet/sensor_source.h"

#define SENSORS_URL "https://tribequa.org/assets/data/sensors-history.json"
#define DEFAULT_CSV_PATH "rapport.csv"
#define DEFAULT_REPORT_LABEL "Rapport capteurs"

static void usage(const char *program)
{
    fprintf(stderr,
        "Usage: %s [--url URL | --file FICHIER] [--csv FICHIER] [--label TEXTE]\n",
        program);
}

typedef struct {
    const char *url;
    const char *file;
    const char *csv_path;
    const char *label;
} ProgramOptions;

/* Analyse argv et remplit *options. Renvoie 1 si les arguments sont valides 0 sinon (main Renvoie 2) */
static int parse_options(int argc, char *argv[], ProgramOptions *options)
{
    (void)argc;
    (void)argv;

    if (options == NULL) {
        return 0;
    }

    options->url = NULL;
    options->file = NULL;
    options->csv_path = DEFAULT_CSV_PATH;
    options->label = DEFAULT_REPORT_LABEL;

    /* TODO S2/S4 : parcourir argv avec une boucle. Accepter --url URL ou
     * --file FICHIER (au plus un), ainsi que --csv FICHIER et --label TEXTE,
     * dans n'importe quel ordre. Les deux derniers remplacent les valeurs par
     * defaut du fichier CSV et du libelle. Utiliser strcmp et refuser les
     * options inconnues, dupliquees ou incompletes. Sans source, l'URL par
     * defaut est utilisee. */
    int i;
    bool csv_given = false;
    bool label_given = false;

    /* On part de i = 1 car argv[0] est le nom du programme. */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--url") == 0) {
            /* Refus si la valeur est absente ou --url déjà donné ou --file déjà donné  */
            if (i + 1 >= argc || options->url != NULL || options->file != NULL) {
                return 0;
            }
            /* ++i avance ET la consomme (ne la relira pas comme une option). */
            options->url = argv[++i];
        } else if (strcmp(argv[i], "--file") == 0) {
            /* Refus si la valeur est absente ou --url déjà donné ou --file déjà donné  */
            if (i + 1 >= argc || options->file != NULL || options->url != NULL) {
                return 0;
            }
            options->file = argv[++i];

        } else if (strcmp(argv[i], "--csv") == 0) {
            /* Refus si la valeur est absente ou --csv a déjà été vu. */
            if (i + 1 >= argc || csv_given) {
                return 0;
            }
            options->csv_path = argv[++i];
            csv_given = true;
        } else if (strcmp(argv[i], "--label") == 0) {
            /* Refus si la valeur est absente ou --label a déjà été vu. */
            if (i + 1 >= argc || label_given) {
                return 0;
            }
            options->label = argv[++i];
            label_given = true;
        } else {
            return 0;
        }
    }
    return 1;
}

/* Écrit le rapport CSV dans le fichier `path`. Renvoie 1 si tout réussit. */
static int write_csv_report(
    const char *path,
    const SensorDataset *dataset,
    const SensorReportOptions *report_options
)
{
    (void)path;
    (void)dataset;
    (void)report_options;

    /* TODO S4 : ouvrir path avec fopen("w"), appeler
     * sensor_report_write_csv, detecter les erreurs (dont fclose), fermer le
     * fichier dans tous les cas et retourner 1 en cas de succes. */
    
    FILE *file;
    int succes;

    /* Refus des arguments invalides. */
    if (path == NULL || dataset == NULL || report_options == NULL) {
        return 0;
    }
    /* Ouverture en écriture (w crée le fichier ou le vide) */
    file = fopen(path, "w");
    if (file == NULL) {
        return 0;
    }

    /* écriture */
    succes = sensor_report_write_csv(file, dataset, report_options);

    /* si fclose échoue. */
    if (fclose(file) != 0) {
        succes = 0;
    }
    return succes;
}

int main(int argc, char *argv[])
{
    SensorDataset dataset;
    SensorReportOptions report_options;
    ProgramOptions options;
    int loaded;

    sensor_dataset_init(&dataset);

    if (!parse_options(argc, argv, &options)) {
        usage(argv[0]);
        return 2;
    }

    if (!sensor_report_options_init(&report_options, options.label)) {
        fprintf(stderr, "Libelle de rapport invalide ou memoire insuffisante.\n");
        return 1;
    }

    if (options.file != NULL) {
        loaded = sensor_source_load_file(options.file, &dataset);
    } else if (options.url == NULL) {
        loaded = sensor_source_load_url(SENSORS_URL, &dataset);
    } else {
        loaded = sensor_source_load_url(options.url, &dataset);
    }

    if (!loaded) {
        fprintf(stderr, "Impossible de charger les capteurs: %s\n",
            sensor_source_last_error());
        sensor_report_options_clear(&report_options);
        sensor_dataset_clear(&dataset);
        return 1;
    }

    sensor_report_print(stdout, &dataset, &report_options);
    /* Le stub TODO retourne toujours 0 avant son implementation. */
    // cppcheck-suppress knownConditionTrueFalse
    if (!write_csv_report(options.csv_path, &dataset, &report_options)) {
        fprintf(stderr, "Impossible d'ecrire le rapport CSV.\n");
        sensor_report_options_clear(&report_options);
        sensor_dataset_clear(&dataset);
        return 1;
    }

    sensor_report_options_clear(&report_options);
    sensor_dataset_clear(&dataset);
    return 0;
}
