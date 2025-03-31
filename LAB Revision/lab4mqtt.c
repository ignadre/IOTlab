//LAB 4 MQTT

#include <WiFi.h>          // Library for WiFi functionality
#include <PubSubClient.h>  // Library for MQTT client functionality
#include <M5StickCPlus.h>  // Library for M5StickCPlus hardware

WiFiClient espClient;      // WiFi client object for network communication
PubSubClient client(espClient); // MQTT client object using the WiFi client

// Network and MQTT credentials
const char* ssid = "kukubird";         // WiFi SSID
const char* password = "kukubird";     // WiFi password
const char* mqtt_server = "172.20.10.14"; // MQTT broker IP address (update to your broker’s IP)

#define LED_PIN 10         // GPIO pin for LED (confirm with M5StickCPlus hardware)
#define BUTTON_PIN M5.BtnA // Built-in Button A on M5StickCPlus (not a pin number, but an object)

// Function declarations
void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();

void setup() {
    M5.begin();            // Initialize M5StickCPlus (LCD, buttons, etc.)
    M5.Lcd.setRotation(3); // Rotate LCD 270 degrees (landscape mode)

    pinMode(LED_PIN, OUTPUT);      // Set LED pin as output
    digitalWrite(LED_PIN, LOW);    // Ensure LED is off initially (assuming LOW = off)

    setupWifi();                   // Connect to WiFi
    client.setServer(mqtt_server, 1883); // Set MQTT broker address and port (1883 is default)
    client.setCallback(callback);  // Set callback function for incoming messages

    // Sample Q1: What is the purpose of client.setServer()?
    // Sample A1: It configures the MQTT client to connect to the specified broker IP and port.
}

void loop() {
    M5.update();  // Update M5StickCPlus button states

    if (!client.connected()) { // Check if MQTT connection is lost
        reConnect();           // Attempt to reconnect
    }
    client.loop();             // Process MQTT events (e.g., incoming messages)

    if (M5.BtnA.wasPressed()) { // Check if Button A was pressed
        M5.Lcd.println("Button Pressed!");
        client.publish("nodeA_to_nodeB_led", "TOGGLE"); // Publish "TOGGLE" message to topic
        delay(500);  // Debounce delay to prevent multiple triggers

        // Sample Q2: Why is delay(500) used after publishing?
        // Sample A2: It acts as a simple debounce to avoid rapid, unintended button presses.
    }
}

void setupWifi() {
    delay(10);              // Brief delay for stability
    M5.Lcd.printf("Connecting to %s", ssid); // Display connection attempt
    WiFi.mode(WIFI_STA);    // Set WiFi mode to Station (client)
    WiFi.begin(ssid, password); // Start WiFi connection

    while (WiFi.status() != WL_CONNECTED) { // Wait until connected
        delay(500);
        M5.Lcd.print("."); // Show progress dots
    }
    M5.Lcd.println("\nWiFi Connected!"); // Confirm connection

    // Sample Q3: What does WiFi.mode(WIFI_STA) do?
    // Sample A3: It configures the device as a WiFi client (Station) to connect to an access point.
}

void callback(char* topic, byte* payload, unsigned int length) {
    M5.Lcd.print("Message arrived: [");
    M5.Lcd.print(topic);     // Display the topic
    M5.Lcd.print("] ");

    String receivedMessage;   // Construct message from payload
    for (int i = 0; i < length; i++) {
        receivedMessage += (char)payload[i]; // Convert byte array to string
    }
    M5.Lcd.println(receivedMessage); // Display the message

    // Check if message is for the "led" topic
    if (String(topic) == "nodeB_to_nodeA_led") { // Corrected topic name for consistency
        if (receivedMessage == "TOGGLE") {
            M5.Lcd.println("Toggling LED!");
            digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle LED state
        }
    }

    // Sample Q4: What does the callback function do in MQTT?
    // Sample A4: It’s called when a message arrives on a subscribed topic, allowing custom handling (e.g., toggling LED).
}

void reConnect() { // Fixed syntax (removed erroneous =)
    while (!client.connected()) { // Keep trying until connected
        M5.Lcd.print("Attempting MQTT connection...");
        String clientId = "NodeA-"; // Generate unique client ID
        clientId += String(random(0xffff), HEX); // Add random hex suffix

        if (client.connect(clientId.c_str())) { // Attempt to connect to broker
            M5.Lcd.println("Connected!");
            client.subscribe("nodeB_to_nodeA_led"); // Subscribe to topic for LED control
        } else {
            M5.Lcd.print("Failed, rc=");
            M5.Lcd.print(client.state()); // Display error code
            M5.Lcd.println(" retrying in 5s...");
            delay(5000); // Wait 5 seconds before retrying
        }
    }

    // Sample Q5: Why is a random client ID used?
    // Sample A5: To avoid conflicts if multiple devices connect with the same ID, ensuring unique identification.
}

// Q6: What is the role of the MQTT broker in this setup?
// A6: The broker acts as a central server that receives messages from publishers and forwards them to subscribers based on topics.

// Q7: What does client.loop() do in the loop() function?
// A7: It maintains the MQTT connection, processes incoming messages, and handles keep-alive signals with the broker.

// Q8: How could you modify the code to control the LED with "ON" and "OFF" instead of "TOGGLE"?
// A8: In callback, check for "ON" (set digitalWrite(LED_PIN, HIGH)) or "OFF" (set digitalWrite(LED_PIN, LOW))) instead of toggling.