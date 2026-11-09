#include <assert.h>

#include "projet/series.h"

int main(void)
{
    MeasureSeries series;
    TemperatureMeasure measure = {"2026-09-30T14:00:00+02:00", 20.0, 15.0, 50.0, 60.0};
    size_t i;

    measure_series_init(&series);
    assert(series.items == NULL);
    assert(series.count == 0);
    assert(series.capacity == 0);

    for (i = 0; i < 17; i++) {
        measure.indoor_temperature = (double)i;
        assert(measure_series_append(&series, measure));
    }
    assert(series.count == 17);
    assert(series.capacity >= 17);
    assert(series.items[16].indoor_temperature == 16.0);

    measure_series_clear(&series);
    assert(series.items == NULL);
    assert(series.count == 0);
    assert(series.capacity == 0);
    return 0;
}
