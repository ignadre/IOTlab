//LAB 2 BLE SERVER

#include <BLEDevice.h>      // Library for BLE functionality
#include <BLEServer.h>      // Library for BLE server-specific features
#include <M5StickCPlus.h>   // Library for M5StickCPlus hardware

// BLE UUIDs (unique identifiers for service and characteristics)
#define SERVICE_UUID "01234679-0343-4566-89ab-0123456789ab"              // Custom service UUID
#define LED_STATE_CHARACTERISTIC_UUID "01234567-89ab-4567-89ab-0123456789ac" // UUID for LED state
#define TEMPERATURE_CHARACTERISTIC_UUID "01234567-0123-4567-89ab-0123456789cd" // UUID for temperature
#define VOLTAGE_CHARACTERISTIC_UUID "01234567-0123-4567-89ab-0123456789ef" // UUID for voltage

// BLE server objects (pointers to characteristics)
BLECharacteristic* ledStateCharacteristic;      // Characteristic for LED state
BLECharacteristic* temperatureCharacteristic;   // Characteristic for temperature
BLECharacteristic* voltageCharacteristic;       // Characteristic for voltage

// LED state (global variable to track LED on/off)
bool ledState = false;
#define LED_PIN 10  // G10 pin for LED (confirm this matches M5StickCPlus hardware)

// Function to display messages on the M5StickCPlus LCD
void displayMessage(const char* message) {
  M5.Lcd.fillScreen(BLACK);    // Clear screen with black background
  M5.Lcd.setCursor(0, 0, 2);   // Set cursor to top-left, font size 2
  M5.Lcd.printf(message);      // Print the provided message
}

// Callback class for server connection/disconnection events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {  // Called when a client connects
    Serial.println("Client connected!");
    displayMessage("Client Connected!");
  }

  void onDisconnect(BLEServer* pServer) {  // Called when a client disconnects
    Serial.println("Client disconnected!");
    displayMessage("Client Disconnected!");
  }
};

// Callback class for LED state characteristic writes
class LEDStateCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) override {  // Called when client writes to characteristic
    std::string value = pCharacteristic->getValue();  // Get the written value
    if (value == "ON") {
      ledState = true;
      digitalWrite(LED_PIN, LOW);  // LED ON (active low; confirm with hardware)
      Serial.println("LED turned ON by client");
    } else if (value == "OFF") {
      ledState = false;
      digitalWrite(LED_PIN, HIGH); // LED OFF
      Serial.println("LED turned OFF by client");
    } else {
      Serial.println("Invalid LED state received from client");
    }

    // Update display with current LED state
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.printf("LED: %s\n", ledState ? "ON" : "OFF");

    // Sample Q1: What does the onWrite() callback do?
    // Sample A1: It’s triggered when a client writes to the characteristic, here controlling the LED based on "ON" or "OFF".
  }
};

void setupBLE() {
  BLEDevice::init("THANOS");  // Initialize BLE with device name "THANOS"

  // Create BLE server and set callbacks
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService* pService = pServer->createService(SERVICE_UUID);  // Create service with defined UUID

  // Create LED state characteristic with read, write, and notify properties
  ledStateCharacteristic = pService->createCharacteristic(
    LED_STATE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
  );
  ledStateCharacteristic->setCallbacks(new LEDStateCallback());  // Assign write callback

  // Create temperature characteristic with read and notify properties
  temperatureCharacteristic = pService->createCharacteristic(
    TEMPERATURE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  // Create voltage characteristic with read and notify properties
  voltageCharacteristic = pService->createCharacteristic(
    VOLTAGE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  // Set initial values for characteristics
  ledStateCharacteristic->setValue(ledState ? "ON" : "OFF");
  temperatureCharacteristic->setValue("25.0");  // Dummy initial temperature
  voltageCharacteristic->setValue("3.7");       // Dummy initial voltage

  // Start the service and advertising
  pService->start();
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);  // Advertise the service UUID
  pAdvertising->start();

  Serial.println("BLE Server is running...");
  displayMessage("Server Running...");

  // Sample Q2: What is the purpose of PROPERTY_NOTIFY in a characteristic?
  // Sample A2: It allows the server to notify subscribed clients when the characteristic’s value changes.
}

void setup() {
  Serial.begin(115200);  // Start serial communication for debugging
  M5.begin();            // Initialize M5StickCPlus hardware
  M5.Lcd.setRotation(3); // Rotate LCD 270 degrees (landscape)
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.println("BLE Server");

  // Set up LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Start with LED off (HIGH = off, assuming active low)

  setupBLE();  // Initialize BLE server
}

float readTemperature() {
  // Read temperature from onboard sensor (AXP192 chip on M5StickCPlus)
  return M5.Axp.GetTempData() / 100.0;  // Convert to Celsius (check documentation for accuracy)
}

float readVoltage() {
  // Read battery voltage from AXP192 power management chip
  return M5.Axp.GetVBusVoltage() / 1000.0;  // Convert to volts
}

void loop() {
  M5.update();  // Update M5StickCPlus button states

  // Toggle LED state and notify clients when Button A is pressed
  if (M5.BtnA.wasPressed()) {
    ledState = !ledState;  // Toggle LED state
    ledStateCharacteristic->setValue(ledState ? "ON" : "OFF");  // Update characteristic value
    ledStateCharacteristic->notify();  // Notify subscribed clients

    // Control the LED based on the state
    if (ledState) {
      digitalWrite(LED_PIN, LOW);  // LED ON (active low)
    } else {
      digitalWrite(LED_PIN, HIGH); // LED OFF
    }

    // Get real temperature and voltage readings
    float temperature = readTemperature();
    float voltage = readVoltage();

    // Convert to strings with specified precision
    String temperatureStr = String(temperature, 1);  // 1 decimal place
    String voltageStr = String(voltage, 2);          // 2 decimal places

    // Update characteristics and notify clients
    temperatureCharacteristic->setValue(temperatureStr.c_str());
    temperatureCharacteristic->notify();
    voltageCharacteristic->setValue(voltageStr.c_str());
    voltageCharacteristic->notify();

    // Update LCD display
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.printf("LED: %s\nTemp: %.1f C\nVoltage: %.2f V", 
                  ledState ? "ON" : "OFF", temperature, voltage);

    Serial.printf("LED: %s, Temp: %.1f, Voltage: %.2f\n", 
                  ledState ? "ON" : "OFF", temperature, voltage);

    // Sample Q3: Why is notify() called after setValue()?
    // Sample A3: It sends the updated value to subscribed clients, ensuring they receive real-time changes.
  }
}

// Sample Q4: What is the purpose of M5.update() in the loop?
// Sample A4: It checks for button presses and updates the M5StickCPlus state, ensuring responsive interaction.
//
// Sample Q5: Why is the LED state toggled in the loop?
// Sample A5: It allows the user to control the LED by pressing Button A, providing interactive feedback on the device.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//LAB 2 BLE CLIENT

#include <BLEDevice.h>      // Library for BLE functionality
#include <M5StickCPlus.h>   // Library for M5StickCPlus hardware

// BLE UUIDs (must match the server’s UUIDs)
#define SERVICE_UUID "01234679-0343-4566-89ab-0123456789ab"
#define LED_STATE_CHARACTERISTIC_UUID "01234567-89ab-4567-89ab-0123456789ac"
#define TEMPERATURE_CHARACTERISTIC_UUID "01234567-0123-4567-89ab-0123456789cd"
#define VOLTAGE_CHARACTERISTIC_UUID "01234567-0123-4567-89ab-0123456789ef"

// BLE client state variables
bool doConnect = false;   // Flag to initiate connection
bool connected = false;   // Flag to track connection status

BLEAddress* serverAddress;  // Pointer to store server’s address
BLERemoteCharacteristic* ledStateCharacteristic = nullptr;      // Remote LED state characteristic
BLERemoteCharacteristic* temperatureCharacteristic = nullptr;   // Remote temperature characteristic
BLERemoteCharacteristic* voltageCharacteristic = nullptr;       // Remote voltage characteristic

// Function to display messages on the LCD
void displayMessage(const char* message) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf(message);
}

// Function to connect to the BLE server
bool connectToServer(BLEAddress pAddress) {
  BLEClient* pClient = BLEDevice::createClient();  // Create a BLE client
  Serial.println("Connecting to server...");

  if (pClient->connect(pAddress)) {  // Attempt to connect to the server
    Serial.println("Connected to server");
    BLERemoteService* pService = pClient->getService(SERVICE_UUID);  // Get the service

    if (pService != nullptr) {  // Check if service exists
      // Get references to the characteristics
      ledStateCharacteristic = pService->getCharacteristic(LED_STATE_CHARACTERISTIC_UUID);
      temperatureCharacteristic = pService->getCharacteristic(TEMPERATURE_CHARACTERISTIC_UUID);
      voltageCharacteristic = pService->getCharacteristic(VOLTAGE_CHARACTERISTIC_UUID);

      if (ledStateCharacteristic && temperatureCharacteristic && voltageCharacteristic) {
        connected = true;
        Serial.println("Characteristics found.");
        return true;
      }
    }
  }

  Serial.println("Failed to connect or find characteristics.");
  return false;

  // Sample Q4: What does pClient->connect(pAddress) do?
  // Sample A4: It initiates a connection to the BLE server at the specified address.
}

// Callback class for scanning advertised devices
class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {  // Called for each discovered device
    Serial.println("Found advertised device");
    Serial.print("Device Name: ");
    Serial.println(advertisedDevice.getName().c_str());
    
    if (advertisedDevice.getName() == "THANOS") {  // Look for server named "THANOS"
      advertisedDevice.getScan()->stop();  // Stop scanning
      serverAddress = new BLEAddress(advertisedDevice.getAddress());  // Store server address
      doConnect = true;  // Trigger connection
      Serial.println("Found server. Connecting...");
    }
  }
};

void setup() {
  Serial.begin(115200);  // Start serial communication
  M5.begin();            // Initialize M5StickCPlus
  M5.Lcd.setRotation(3); // Rotate LCD 270 degrees
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.println("BLE Client");

  BLEDevice::init("");  // Initialize BLE (no specific name for client)

  // Start scanning for devices
  BLEScan* pScan = BLEDevice::getScan();
  pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());  // Set scan callback
  pScan->setActiveScan(true);  // Use active scanning (requests more data from devices)
  pScan->start(30, false);     // Scan for 30 seconds

  // Sample Q5: What is the difference between active and passive scanning?
  // Sample A5: Active scanning requests additional data (e.g., service UUIDs) from devices, while passive scanning only listens.
}

void loop() {
  M5.update();  // Update button states

  // Attempt connection if server is found
  if (doConnect && !connected) {
    if (connectToServer(*serverAddress)) {
      displayMessage("Connected!");
    } else {
      displayMessage("Connection failed");
    }
    doConnect = false;
  }

  if (connected) {
    if (M5.BtnA.wasPressed()) {  // Button A: Read LED state
      if (ledStateCharacteristic->canRead()) {
        std::string value = ledStateCharacteristic->readValue();  // Read LED state
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 0, 2);
        M5.Lcd.printf("LED State: %s", value.c_str());
        Serial.printf("Read LED State: %s\n", value.c_str());
        delay(2000);  // Display for 2 seconds
      }
    }

    if (M5.BtnB.wasPressed()) {  // Button B: Toggle LED state
      static bool ledState = false;  // Local LED state tracker
      ledState = !ledState;  // Toggle state

      if (ledStateCharacteristic->canWrite()) {  // Check if writable
        std::string value = ledState ? "ON" : "OFF";
        ledStateCharacteristic->writeValue(value);  // Write new state to server
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 0, 2);
        M5.Lcd.printf("Set LED: %s", value.c_str());
        Serial.printf("Set LED State to: %s\n", value.c_str());
      } else {
        Serial.println("LED State characteristic is not writable.");
      }
    }

    // Display temperature and voltage (updated periodically)
    if (temperatureCharacteristic->canRead() && voltageCharacteristic->canRead()) {
      std::string temp = temperatureCharacteristic->readValue();
      std::string volt = voltageCharacteristic->readValue();

      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0, 2);
      M5.Lcd.printf("Temp: %s\n", temp.c_str());
      M5.Lcd.printf("Voltage: %s\n", volt.c_str());

      Serial.printf("Temp: %s, Voltage: %s\n", temp.c_str(), volt.c_str());
    }

    delay(500);  // Slow down loop to avoid overwhelming the system

    // Sample Q6: What does canRead() and canWrite() check?
    // Sample A6: They verify if the characteristic supports reading or writing based on its defined properties.
  }
}