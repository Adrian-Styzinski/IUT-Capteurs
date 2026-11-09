#ifndef PROJET_MEASURE_H
#define PROJET_MEASURE_H

enum { MEASURE_TIME_SIZE = 40 };

typedef struct {
    char time[MEASURE_TIME_SIZE];
    double indoor_temperature;
    double outdoor_temperature;
    double indoor_humidity;
    double outdoor_humidity;
} TemperatureMeasure;

#endif
