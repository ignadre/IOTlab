//LAB 6 WIRELESS MESH

#include <painlessMesh.h>   // Library for painlessMesh wireless mesh networking
#include <M5StickCPlus.h>   // Library for M5StickCPlus hardware

// Define LED pin (use built-in if available, otherwise G10)
#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else
#define LED 10  // GPIO 10 for M5StickCPlus (confirm with your hardware)
#endif

#define BLINK_PERIOD    3000 // Milliseconds until LED blink cycle repeats
#define BLINK_DURATION  100  // Milliseconds LED is on for

// Mesh network configuration
#define MESH_SSID       "kukubird"       // Mesh network SSID
#define MESH_PASSWORD   "meshpotatoes"   // Mesh network password
#define MESH_PORT       5555             // Port for mesh communication

// Enum to represent message priority levels
enum MessagePriority {
  PRIORITY_HIGH = 0,   // Highest priority (0 for sorting purposes)
  PRIORITY_MEDIUM,     // Medium priority
  PRIORITY_LOW         // Lowest priority
};

// Function prototypes for callbacks and tasks
void sendMessage();                          // Send a message
void receivedCallback(uint32_t from, String &msg); // Handle received messages
void newConnectionCallback(uint32_t nodeId);       // Handle new node connections
void changedConnectionCallback();                  // Handle connection changes
void nodeTimeAdjustedCallback(int32_t offset);     // Handle time synchronization
void delayReceivedCallback(uint32_t from, int32_t delay); // Handle delay measurements

Scheduler     userScheduler;  // Scheduler to manage tasks
painlessMesh  mesh;           // painlessMesh object for network management

bool calc_delay = false;      // Flag for delay calculation (not used in this code)
SimpleList<uint32_t> nodes;   // List to store connected node IDs

// Task to send messages every 1 second (adjustable)
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

// Task to blink LED when no nodes are connected
Task blinkNoNodes;  // LED blinking task
bool onFlag = false; // Flag to control LED state

// Structure to represent a message with priority
struct Message {
  String msg;          // Message content
  MessagePriority priority; // Priority level
  uint32_t nodeId;     // Sender node ID
};

// Queue to hold messages with priorities
SimpleList<Message> messageQueue;

void setup() {
  // Initialize M5StickCPlus and serial communication
  M5.begin();
  M5.Lcd.setRotation(3);       // Rotate LCD to landscape
  M5.Lcd.fillScreen(BLACK);    // Clear screen with black
  M5.Lcd.setTextColor(WHITE);  // Set text color to white
  M5.Lcd.setTextSize(2);       // Set text size

  Serial.begin(115200);        // Start serial for debugging
  pinMode(LED, OUTPUT);        // Set LED pin as output

  // Mesh network setup
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // Enable error and debug messages
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT); // Initialize mesh
  mesh.onReceive(&receivedCallback);          // Set callback for received messages
  mesh.onNewConnection(&newConnectionCallback); // Set callback for new connections
  mesh.onChangedConnections(&changedConnectionCallback); // Set callback for connection changes
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);    // Set callback for time sync
  mesh.onNodeDelayReceived(&delayReceivedCallback);      // Set callback for delay

  // Scheduler setup
  userScheduler.addTask(taskSendMessage); // Add message sending task
  taskSendMessage.enable();               // Enable periodic message sending

  // LED blink task setup (blinks when no nodes are connected)
  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
    onFlag = !onFlag;  // Toggle LED state
    blinkNoNodes.delay(BLINK_DURATION); // Delay for LED on duration

    if (blinkNoNodes.isLastIteration()) { // Adjust timing on last iteration
      blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2); // Update iteration count
      blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000)) / 1000); // Sync timing
    }
  });
  userScheduler.addTask(blinkNoNodes); // Add blink task to scheduler
  blinkNoNodes.enable();               // Enable LED blinking

  // Sample Q1: What is the purpose of the Scheduler in this code?
  // Sample A1: It manages periodic tasks like sending messages and blinking the LED, ensuring they run at specified intervals.
}

void loop() {
  mesh.update();          // Update mesh network state (handle communication)
  digitalWrite(LED, !onFlag); // Control LED (inverted logic: LOW = on, HIGH = off)

  processMessages();      // Process queued messages based on priority

  // Sample Q2: Why is mesh.update() called in loop()?
  // Sample A2: It keeps the mesh network running, handling message routing, node discovery, and callbacks.
}

// Function to send a message at random intervals
void sendMessage() {
  String msg = "Hello from M5StickC "; // Base message
  msg += String(mesh.getNodeId());     // Append unique node ID
  
  // Assign random priority (HIGH, MEDIUM, LOW)
  MessagePriority priority = static_cast<MessagePriority>(random(0, 3));
  Message message = {msg, priority, mesh.getNodeId()}; // Create message object
  
  messageQueue.push_back(message); // Add message to queue
  
  Serial.printf("Sending message: %s with priority %d\n", msg.c_str(), priority);

  // Set random interval for next message (1-5 seconds)
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));

  // Sample Q3: Why is a random interval used for taskSendMessage?
  // Sample A3: To prevent all nodes from sending messages simultaneously, reducing network congestion.
}

// Function to process messages based on priority
void processMessages() {
  // Sort queue by priority (lower number = higher priority)
  messageQueue.sort([](const Message& a, const Message& b) {
    return a.priority < b.priority; // HIGH (0) comes before LOW (2)
  });

  // Send highest priority message if queue is not empty
  if (messageQueue.size() > 0) {
    Message message = messageQueue.front(); // Get highest priority message
    mesh.sendBroadcast(message.msg);        // Broadcast to all nodes
    messageQueue.pop_front();               // Remove sent message
  }

  // Sample Q4: How does sorting by priority improve the mesh network?
  // Sample A4: It ensures critical messages (e.g., HIGH priority) are sent first, improving responsiveness for important data.
}

// Callback for received messages
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u: %s\n", from, msg.c_str()); // Log received message

  // Update LCD with sender and message
  M5.Lcd.fillRect(0, 10, 160, 70, BLACK); // Clear previous message area
  M5.Lcd.setCursor(10, 20);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("From: ");
  M5.Lcd.println(from);

  M5.Lcd.setCursor(10, 50);
  M5.Lcd.setTextSize(1);
  M5.Lcd.print("Msg: ");
  M5.Lcd.println(msg);

  // Sample Q5: What is the benefit of broadcasting messages in a mesh network?
  // Sample A5: It allows all nodes to receive the message, useful for network-wide updates or synchronization.
}

// Callback for new node connections
void newConnectionCallback(uint32_t nodeId) {
  onFlag = false; // Stop LED blinking (nodes are connected)
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2); // Update blink count
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000)) / 1000); // Sync timing

  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  Serial.printf("Connection Details: %s\n", mesh.subConnectionJson(true).c_str());
}

// Callback for changed connections
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  onFlag = false; // Stop LED blinking
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2); // Update blink count
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000)) / 1000); // Sync timing

  nodes = mesh.getNodeList(); // Update node list
  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node); // Print each node ID
    node++;
  }
  Serial.println();

  // Sample Q6: Why does the LED stop blinking when a new connection is made?
  // Sample A6: It indicates the network is active with connected nodes, so no “lonely” signal is needed.
}

// Callback for time adjustment
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);

  // Sample Q7: Why is time synchronization important in a mesh network?
  // Sample A7: It ensures coordinated timing for tasks (e.g., blinking, sending), preventing drift across nodes.
}

// Callback for delay measurement
void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

// Q8: What is the main advantage of a mesh network over a star topology?
// A8: Mesh networks are self-healing and extend range by relaying messages through nodes, unlike star topologies that rely on a central hub.

// Q9: How does painlessMesh handle routing?
// A9: It automatically manages routing by maintaining a list of nodes and forwarding messages through the shortest path, transparent to the user.

// Q10: How could you modify the code to send a message to a specific node instead of broadcasting?
// A10: Replace mesh.sendBroadcast(message.msg) with mesh.sendSingle(targetNodeId, message.msg) where targetNodeId is the destination node’s ID.