#include "TempSensor.h"

TempSensor::TempSensor() : currentTemp(24.0f), rising(true) {}

float TempSensor::read()
{
    if (rising)
    {
        currentTemp += 0.6f;
        if (currentTemp >= 31.0f)
        {
            rising = false;
        }
    }
    else
    {
        currentTemp -= 0.6f;
        if (currentTemp <= 18.0f)
        {
            rising = true;
        }
    }

    return currentTemp;
}