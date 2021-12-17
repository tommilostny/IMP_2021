#include <stdexcept>
#include <Adafruit_SSD1306.h>
#include "Adafruit_VL53L0X.h"

// OLED display constants.
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_MOSI     23
#define OLED_CLK      18
#define OLED_DC       16
#define OLED_CS       5
#define OLED_RESET    17

// VL53L0X sensor constants.
#define VL53L0X_FAIL_RANGE_STATUS 4
#define VL53L0X_MAX_RANGE 255

// Global variables for the OLED display and VL53L0X sensor.
Adafruit_VL53L0X distanceSensor = Adafruit_VL53L0X();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Setup VL53L0X sensor for long range mode measurement.
inline void DistanceSensorSetup()
{
    if (!distanceSensor.begin())
    {
        Serial.println("Failed to boot VL53L0X");
        while (true);
    }
    if (!distanceSensor.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE))
    {
        Serial.println("Failed to configure VL53L0X");
        while (true);
    }
}

// Setup SPI OLED display.
inline void DisplaySetup()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC))
    {
        Serial.println("SSD1306 allocation failed");
        while (true);
    }
}

// Gets the distance in centimeters from the VL53L0X sensor.
// Throws std::out_of_range if the measurement fails (is out of range).
double MeasureDistanceCm()
{
    VL53L0X_RangingMeasurementData_t measure;
    distanceSensor.rangingTest(&measure);
    double distance;

    if (measure.RangeStatus == VL53L0X_FAIL_RANGE_STATUS || (distance = measure.RangeMilliMeter / 10.0) > VL53L0X_MAX_RANGE)
    {
        throw std::out_of_range("  out of\n range...");
    }
    return distance;
}

// Send the measured distance to OLED display.
inline void DisplayDistance(const double distance, const char* unit)
{
    display.setCursor(5, 28);
    display.setTextColor(SSD1306_WHITE);
    display.print(distance);
    display.print(' ');
    display.println(unit);
    display.display();
}

// Send error message to OLED display in a rounded box.
inline void DisplayError(const char* errorMessage)
{
    display.fillRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 8, SSD1306_WHITE);
    display.setCursor(0, 14);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.println(errorMessage);
    display.display();
}

// Arduino setup function.
void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(1);

    DistanceSensorSetup();
    DisplaySetup();
}

// Arduino loop function.
void loop()
{
    display.clearDisplay();
    display.setTextSize(2);
    try
    {
        DisplayDistance(MeasureDistanceCm(), "cm");
    }
    catch (std::out_of_range& exception)
    {
        DisplayError(exception.what());
    }
    delay(500);
}
