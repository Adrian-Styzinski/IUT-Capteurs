#ifndef PROJET_REPORT_H
#define PROJET_REPORT_H

#include <stdio.h>

#include "projet/sensor_source.h"

/*
 * Le libelle appartient aux options : il est copie a l'initialisation et doit
 * donc etre libere avec sensor_report_options_clear.
 */
typedef struct {
    char *label;
} SensorReportOptions;

/**
 * Initialise `options` en y copiant le texte `label`. Le libelle ne doit etre
 * ni NULL, ni vide, ni contenir ',', '"', '\n' ou '\r', car il sera ecrit
 * dans un CSV. Renvoie 1 si la copie reussit ; renvoie 0 sinon. Apres un echec
 * avec `options` non NULL, `options->label` vaut NULL.
 */
int sensor_report_options_init(SensorReportOptions *options, const char *label);

/**
 * Libere le libelle possede par `options` et remet son pointeur a NULL.
 * `options` peut etre NULL ; la fonction ne fait alors rien. Ne renvoie aucune
 * valeur.
 */
void sensor_report_options_clear(SensorReportOptions *options);

/**
 * Ecrit un rapport lisible dans le flux deja ouvert `output`, a partir des
 * mesures de `dataset` et du libelle contenu dans `options`. Les trois
 * pointeurs doivent etre valides. Ne ferme pas `output` et ne renvoie aucune
 * valeur.
 */
void sensor_report_print(
    FILE *output,
    const SensorDataset *dataset,
    const SensorReportOptions *options
);

/**
 * Ecrit dans le flux deja ouvert `output` l'en-tete CSV puis une ligne de
 * synthese obtenue a partir de `dataset` et de `options`. Ne ferme pas
 * `output`. Renvoie 1 si toutes les ecritures reussissent ; renvoie 0 si un
 * argument est invalide, si les donnees ne permettent pas de calculer les
 * statistiques, ou en cas d'erreur d'ecriture.
 */
int sensor_report_write_csv(
    FILE *output,
    const SensorDataset *dataset,
    const SensorReportOptions *options
);

#endif
