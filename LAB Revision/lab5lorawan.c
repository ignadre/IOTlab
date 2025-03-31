//LAB 5 LORAWAN

#include <M5StickCPlus.h>   // Library for M5StickCPlus hardware
#include <lmic.h>           // LoRaWAN LMIC library (install via Arduino Library Manager)
#include <hal/hal.h>        // Hardware abstraction layer for LMIC
#include <SPI.h>            // SPI library for LoRa module communication

// LoRaWAN credentials (replace with your TTN keys)
static const u1_t NWKSKEY[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 }; // Network Session Key
static const u1_t APPSKEY[16] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20 }; // Application Session Key
static const u4_t DEVADDR = 0x26011A2B; // Device Address (ABP method)

// Pin mapping for LoRa module (e.g., SX1276 on M5StickCPlus hat)
#define LORA_SCK  5    // SPI Clock
#define LORA_MISO 19   // SPI MISO
#define LORA_MOSI 23   // SPI MOSI
#define LORA_SS   18   // SPI Chip Select
#define LORA_RST  14   // Reset pin
#define LORA_DIO0 26   // DIO0 (interrupt pin)

// LMIC pin mapping
const lmic_pinmap lmic_pins = {
    .nss = LORA_SS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LORA_RST,
    .dio = {LORA_DIO0, LMIC_UNUSED_PIN, LMIC_UNUSED_PIN},
};

// Data to send
static uint8_t mydata[4]; // Buffer for sending data (e.g., temperature)
static osjob_t sendjob;   // Job for scheduling transmissions

// Function to display messages on LCD
void displayMessage(const char* msg) {
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.println(msg);
}

void os_getArtEui(u1_t* buf) { } // Not needed for ABP
void os_getDevEui(u1_t* buf) { } // Not needed for ABP
void os_getDevKey(u1_t* buf) { } // Not needed for ABP

// Callback for LoRaWAN events
void onEvent(ev_t ev) {
    switch (ev) {
        case EV_TXCOMPLETE:
            Serial.println("Transmission completed");
            displayMessage("Tx Complete");
            break;
        case EV_JOINED:
            Serial.println("Joined network");
            displayMessage("Joined!");
            break;
        case EV_JOIN_FAILED:
            Serial.println("Join failed");
            displayMessage("Join Failed");
            break;
        default:
            break;
    }

    // Sample Q1: What does the EV_TXCOMPLETE event signify in LoRaWAN?
    // Sample A1: It indicates that the device has successfully completed sending a message to the gateway.
}

void do_send(osjob_t* j) {
    // Check if no transmission is in progress
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("Transmission pending");
        return;
    }

    // Prepare data (e.g., dummy temperature value)
    float temp = 25.5; // Replace with real sensor data if available
    mydata[0] = (uint8_t)temp;      // Integer part
    mydata[1] = (uint8_t)(temp * 10) % 10; // Decimal part
    mydata[2] = 0xAA; // Dummy byte
    mydata[3] = 0xBB; // Dummy byte

    // Send data
    LMIC_setTxData2(1, mydata, sizeof(mydata), 0); // Port 1, unconfirmed message
    Serial.println("Sending data...");
    displayMessage("Sending...");

    // Sample Q2: What is the purpose of LMIC_setTxData2()?
    // Sample A2: It prepares and queues a data message for transmission over LoRaWAN, specifying port, data, length, and confirmation type.
}

void setup() {
    M5.begin();            // Initialize M5StickCPlus
    M5.Lcd.setRotation(3); // Rotate LCD to landscape
    Serial.begin(115200);  // Start serial communication

    displayMessage("LoRaWAN Init");

    // Initialize SPI for LoRa module
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

    // Initialize LMIC
    os_init();
    LMIC_reset();

    // Set static session parameters (ABP method)
    LMIC_setSession(0x1, DEVADDR, NWKSKEY, APPSKEY);
    LMIC_setLinkCheckMode(0); // Disable link check
    LMIC.dn2Dr = DR_SF9;      // Set data rate to SF9 (adjust as needed)
    LMIC_setDrTxpow(DR_SF9, 14); // Set TX power to 14 dBm

    // Set frequency (e.g., 915 MHz band for US, adjust for your region)
    LMIC_setupChannel(0, 915000000, DR_RANGE_MAP(DR_SF12, DR_SF7), 0);

    // Schedule first transmission
    os_setCallback(&sendjob, do_send);

    // Sample Q3: What is the difference between ABP and OTAA in LoRaWAN?
    // Sample A3: ABP (Activation By Personalization) uses pre-set keys and address, while OTAA (Over-The-Air Activation) dynamically negotiates keys during a join procedure.
}

void loop() {
    os_runloop_once(); // Run LMIC event loop
    M5.update();       // Update M5StickCPlus state

    // Schedule next transmission every 60 seconds
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 60000) {
        os_setCallback(&sendjob, do_send);
        lastSend = millis();
    }

    // Sample Q4: Why is os_runloop_once() called in loop()?
    // Sample A4: It processes LoRaWAN events and tasks (e.g., sending, receiving) managed by the LMIC library.
}

// Q5: What is the role of the gateway in a LoRaWAN network?
// A5: The gateway receives LoRa signals from end devices, forwards them to the network server via the internet, and relays downlink messages back to devices.

// Q6: Why does LoRaWAN use spreading factors (e.g., SF9)?
// A6: Spreading factors control the trade-off between range and data rate. Higher SF (e.g., SF12) increases range but reduces data rate, while lower SF (e.g., SF7) does the opposite.

// Q7: What is the purpose of the MIC (Message Integrity Code) in LoRaWAN?
// A7: The MIC ensures message integrity and authenticity, calculated using the NWKSKEY to verify the message hasn’t been tampered with.

// Q8: How does LoRaWAN ensure low power consumption?
// A8: It uses a Class A mode where devices sleep most of the time, waking only to transmit and briefly listen for downlinks, minimizing energy use.

// Q9: What is the significance of the DR_SF9 setting in the code?
// A9: It sets the data rate to Spreading Factor 9, balancing range and speed for transmission, suitable for moderate distances.

// Q10: How could you modify the code to send real sensor data instead of a dummy value?
// A10: Replace the dummy temp value in do_send() with a sensor reading, e.g., float temp = M5.Axp.GetTempData() / 100.0; for the M5StickCPlus temperature sensor.