/*
code based on: https://programmaticponderings.com/2020/08/04/getting-started-with-bluetooth-low-energy-ble-and-generic-attribute-profile-gatt-specification-for-iot/
*/

#include <ArduinoBLE.h>

const int UPDATE_FREQUENCY = 2000;      // Update frequency in ms
const float CALIBRATION_FACTOR = -4.0;  // Temperature calibration factor (Celsius)

int previousTemperature = 0;
long previousMillis = 0;  // last time readings were checked, in ms

BLEService environmentService("181A");  // Standard Environmental Sensing service
BLEIntCharacteristic tempCharacteristic("2A6E", BLERead | BLENotify);  // Standard 16-bit Temperature characteristic

void setup()
{
    Serial.begin(9600); // Initialize serial communication
    while (!Serial)
        ; // only when connected to laptop

    randomSeed(analogRead(0));    // initialize pseudo-random number generator
    pinMode(LED_BUILTIN, OUTPUT); // Initialize the built-in LED pin
    if (!BLE.begin())
    { // initialize BLE
        Serial.println("starting BLE failed!");
        while (1)
            ;
    }
\
    BLE.setDeviceName("Team 16 Temp Sensor");
    BLE.setLocalName("Team 16 Temp Sensor");   // Set name for connection
    BLE.setAppearance(0x0543);  // sets appearance as 'Temperature Sensor'
    BLE.setAdvertisedService(environmentService);             // Advertise environment service
    environmentService.addCharacteristic(tempCharacteristic); // Add temperature characteristic

    BLE.addService(environmentService); // Add environment service
    tempCharacteristic.setValue(0);     // Set initial temperature value
    BLE.advertise();                    // Start advertising

    Serial.print("Peripheral device MAC: ");
    Serial.println(BLE.address());
    
    Serial.print("Advertised Service UUID: ");
    Serial.println(environmentService.uuid());
    
    Serial.print("Advertised Service UUID: ");
    Serial.println(environmentService.uuid());
    
    Serial.println("Waiting for connections…");
}

void loop()
{
    BLEDevice central = BLE.central(); // Wait for a BLE central to connect

    // If central is connected to peripheral
    if (central)
    {
        Serial.print("Connected to central MAC: ");
        Serial.println(central.address()); // Central's BT address:

        digitalWrite(LED_BUILTIN, HIGH); // Turn on the LED to indicate the connection

        while (central.connected())
        {
            long currentMillis = millis();
            // After UPDATE_FREQUENCY ms have passed, check temperature & humidity
            if (currentMillis - previousMillis >= UPDATE_FREQUENCY)
            {
                previousMillis = currentMillis;
                updateReadings();
            }
        }

        digitalWrite(LED_BUILTIN, LOW); // When the central disconnects, turn off the LED
        Serial.print("Disconnected from central MAC: ");
        Serial.println(central.address());
    }
}

int getTemperature(float calibration)
{
    // Get calibrated temperature as signed 16-bit int for BLE characteristic
    // return (int) (HTS.readTemperature() * 100) + (int) (calibration * 100);
    return random(30, 36);
}

void updateReadings()
{
    int temperature = getTemperature(CALIBRATION_FACTOR);

    if (temperature != previousTemperature)
    { // If reading has changed
        Serial.print("Temperature: ");
        Serial.println(temperature);
        tempCharacteristic.writeValue(temperature); // Update characteristic
        previousTemperature = temperature;          // Save value
    }
}
