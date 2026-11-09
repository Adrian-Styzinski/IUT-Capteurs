#ifndef PROJET_SENSOR_SOURCE_H
#define PROJET_SENSOR_SOURCE_H

#include "projet/series.h"

enum { SENSOR_SOURCE_UPDATED_AT_SIZE = 40 };

typedef struct {
    char updated_at[SENSOR_SOURCE_UPDATED_AT_SIZE];
    MeasureSeries points;
} SensorDataset;

/**
 * Initialise le jeu de donnees pointe par `dataset` : date vide et serie de
 * mesures vide. `dataset` peut etre NULL ; la fonction ne fait alors rien.
 * Ne renvoie aucune valeur.
 */
void sensor_dataset_init(SensorDataset *dataset);

/**
 * Libere les ressources du jeu de donnees pointe par `dataset`, puis le remet
 * dans l'etat initialise. `dataset` peut etre NULL ; la fonction ne fait alors
 * rien. Ne renvoie aucune valeur.
 */
void sensor_dataset_clear(SensorDataset *dataset);

/*
 * Ces deux fonctions sont l'unique point de raccordement de la bibliotheque
 * HTTP/JSON fournie par l'enseignant. Elles remplissent un jeu de mesures
 * typées sans exposer JSON ni le reseau au reste du projet.
 */
/**
 * Telecharge et analyse le document JSON accessible a l'URL `url`, puis place
 * les mesures lues dans `dataset`. `dataset` doit avoir ete initialise avec
 * sensor_dataset_init. Renvoie 1 en cas de succes, 0 en cas d'URL invalide,
 * d'erreur reseau, de JSON invalide ou de memoire insuffisante. En cas
 * d'echec, consulter sensor_source_last_error pour le detail.
 */
int sensor_source_load_url(const char *url, SensorDataset *dataset);

/**
 * Lit et analyse le document JSON du fichier `path`, puis place les mesures
 * lues dans `dataset`. `dataset` doit avoir ete initialise avec
 * sensor_dataset_init. Renvoie 1 en cas de succes, 0 si les arguments sont
 * invalides, si le fichier ou son contenu est invalide, ou si la memoire est
 * insuffisante. En cas d'echec, consulter sensor_source_last_error.
 */
int sensor_source_load_file(const char *path, SensorDataset *dataset);

/**
 * Retourne un pointeur vers le dernier message d'erreur produit par une
 * fonction de chargement. La chaine retournee appartient au module fourni :
 * elle ne doit pas etre modifiee ni liberee par l'appelant.
 */
const char *sensor_source_last_error(void);

#endif
