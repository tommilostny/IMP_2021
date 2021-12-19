#include <stdexcept>
#include <Adafruit_SSD1306.h>
#include "Adafruit_VL53L0X.h"

// OLED display constants.
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define MOSI 23
#define SCK  18
#define RES  13
#define DC   12
#define CS   5

// VL53L0X sensor constants.
#define FAIL_RANGE_STATUS  4
#define OUT_OF_RANGE_POINT 236

// Global variables for the OLED display and VL53L0X sensor.
auto distanceSensor = Adafruit_VL53L0X();
auto oledDisplay = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI, SCK, DC, RES, CS);

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
    if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC))
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

    // Check if the measurement is valid (not FAIL_RANGE_STATUS and distance in cm is not too large).
    if (measure.RangeStatus == FAIL_RANGE_STATUS || (distance = measure.RangeMilliMeter / 10.0) > OUT_OF_RANGE_POINT)
    {
        throw std::out_of_range("  out of\n range...");
    }
    return distance;
}

// Send the measured distance to OLED display.
inline void DisplayDistance(const double distance, const char* unit)
{
    // Clear and set the display text size, cursor and color.
    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(2);
    oledDisplay.setCursor(5, 28);
    oledDisplay.setTextColor(SSD1306_WHITE);

    // Display the distance.
    oledDisplay.print(distance);
    oledDisplay.print(' ');
    oledDisplay.println(unit);
    oledDisplay.display();
}

// Send error message to OLED display.
inline void DisplayError(const char* errorMessage)
{
    // Clear and set the display text size, cursor and color.
    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(2);
    oledDisplay.setCursor(0, 14);
    oledDisplay.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

    // Display the error message in a rounded box.
    oledDisplay.fillRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 8, SSD1306_WHITE);
    oledDisplay.println(errorMessage);
    oledDisplay.display();
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
    static auto inOutOfRangeMode = false;
    try
    {
        auto distance = MeasureDistanceCm();
        DisplayDistance(distance, "cm");
        inOutOfRangeMode = false;
    }
    catch (std::out_of_range& exception)
    {
        if (!inOutOfRangeMode)
        {
            DisplayError(exception.what());
            inOutOfRangeMode = true;
        }
    }
    delay(500);
}
