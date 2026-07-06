#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

class TempSensor
{
public:
    TempSensor() : fakeTemp(25.0f), direction(1.0f) {}

    float read()
    {
        fakeTemp += direction * 0.7f;
        if (fakeTemp >= 35.0f)
            direction = -1.0f;
        if (fakeTemp <= 15.0f)
            direction = 1.0f;
        return fakeTemp;
    }

private:
    float fakeTemp;
    float direction;
};

#endif