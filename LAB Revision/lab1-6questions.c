// Lab 1: RESTful API
// Multiple Choice Questions (MCQ)
// What is the primary purpose of defining endpoints in a RESTful API?
// To map specific URLs to functions that handle HTTP requests

// What HTTP status code indicates a resource was not found on the server?
// 404

// Multi-Select Questions
// Which HTTP methods are commonly used in RESTful APIs for IoT devices?
// GET, POST, PUT

// Which hardware components are typically initialized when setting up a RESTful API on an IoT device?
// Network interface, GPIO pins, sensors

// Short Answer Questions
// How does a RESTful API typically provide sensor data to a client?
// It retrieves sensor data, formats it (e.g., as text or JSON), and sends it in an HTTP response with a status code like 200.

// What is a common way to control an actuator (e.g., buzzer) via a RESTful API?
// Define an endpoint (e.g., /actuator/off) that triggers a function to change the actuator’s state and confirm the action in the response.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lab 2: BLE (Bluetooth Low Energy)
// Multiple Choice Questions (MCQ)
// What BLE characteristic property enables real-time updates to connected devices?
// PROPERTY_NOTIFY

// What action typically triggers a write callback in a BLE server?
// A client writing a value to a characteristic

// Multi-Select Questions
// Which types of data might a BLE server expose through characteristics?
// Sensor readings, device status, control commands

// Which events are commonly monitored in a BLE server application?
// Client connection, client disconnection

// Short Answer Questions
// How can a BLE client control a device’s LED remotely?
// It writes a value (e.g., "ON" or "OFF") to a specific characteristic exposed by the server.

// How does a BLE server send temperature updates to a client?
// It updates a temperature characteristic’s value and uses a notification to inform subscribed clients.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lab 3: LoRa
// Multiple Choice Questions (MCQ)
// What does setting the transmission power in a LoRa module affect?
// The range and power consumption of the transmission

// Why is a checksum used in LoRa communication?
// To ensure the received data is not corrupted

// Multi-Select Questions
// Which conditions might a LoRa receiver check before acknowledging a message?
// Correct destination ID, valid message type, data integrity

// Which steps are required to initialize a LoRa module for communication?
// Initialize the module, set the frequency, configure the power level

// Short Answer Questions
// How does a LoRa transmitter ensure reliable message delivery?
// It retransmits the message if an acknowledgment (ACK) is not received within a timeout period.

// What is the purpose of sending an ACK in a LoRa receiver?
// To confirm to the transmitter that the message was received correctly.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lab 4: MQTT
// Multiple Choice Questions (MCQ)
// What is the main function of an MQTT client loop in an IoT application?
// To keep the connection alive and handle incoming messages

// What is the default port for MQTT communication?
// 1883

// Multi-Select Questions
// Which elements are essential in an MQTT-based IoT system?
// Publisher, subscriber, broker

// What actions are typically performed during MQTT reconnection?
// Generate a client ID, connect to the broker, resubscribe to topics

// Short Answer Questions
// How does an MQTT client control an LED on another device?
// It publishes a command (e.g., "TOGGLE") to a topic that the other device subscribes to, triggering an action.

// What happens when an IoT device publishes a message to an MQTT topic?
// The broker forwards the message to all subscribers of that topic.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lab 5: LoRaWAN
// Multiple Choice Questions (MCQ)
// What is the purpose of queuing a message in LoRaWAN?
// To prepare it for transmission to the network server via a gateway

// Which LoRaWAN class allows devices to sleep most of the time for power efficiency?
// Class A

// Multi-Select Questions
// Which events might a LoRaWAN device monitor during operation?
// Transmission completion, network join success, join failure

// Which parameters are crucial for configuring a LoRaWAN device?
// Session keys, device address, data rate, frequency

// Short Answer Questions
// How does a LoRaWAN device manage periodic data transmissions?
// It schedules transmissions at fixed intervals using a timer or event-driven mechanism.

// Why is an event loop necessary in a LoRaWAN application?
// To handle network events like sending data and receiving downlinks in a timely manner.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lab 6: Wireless Mesh
// Multiple Choice Questions (MCQ)
// What is the purpose of broadcasting in a wireless mesh network?
// To send a message to all nodes within range

// Why might an LED blink in a mesh network application?
// To indicate the absence of connected nodes

// Multi-Select Questions
// Which events are typically monitored in a wireless mesh network?
// Message reception, new node connection, connection changes

// What actions might occur when a new node joins a mesh network?
// Update node list, adjust network status indicators, log connection details

// Short Answer Questions
// How can message priority be implemented in a mesh network?
// By sorting messages based on a priority value and sending higher-priority ones first.

// What happens when a mesh node receives a message from another node?
// It processes the message and may display or forward it, depending on the application.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

