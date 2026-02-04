// Include libraries for WiFi, web server, I2C communication, and OLED display
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the OLED screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
// Create display object with specified width, height, I2C interface, and no reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Define which GPIO pins the LEDs are connected to
int LED_GREEN = 33;
int LED_YELLOW = 25;
int LED_RED = 26;

// WiFi credentials for connecting to network
const char* ssid = "MediaCollege";
const char* password = "vms12345";

// Create web server on port 80 (HTTP)
WebServer server(80);

// Variables for animation: person's X position, green light status, and animation frame
int personX = 0;
bool isGreen = false;
int frame = 0;

// Function to draw a stick figure at position x with animated legs
void drawPerson(int x, int legFrame) {
  // Draw circular head at position (x, 10) with radius 3
  display.fillCircle(x, 10, 3, SSD1306_WHITE);
  // Draw vertical line for body from y=13 to y=22
  display.drawLine(x, 13, x, 22, SSD1306_WHITE);
  // Draw left arm
  display.drawLine(x, 15, x - 3, 18, SSD1306_WHITE);
  // Draw right arm
  display.drawLine(x, 15, x + 3, 18, SSD1306_WHITE);
  // Animate legs based on frame number (0 or 1)
  if (legFrame == 0) {
    // Frame 0: legs in neutral position
    display.drawLine(x, 22, x - 2, 28, SSD1306_WHITE);
    display.drawLine(x, 22, x + 2, 28, SSD1306_WHITE);
  } else {
    // Frame 1: legs in walking position
    display.drawLine(x, 22, x - 3, 28, SSD1306_WHITE);
    display.drawLine(x, 22, x + 1, 28, SSD1306_WHITE);
  }
}

// Function to handle HTTP requests for controlling LEDs
void handleLED() {
  // Check if URL has both 'led' and 'state' parameters
  if (server.hasArg("led") && server.hasArg("state")) {
    // Get the LED name from URL parameter
    String led = server.arg("led");
    // Convert state parameter to boolean (true if "on", false otherwise)
    bool state = server.arg("state") == "on";
    
    // Handle green LED
    if (led == "green") {
      // Turn LED on or off based on state
      digitalWrite(LED_GREEN, state ? HIGH : LOW);
      // Update animation flag
      isGreen = state;
      // Reset person position when light turns off
      if (!state) personX = 0;
    }
    // Handle yellow LED
    else if (led == "yellow") digitalWrite(LED_YELLOW, state ? HIGH : LOW);
    // Handle red LED
    else if (led == "red") digitalWrite(LED_RED, state ? HIGH : LOW);
    
    // Send success response as JSON
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    // Send error response if parameters are missing
    server.send(400, "application/json", "{\"status\":\"error\"}");
  }
}

// Setup function runs once when ESP32 starts
void setup() {
  // Start serial communication at 115200 baud rate for debugging
  Serial.begin(115200);
  // Initialize I2C with SDA on pin 21 and SCL on pin 22
  Wire.begin(21, 22);

  // Configure LED pins as outputs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Initialize OLED display at I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // Print error if display not found
    Serial.println("OLED not found");
    // Stop program if display fails
    while (true);
  }
  
  // Clear display buffer and show blank screen
  display.clearDisplay();
  display.display();

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  // Wait until connected, printing dots
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print IP address when connected
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  // Register URL route "/led" to handleLED function for GET requests
  server.on("/led", HTTP_GET, handleLED);
  // Enable Cross-Origin Resource Sharing for React app
  server.enableCORS(true);
  // Start web server
  server.begin();
}

// Loop function runs repeatedly
void loop() {
  // Process incoming HTTP requests
  server.handleClient();
  
  // Clear display buffer for new frame
  display.clearDisplay();
  
  // Only animate when green light is on
  if (isGreen) {
    // Draw person at current X position with current leg frame
    drawPerson(personX, frame);
    // Move person 2 pixels to the right
    personX += 2;
    // Alternate between leg frames 0 and 1 for walking animation
    frame = (frame + 1) % 2;
    
    // Reset person to left side when they walk off right side
    if (personX > SCREEN_WIDTH + 10) {
      personX = -10;
    }
  }
  
  // Send buffer to display to show changes
  display.display();
  // Wait 100ms before next frame (controls animation speed)
  delay(100);
}