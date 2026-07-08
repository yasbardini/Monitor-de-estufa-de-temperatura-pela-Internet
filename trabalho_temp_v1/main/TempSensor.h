#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

class TempSensor
{
public:
    TempSensor();
    float read();

private:
    float currentTemp;
    bool rising;
};

#endif