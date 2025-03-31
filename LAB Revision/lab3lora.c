//LAB 3 LORA RECEIVER

#include <SPI.h>              // Library for SPI communication
#include <RH_RF95.h>          // Library for RF95 LoRa module
#include <Wire.h>             // Library for I2C communication
#include <Adafruit_GFX.h>     // Graphics library for OLED
#include <Adafruit_SSD1306.h> // Library for SSD1306 OLED display

#define SCREEN_WIDTH 128      // OLED display width
#define SCREEN_HEIGHT 16      // OLED display height
#define OLED_RESET -1         // Reset pin not used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // OLED object

#define RFM95_CS 10           // Chip Select pin for LoRa
#define RFM95_RST 9           // Reset pin for LoRa
#define RFM95_INT 2           // Interrupt pin for LoRa
#define RF95_FREQ 900.0       // LoRa frequency in MHz

RH_RF95 rf95(RFM95_CS, RFM95_INT); // LoRa radio object

const uint8_t myID = 1;       // Unique ID of this node (Transmitter)
const uint8_t targetID = 2;   // Intended receiver's ID

// Message Types
const uint8_t MSG_TYPE_DATA = 0x01; // Data message type
const uint8_t MSG_TYPE_ACK  = 0x02; // Acknowledgment message type

// Function to calculate checksum
uint8_t calculateChecksum(uint8_t *data, uint8_t length) {
  uint8_t sum = 0;
  for (uint8_t i = 0; i < length - 1; i++) {
    sum += data[i];
  }
  return sum;

  // Sample Q4: What is the purpose of the checksum in LoRa communication?
  // Sample A4: It verifies data integrity, detecting corruption during transmission.
}

void setup() {
  Serial.begin(9600); // Start serial communication
  delay(200);         // Brief delay
  
  Wire.begin();       // Explicitly initialize I2C

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1); // Halt if OLED fails
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK); // Clear screen
  displayMessage("Initializing...");

  // Reset LoRa module
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Initialize LoRa module
  if (!rf95.init()) {
    Serial.println("LoRa init failed");
    displayMessage("Setup Failed");
    while (1);
  }

  // Set LoRa frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    displayMessage("Setup Failed");
    while (1);
  }

  rf95.setTxPower(13, false); // Set transmit power to 13 dBm
  displayMessage("Setup Successful");
  delay(2000);
}

void loop() {
  sendMessageWithRetransmission(targetID, MSG_TYPE_DATA, "Hello from Transmitter!");
  delay(5000);  // Wait 5 seconds before sending next message
}

void sendMessageWithRetransmission(uint8_t receiverID, uint8_t msgType, const char* data) {
  bool ackReceived = false;
  uint8_t attempts = 0;
  const uint8_t maxAttempts = 3; // Maximum retransmission attempts

  // Prepare message (header + data + checksum)
  uint8_t dataLen = strlen(data);
  uint8_t message[dataLen + 4]; // [sender, receiver, type, data..., checksum]
  message[0] = myID;
  message[1] = receiverID;
  message[2] = msgType;
  memcpy(&message[3], data, dataLen); // Copy data into message
  message[dataLen + 3] = calculateChecksum(message, dataLen + 4); // Add checksum

  while (!ackReceived && attempts < maxAttempts) {
    displayMessage("Sending Message");
    delay(1000);

    // Send the message
    rf95.send(message, dataLen + 4);
    rf95.waitPacketSent();

    // Wait for ACK
    displayMessage("Waiting for Reply");
    unsigned long startTime = millis();
    while (millis() - startTime < 3000) {  // 3-second timeout
      if (rf95.available()) {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (rf95.recv(buf, &len)) {
          // Validate ACK
          if (len >= 4 && buf[0] == receiverID && buf[1] == myID && buf[2] == MSG_TYPE_ACK) {
            uint8_t checksumReceived = buf[len - 1];
            if (checksumReceived == calculateChecksum(buf, len)) {
              Serial.println("ACK Received");
              ackReceived = true;
              displayMessage("Message Received");
              break;
            }
          }
        }
      }
    }
    if (!ackReceived) {
      attempts++;
      Serial.println("No ACK Received, retrying...");
      displayMessage("No ACK, Retry");
      delay(1000); // Brief pause before retrying
    }
  }
  if (!ackReceived) {
    Serial.println("Message failed after retries.");
    displayMessage("Send Failed");
  }

  // Sample Q5: Why does the transmitter retry if no ACK is received?
  // Sample A5: To ensure reliable delivery in case of packet loss, common in wireless communication.
}

void displayMessage(const char* msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//LAB 3 LORA TRANSMITTER

#include <SPI.h>              // Library for SPI communication
#include <RH_RF95.h>          // Library for RF95 LoRa module
#include <Wire.h>             // Library for I2C communication
#include <Adafruit_GFX.h>     // Graphics library for OLED
#include <Adafruit_SSD1306.h> // Library for SSD1306 OLED display

#define SCREEN_WIDTH 128      // OLED display width
#define SCREEN_HEIGHT 16      // OLED display height
#define OLED_RESET -1         // Reset pin not used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // OLED object

#define RFM95_CS 10           // Chip Select pin for LoRa
#define RFM95_RST 9           // Reset pin for LoRa
#define RFM95_INT 2           // Interrupt pin for LoRa
#define RF95_FREQ 900.0       // LoRa frequency in MHz

RH_RF95 rf95(RFM95_CS, RFM95_INT); // LoRa radio object

const uint8_t myID = 1;       // Unique ID of this node (Transmitter)
const uint8_t targetID = 2;   // Intended receiver's ID

// Message Types
const uint8_t MSG_TYPE_DATA = 0x01; // Data message type
const uint8_t MSG_TYPE_ACK  = 0x02; // Acknowledgment message type

// Function to calculate checksum
uint8_t calculateChecksum(uint8_t *data, uint8_t length) {
  uint8_t sum = 0;
  for (uint8_t i = 0; i < length - 1; i++) {
    sum += data[i];
  }
  return sum;

  // Sample Q4: What is the purpose of the checksum in LoRa communication?
  // Sample A4: It verifies data integrity, detecting corruption during transmission.
}

void setup() {
  Serial.begin(9600); // Start serial communication
  delay(200);         // Brief delay
  
  Wire.begin();       // Explicitly initialize I2C

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1); // Halt if OLED fails
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK); // Clear screen
  displayMessage("Initializing...");

  // Reset LoRa module
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Initialize LoRa module
  if (!rf95.init()) {
    Serial.println("LoRa init failed");
    displayMessage("Setup Failed");
    while (1);
  }

  // Set LoRa frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    displayMessage("Setup Failed");
    while (1);
  }

  rf95.setTxPower(13, false); // Set transmit power to 13 dBm
  displayMessage("Setup Successful");
  delay(2000);
}

void loop() {
  sendMessageWithRetransmission(targetID, MSG_TYPE_DATA, "Hello from Transmitter!");
  delay(5000);  // Wait 5 seconds before sending next message
}

void sendMessageWithRetransmission(uint8_t receiverID, uint8_t msgType, const char* data) {
  bool ackReceived = false;
  uint8_t attempts = 0;
  const uint8_t maxAttempts = 3; // Maximum retransmission attempts

  // Prepare message (header + data + checksum)
  uint8_t dataLen = strlen(data);
  uint8_t message[dataLen + 4]; // [sender, receiver, type, data..., checksum]
  message[0] = myID;
  message[1] = receiverID;
  message[2] = msgType;
  memcpy(&message[3], data, dataLen); // Copy data into message
  message[dataLen + 3] = calculateChecksum(message, dataLen + 4); // Add checksum

  while (!ackReceived && attempts < maxAttempts) {
    displayMessage("Sending Message");
    delay(1000);

    // Send the message
    rf95.send(message, dataLen + 4);
    rf95.waitPacketSent();

    // Wait for ACK
    displayMessage("Waiting for Reply");
    unsigned long startTime = millis();
    while (millis() - startTime < 3000) {  // 3-second timeout
      if (rf95.available()) {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (rf95.recv(buf, &len)) {
          // Validate ACK
          if (len >= 4 && buf[0] == receiverID && buf[1] == myID && buf[2] == MSG_TYPE_ACK) {
            uint8_t checksumReceived = buf[len - 1];
            if (checksumReceived == calculateChecksum(buf, len)) {
              Serial.println("ACK Received");
              ackReceived = true;
              displayMessage("Message Received");
              break;
            }
          }
        }
      }
    }
    if (!ackReceived) {
      attempts++;
      Serial.println("No ACK Received, retrying...");
      displayMessage("No ACK, Retry");
      delay(1000); // Brief pause before retrying
    }
  }
  if (!ackReceived) {
    Serial.println("Message failed after retries.");
    displayMessage("Send Failed");
  }

  // Sample Q5: Why does the transmitter retry if no ACK is received?
  // Sample A5: To ensure reliable delivery in case of packet loss, common in wireless communication.
}

void displayMessage(const char* msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}

// Q6: What is the advantage of LoRa over WiFi for this application?
// A6: LoRa offers longer range (up to kilometers) and lower power consumption, ideal for IoT, while WiFi is shorter-range and power-hungry.

// Q7: What does rf95.waitPacketSent() do?
// A7: It blocks execution until the LoRa module finishes transmitting the packet, ensuring the send is complete.