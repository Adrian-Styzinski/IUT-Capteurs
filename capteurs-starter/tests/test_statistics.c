#include <assert.h>
#include <math.h>

#include "projet/statistics.h"

int main(void)
{
    TemperatureMeasure points[] = {
        {"2026-09-24T08:15:00+02:00", 27.3, 27.8, 56.3, 55.5},
        {"2026-09-24T08:20:00+02:00", 27.2, 27.9, 56.3, 55.5},
        {"2026-09-24T08:25:00+02:00", 27.5, 27.6, 56.9, 55.4}
    };
    MeasureSeries empty = {NULL, 0, 0};
    MeasureSeries inconsistent = {NULL, 3, 3};
    MeasureSeries series = {points, 3, 3};
    TemperatureStatistics statistics;
    double gap;

    assert(!temperature_statistics_compute(NULL, TEMPERATURE_INDOOR, &statistics));
    assert(!temperature_statistics_compute(&series, TEMPERATURE_INDOOR, NULL));
    assert(!temperature_statistics_compute(&empty, TEMPERATURE_INDOOR, &statistics));
    assert(!temperature_statistics_compute(&inconsistent, TEMPERATURE_INDOOR, &statistics));
    assert(!temperature_statistics_compute(&series, (TemperatureKind)42, &statistics));

    assert(temperature_statistics_compute(&series, TEMPERATURE_INDOOR, &statistics));
    assert(statistics.count == 3);
    assert(fabs(statistics.minimum - 27.2) < 0.0001);
    assert(fabs(statistics.maximum - 27.5) < 0.0001);
    assert(fabs(statistics.average - (82.0 / 3.0)) < 0.0001);

    assert(temperature_statistics_compute(&series, TEMPERATURE_OUTDOOR, &statistics));
    assert(statistics.count == 3);
    assert(fabs(statistics.minimum - 27.6) < 0.0001);
    assert(fabs(statistics.maximum - 27.9) < 0.0001);
    assert(fabs(statistics.average - (83.3 / 3.0)) < 0.0001);

    assert(!temperature_average_gap(NULL, &gap));
    assert(!temperature_average_gap(&series, NULL));
    assert(!temperature_average_gap(&empty, &gap));
    assert(!temperature_average_gap(&inconsistent, &gap));
    assert(temperature_average_gap(&series, &gap));
    assert(fabs(gap + (1.3 / 3.0)) < 0.0001);
    return 0;
}
