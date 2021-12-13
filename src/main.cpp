#include "Adafruit_VL53L0X.h"

#define PHASE_FAIL_RANGE_STATUS 4

auto vl53l0xSensor = Adafruit_VL53L0X();

void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(1);

    if (!vl53l0xSensor.begin())
    {
        Serial.println("Failed to boot VL53L0X");
        while(true);
    }
    if (!vl53l0xSensor.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE))
    {
        Serial.println("Failed to configure VL53L0X");
        while(true);
    }
}

void loop()
{
    VL53L0X_RangingMeasurementData_t measure;
    vl53l0xSensor.rangingTest(&measure);

    if (measure.RangeStatus != PHASE_FAIL_RANGE_STATUS)
    {
        Serial.print(measure.RangeMilliMeter / 10.0);
        Serial.println(" cm");
    }
    else
    {
        Serial.println(" out of range ");
    }
    delay(250);
}
