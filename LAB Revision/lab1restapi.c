// LAB 1 RESTful API
#include <WiFi.h>         // Library for WiFi functionality
#include <WebServer.h>    // Library to create a web server
#include <M5StickCPlus.h> // Library for M5StickCPlus hardware

/* Put your SSID & Password */
const char* ssid = "kukubird";     // WiFi network name (SSID)
const char* password = "kukubird"; // WiFi password

// Sensor values (global variables to store IMU and temperature data)
float pitch, roll, yaw, temperature;

// Create a WebServer object listening on port 80 (HTTP default port)
WebServer server(80);

// Pin definitions for LED and buzzer
#define M5_LED 10    // GPIO pin for the LED (Note: Confirm this matches your M5StickCPlus hardware)
#define BUZZER_PIN 9 // GPIO pin for the buzzer (Note: Confirm this matches your hardware)

// Sample Q1: What is the purpose of the #define directive here?
// Sample A1: It assigns a pin number to a readable name (e.g., M5_LED) for easier reference in the code.

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud rate for debugging

  // Initialize M5StickCPlus hardware (display, buttons, etc.)
  M5.begin();

  // Initialize the IMU (Inertial Measurement Unit) for gyro/accel data
  if (M5.IMU.Init() != 0)
    Serial.println("IMU initialization failed!"); // Print error if IMU fails to start

  // Configure the LCD screen
  M5.Lcd.setRotation(3);     // Rotate display 270 degrees (landscape mode)
  M5.Lcd.fillScreen(BLACK);  // Clear screen with black background
  M5.Lcd.setCursor(0, 0, 2); // Set text cursor at top-left, font size 2
  M5.Lcd.printf("RESTful API", 0); // Display "RESTful API" on screen

  // Configure pins as outputs
  pinMode(M5_LED, OUTPUT);     // Set LED pin as output
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output

  // Turn off LED and buzzer initially (assuming LOW is off; confirm with hardware)
  digitalWrite(M5_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect to WiFi network
  WiFi.begin(ssid, password);      // Start WiFi connection with SSID and password
  WiFi.setHostname("group01-stick"); // Set device hostname for network identification

  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) { // Wait until connected
    delay(500); // Delay 500ms between checks
    Serial.print("."); // Print dots to show progress
  }

  // Display IP address on LCD once connected
  M5.Lcd.setCursor(0, 20, 2); // Move cursor to next line
  M5.Lcd.print("IP: ");
  M5.Lcd.println(WiFi.localIP()); // Show assigned IP address

  // Configure RESTful API endpoints (routes and their handler functions)
  server.on("/", handle_Root);          // Root endpoint
  server.on("/gyro/", handle_Gyro);     // Gyroscope data endpoint
  server.on("/accel/", handle_Accel);   // Accelerometer data endpoint
  server.on("/temp/", handle_Temp);     // Temperature data endpoint
  server.on("/led/0", handle_LED_Off);  // Turn LED off
  server.on("/led/1", handle_LED_On);   // Turn LED on
  server.on("/buzzer/1", handle_Buzzer_On); // Turn buzzer on
  server.on("/buzzer/0", handle_Buzzer_Off); // Turn buzzer off
  server.onNotFound(handle_NotFound);   // Handle invalid URLs

  server.begin(); // Start the web server
  Serial.println("HTTP server started");

  // Sample Q2: What does server.on() do?
  // Sample A2: It maps a specific URL (e.g., "/gyro/") to a handler function (e.g., handle_Gyro) that runs when the URL is accessed.
}

// Function to handle root endpoint ("/")
void handle_Root() {
  server.send(200, "text/plain", "Welcome to M5StickCPlus RESTful API"); // Send HTTP 200 OK response with welcome message
}

// Function to handle gyroscope data endpoint ("/gyro/")
void handle_Gyro() {
  M5.IMU.getGyroData(&pitch, &roll, &yaw); // Fetch gyroscope data into variables
  String message = "Gyroscope Data:\nPitch: " + String(pitch) + "\nRoll: " + String(roll) + "\nYaw: " + String(yaw);
  server.send(200, "text/plain", message); // Send data as plain text response

  // Sample Q3: What is the purpose of the & symbol in getGyroData(&pitch, &roll, &yaw)?
  // Sample A3: It passes the variables by reference, allowing the function to modify their values directly.
}

// Function to handle accelerometer data endpoint ("/accel/")
void handle_Accel() {
  M5.IMU.getAccelData(&pitch, &roll, &yaw); // Fetch accelerometer data (assuming this function exists)
  String message = "Accelerometer Data:\nPitch: " + String(pitch) + "\nRoll: " + String(roll) + "\nYaw: " + String(yaw);
  server.send(200, "text/plain", message); // Send data as plain text response

  // Note: Verify if pitch, roll, yaw are correct for accel data or if different variables are needed.
}

// Function to handle temperature data endpoint ("/temp/")
void handle_Temp() {
  M5.IMU.getTempData(&temperature); // Fetch temperature data
  String message = "Temperature: " + String(temperature) + " °C";
  server.send(200, "text/plain", message); // Send temperature as plain text

  // Sample Q4: What does server.send(200, "text/plain", message) do?
  // Sample A4: It sends an HTTP response with status code 200 (OK), content type "text/plain", and the message content.
}

void handle_LED_Off() {
  digitalWrite(M5_LED, HIGH); // Turn LED off (assuming HIGH is off; confirm with hardware)
  server.send(200, "text/plain", "LED turned off"); // Confirm action
}

void handle_LED_On() {
  digitalWrite(M5_LED, LOW); // Turn LED on (assuming LOW is on; confirm with hardware)
  server.send(200, "text/plain", "LED turned on"); // Confirm action
}

void handle_Buzzer_Off() {
  digitalWrite(BUZZER_PIN, LOW); // Turn buzzer off
  noTone(BUZZER_PIN); // Stop any tone being played
  server.send(200, "text/plain", "Buzzer turned off"); // Confirm action
}

void handle_Buzzer_On() {
  digitalWrite(BUZZER_PIN, HIGH); // Turn buzzer on
  tone(BUZZER_PIN, 1000); // Play a 1000 Hz tone
  server.send(200, "text/plain", "Buzzer turned on"); // Confirm action

  // Sample Q5: What is the difference between tone() and noTone()?
  // Sample A5: tone() generates a square wave of a specified frequency on a pin, while noTone() stops it.
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found"); // Send 404 error for invalid URLs
}

void loop() {
  server.handleClient(); // Continuously handle incoming HTTP requests
  // Sample Q6: Why is server.handleClient() placed in loop()?
  // Sample A6: It ensures the server keeps listening and responding to client requests in real-time.
}