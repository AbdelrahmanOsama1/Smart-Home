
#include <WiFi.h>

#define TX_PIN 17
#define NONE -1
#define BAUD_RATE 9600

// Replace with your network credentials
const char* ssid = "Abdelr@hm@n";         // Your Wi-Fi SSID
const char* password = "O11Z7A10S1M25$";  // Your Wi-Fi password

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
String lcdMessage = "";  // String to store the numeric message for LCD

// Auxiliar variables to store the current output state
String relayState = "off";
String motorState = "off";
String rgbState = "none";

// Current time for timeout
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  // Start Serial for UART communication to ATmega32
  Serial.begin(BAUD_RATE, SERIAL_8N1, NONE, TX_PIN);  // Baud rate, 8 data bits, No parity, 1 stop bit, no RX, TX

  // Connect to Wi-Fi
  /*Serial.print("Connecting to ");
  Serial.println(ssid);*/
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {  // wait until already connected to WIFI
    delay(500);
    //Serial.print(".");
  }

  /*Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());*/

  // Start the web server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("New Client.");
    String currentLine = "";  // To hold incoming data from client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          // HTTP request ends after two newlines
          if (currentLine.length() == 0) {
            // Send a response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Handle requests for Relay, Motor, and RGB
            if (header.indexOf("GET /relay/on") >= 0) {
              relayState = "on";
              Serial.print("@relayON-1#");  // Send to ATmega32

            } else if (header.indexOf("GET /relay/off") >= 0) {
              relayState = "off";
              Serial.print("@relayOFF-1#");  // Send to ATmega32

            } else if (header.indexOf("GET /motor/on") >= 0) {
              motorState = "on";
              Serial.print("@motorON-1#");  // Send to ATmega32

            } else if (header.indexOf("GET /motor/off") >= 0) {
              motorState = "off";
              Serial.print("@motorOFF-1#");  // Send to ATmega32

            } else if (header.indexOf("GET /rgb/red") >= 0) {
              rgbState = "red";
              Serial.print("@rgb-RED#");  // Send to ATmega32

            } else if (header.indexOf("GET /rgb/green") >= 0) {
              rgbState = "green";
              Serial.print("@rgb-GREEN#");  // Send to ATmega32

            } else if (header.indexOf("GET /rgb/blue") >= 0) {
              rgbState = "blue";
              Serial.print("@rgb-BLUE#");  // Send to ATmega32

            } else if (header.indexOf("GET /rgb/yellow") >= 0) {
              rgbState = "yellow";
              Serial.print("@rgb-YELLOW#");  // Send to ATmega32

            } else if (header.indexOf("GET /lcd?message=") >= 0) {
              // Extract the message after ?message=
              int messageStart = header.indexOf("GET /lcd?message=") + 16;  // 16 accounts for "GET /lcd?message="
              int messageEnd = header.indexOf(" ", messageStart);
              lcdMessage = header.substring(messageStart, messageEnd);
              lcdMessage.replace("=", "");  // Remove '=' from the message if it exists
              // Create the message in the required format
              String sendMessage = "@lcd-" + lcdMessage + "#";  // Format the message
              Serial.print(sendMessage);                        // Send the formatted string to ATmega32
            }



            // Build and send the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".on { background-color: #4CAF50; }");
            client.println(".off { background-color: #555555; }");
            client.println(".rgb-button { border-radius: 50%; width: 50px; height: 50px; margin: 10px; display: inline-block;}");
            client.println(".active { width: 80px; height: 80px;}");
            client.println(".red { background-color: red; }");
            client.println(".green { background-color: green; }");
            client.println(".blue { background-color: blue; }");
            client.println(".yellow { background-color: yellow; }");
            client.println(".lcd-container { margin-top: 20px; }");
            client.println("input[type='text'] { padding: 10px; font-size: 16px; }");
            client.println("button { padding: 10px 20px; font-size: 16px; margin-top: 10px; }</style></head>");

            // Display relay control buttons
            client.println("<body><h1>Abdelrahman_Smart_Home</h1>");
            client.println("<p>Relay - State: " + relayState + "</p>");
            if (relayState == "off") {
              client.println("<p><a href=\"/relay/on\"><button class=\"button on\">Relay ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/relay/off\"><button class=\"button off\">Relay OFF</button></a></p>");
            }

            // Display motor control buttons
            client.println("<p>Motor - State: " + motorState + "</p>");
            if (motorState == "off") {
              client.println("<p><a href=\"/motor/on\"><button class=\"button on\">Motor ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/motor/off\"><button class=\"button off\">Motor OFF</button></a></p>");
            }

            // Display RGB control buttons
            client.println("<p>RGB LED Color</p>");
            client.println("<div><a href=\"/rgb/red\"><div class=\"rgb-button red " + String((rgbState == "red" ? "active" : "")) + "\"></div></a>");
            client.println("<a href=\"/rgb/green\"><div class=\"rgb-button green " + String((rgbState == "green" ? "active" : "")) + "\"></div></a>");
            client.println("<a href=\"/rgb/blue\"><div class=\"rgb-button blue " + String((rgbState == "blue" ? "active" : "")) + "\"></div></a>");
            client.println("<a href=\"/rgb/yellow\"><div class=\"rgb-button yellow " + String((rgbState == "yellow" ? "active" : "")) + "\"></div></a></div>");

            // LCD input and display
            client.println("<div class=\"lcd-container\">");
            client.println("<h2>LCD Numeric Input</h2>");
            client.println("<form action=\"/lcd\">");
            client.println("<input type=\"text\" name=\"message\" value=\"" + lcdMessage + "\" placeholder=\"Type number...\"/>");
            client.println("<button type=\"submit\">Send to LCD</button>");
            client.println("</form>");
            client.println("<p>Last LCD Message: <strong>" + lcdMessage + "</strong></p>");
            client.println("</div>");

            client.println("</body></html>");
            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Clear the header and disconnect client
    header = "";
    client.stop();
    //Serial.println("Client disconnected.");
  }
}
