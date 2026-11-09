#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "projet/report.h"

int main(void)
{
    TemperatureMeasure measures[] = {
        {"2026-09-24T08:15:00+02:00", 27.3, 27.8, 56.3, 55.5},
        {"2026-09-24T08:20:00+02:00", 27.2, 27.9, 56.3, 55.5},
        {"2026-09-24T08:25:00+02:00", 27.5, 27.6, 56.9, 55.4}
    };
    SensorDataset dataset = {
        "2026-09-24T08:30:00+02:00",
        {measures, 3, 3}
    };
    SensorReportOptions options;
    FILE *output;
    char terminal[512];
    size_t terminal_size;
    char label[] = "Atelier capteurs";

    assert(!sensor_report_options_init(NULL, "Atelier"));
    assert(!sensor_report_options_init(&options, NULL));
    assert(options.label == NULL);
    assert(!sensor_report_options_init(&options, ""));
    assert(options.label == NULL);
    assert(!sensor_report_options_init(&options, "Avec,virgule"));
    assert(!sensor_report_options_init(&options, "Avec\"guillemet"));
    assert(!sensor_report_options_init(&options, "Avec\nretour"));
    assert(!sensor_report_options_init(&options, "Avec\rretour"));
    assert(sensor_report_options_init(&options, label));
    label[0] = 'X';
    assert(strcmp(options.label, "Atelier capteurs") == 0);

    output = tmpfile();
    assert(output != NULL);
    sensor_report_print(output, &dataset, &options);
    assert(fflush(output) == 0);
    rewind(output);
    terminal_size = fread(terminal, 1, sizeof terminal - 1, output);
    terminal[terminal_size] = '\0';
    assert(strcmp(terminal,
        "Rapport : Atelier capteurs\n"
        "Mesures: 3\n"
        "Mise a jour: 2026-09-24T08:30:00+02:00\n"
        "Temperature interieure: min=27.20 max=27.50 moyenne=27.33\n"
        "Temperature exterieure: min=27.60 max=27.90 moyenne=27.77\n"
        "Ecart interieur-exterieur moyen: -0.43\n") == 0);
    assert(fclose(output) == 0);

    sensor_report_options_clear(&options);
    assert(options.label == NULL);
    return 0;
}
