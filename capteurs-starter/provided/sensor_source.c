#include "projet/sensor_source.h"

#include <curl/curl.h>
#include <cJSON.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { ERROR_SIZE = 256 };

typedef struct {
    char *data;
    size_t size;
} DownloadBuffer;

static char last_error[ERROR_SIZE];
static int curl_is_initialized;

static void set_error(const char *message)
{
    snprintf(last_error, sizeof last_error, "%s", message);
}

static int initialize_curl(void)
{
    if (curl_is_initialized) {
        return 1;
    }
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        set_error("Initialisation de libcurl impossible.");
        return 0;
    }
    if (atexit(curl_global_cleanup) != 0) {
        curl_global_cleanup();
        set_error("Enregistrement du nettoyage de libcurl impossible.");
        return 0;
    }
    curl_is_initialized = 1;
    return 1;
}

void sensor_dataset_init(SensorDataset *dataset)
{
    if (dataset == NULL) {
        return;
    }
    dataset->updated_at[0] = '\0';
    measure_series_init(&dataset->points);
}

void sensor_dataset_clear(SensorDataset *dataset)
{
    if (dataset == NULL) {
        return;
    }
    measure_series_clear(&dataset->points);
    dataset->updated_at[0] = '\0';
}

static int copy_string(char *destination, size_t destination_size, const char *source)
{
    size_t length;

    if (destination == NULL || source == NULL) {
        return 0;
    }
    length = strlen(source);
    if (length >= destination_size) {
        return 0;
    }
    memcpy(destination, source, length + 1);
    return 1;
}

static int parse_measure(const cJSON *json, TemperatureMeasure *measure)
{
    const cJSON *time = cJSON_GetObjectItemCaseSensitive(json, "time");
    const cJSON *indoor_temperature = cJSON_GetObjectItemCaseSensitive(
        json, "indoor_temperature");
    const cJSON *outdoor_temperature = cJSON_GetObjectItemCaseSensitive(
        json, "outdoor_temperature");
    const cJSON *indoor_humidity = cJSON_GetObjectItemCaseSensitive(
        json, "indoor_humidity");
    const cJSON *outdoor_humidity = cJSON_GetObjectItemCaseSensitive(
        json, "outdoor_humidity");

    if (!cJSON_IsString(time) || !cJSON_IsNumber(indoor_temperature)
        || !cJSON_IsNumber(outdoor_temperature) || !cJSON_IsNumber(indoor_humidity)
        || !cJSON_IsNumber(outdoor_humidity)
        || !copy_string(measure->time, sizeof measure->time, time->valuestring)) {
        return 0;
    }

    measure->indoor_temperature = indoor_temperature->valuedouble;
    measure->outdoor_temperature = outdoor_temperature->valuedouble;
    measure->indoor_humidity = indoor_humidity->valuedouble;
    measure->outdoor_humidity = outdoor_humidity->valuedouble;
    return 1;
}

static int sensor_source_parse_json(const char *json_text, SensorDataset *dataset)
{
    cJSON *root;
    const cJSON *updated_at;
    const cJSON *points;
    const cJSON *point;
    TemperatureMeasure measure;

    if (json_text == NULL || dataset == NULL) {
        set_error("Document JSON ou jeu de donnees invalide.");
        return 0;
    }

    root = cJSON_Parse(json_text);
    if (root == NULL) {
        set_error("Le document recu n'est pas un JSON valide.");
        return 0;
    }

    updated_at = cJSON_GetObjectItemCaseSensitive(root, "updated_at");
    points = cJSON_GetObjectItemCaseSensitive(root, "points");
    if (!cJSON_IsString(updated_at) || !cJSON_IsArray(points)
        || !copy_string(dataset->updated_at, sizeof dataset->updated_at,
            updated_at->valuestring)) {
        set_error("Le JSON ne respecte pas le format des capteurs.");
        cJSON_Delete(root);
        return 0;
    }

    cJSON_ArrayForEach(point, points) {
        if (!parse_measure(point, &measure)
            || !measure_series_append(&dataset->points, measure)) {
            set_error("Une mesure est invalide ou la memoire est insuffisante.");
            cJSON_Delete(root);
            return 0;
        }
    }

    cJSON_Delete(root);
    return 1;
}

static size_t write_to_buffer(
    char *contents,
    size_t size,
    size_t count,
    void *user_data
)
{
    DownloadBuffer *buffer = user_data;
    size_t bytes;
    char *resized;

    if (count != 0 && size > SIZE_MAX / count) {
        return 0;
    }
    bytes = size * count;
    if (bytes > SIZE_MAX - buffer->size - 1) {
        return 0;
    }

    resized = realloc(buffer->data, buffer->size + bytes + 1);
    if (resized == NULL) {
        return 0;
    }
    buffer->data = resized;
    memcpy(buffer->data + buffer->size, contents, bytes);
    buffer->size += bytes;
    buffer->data[buffer->size] = '\0';
    return bytes;
}

static int load_text_file(const char *path, DownloadBuffer *buffer)
{
    FILE *input;
    char chunk[4096];
    size_t read_count;

    input = fopen(path, "rb");
    if (input == NULL) {
        set_error("Impossible d'ouvrir le fichier JSON.");
        return 0;
    }

    while ((read_count = fread(chunk, 1, sizeof chunk, input)) > 0) {
        if (write_to_buffer(chunk, 1, read_count, buffer) != read_count) {
            fclose(input);
            set_error("Memoire insuffisante pour lire le fichier JSON.");
            return 0;
        }
    }

    if (ferror(input)) {
        fclose(input);
        set_error("Erreur de lecture du fichier JSON.");
        return 0;
    }
    fclose(input);
    return 1;
}

static int load_json_buffer(DownloadBuffer *buffer, SensorDataset *dataset)
{
    int success;

    if (buffer->data == NULL) {
        set_error("Le document JSON est vide.");
        return 0;
    }
    sensor_dataset_clear(dataset);
    success = sensor_source_parse_json(buffer->data, dataset);
    if (!success) {
        sensor_dataset_clear(dataset);
    }
    return success;
}

int sensor_source_load_url(const char *url, SensorDataset *dataset)
{
    CURL *curl;
    CURLcode result;
    long status = 0;
    DownloadBuffer buffer = {0};
    int success;

    if (url == NULL || dataset == NULL) {
        set_error("URL ou jeu de donnees invalide.");
        return 0;
    }
    if (!initialize_curl()) {
        return 0;
    }

    curl = curl_easy_init();
    if (curl == NULL) {
        set_error("Creation de la requete HTTP impossible.");
        return 0;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "sae-c-capteurs/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(curl);
    if (result != CURLE_OK || status < 200 || status >= 300) {
        snprintf(last_error, sizeof last_error, "Telechargement HTTP impossible: %s.",
            curl_easy_strerror(result));
        free(buffer.data);
        return 0;
    }

    success = load_json_buffer(&buffer, dataset);
    free(buffer.data);
    return success;
}

int sensor_source_load_file(const char *path, SensorDataset *dataset)
{
    DownloadBuffer buffer = {0};
    int success;

    if (path == NULL || dataset == NULL) {
        set_error("Fichier ou jeu de donnees invalide.");
        return 0;
    }
    if (!load_text_file(path, &buffer)) {
        free(buffer.data);
        return 0;
    }
    success = load_json_buffer(&buffer, dataset);
    free(buffer.data);
    return success;
}

const char *sensor_source_last_error(void)
{
    return last_error;
}
