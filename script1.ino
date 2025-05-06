#include <Wire.h>
#include <BH1750.h>
#include <ArduinoBLE.h>

// Define BLE service and characteristic UUIDs
BLEService lightSensorService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic lightLevelCharacteristic(
  "19B10001-E8F2-537E-4F6C-D104768A1214", 
  BLERead | BLENotify
);

// Create an instance of the BH1750 light sensor
BH1750 lightSensor;

void setup() {
  Serial.begin(9600);

  // Initialize I2C and light sensor
  Wire.begin();
  if (!lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Error: BH1750 sensor initialization failed.");
    while (true);
  }
  Serial.println("BH1750 light sensor successfully initialized.");

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Error: Unable to start BLE module.");
    while (true);
  }
  Serial.println("BLE module initialized and ready.");

  // Configure BLE service and characteristic
  BLE.setLocalName("LightSensor");
  BLE.setAdvertisedService(lightSensorService);
  lightSensorService.addCharacteristic(lightLevelCharacteristic);
  BLE.addService(lightSensorService);
  BLE.advertise();

  Serial.println("BLE service is now broadcasting. Waiting for central device...");
}

void loop() {
  // Wait for a BLE central device to connect
  BLEDevice centralDevice = BLE.central();

  if (centralDevice) {
    Serial.println("Connection established with central device.");

    while (centralDevice.connected()) {
      // Read current lux value
      uint16_t lux = lightSensor.readLightLevel();

      if (lux == 0) {
        Serial.println("Warning: Light sensor returned zero or invalid reading.");
      } else {
        Serial.print("Current light intensity: ");
        Serial.print(lux);
        Serial.println(" lux");

        // Send light level data over BLE
        lightLevelCharacteristic.writeValue(lux);
      }

      delay(1000);
    }

    Serial.println("Central device has disconnected.");
  } else {
    Serial.println("No device connected. Scanning...");
    delay(1000);
  }
}

