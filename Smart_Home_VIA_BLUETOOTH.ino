#include "BluetoothSerial.h"

BluetoothSerial SerialBT;  // Bluetooth Serial object

#define TX_PIN 17  // Pin for communication to ATmega32
#define BAUD_RATE 9600
#define NONE -1


String relayState = "off";
String motorState = "off";
String receivedData = "";  // To store the incoming Bluetooth data


void setup() {

  // Initialize Serial communication to ATmega32
  Serial.begin(BAUD_RATE, SERIAL_8N1, NONE, TX_PIN);  // No RX, only TX

  // Initialize Bluetooth communication
  SerialBT.begin("ESP32-Bluetooth");  // Bluetooth device name

}

void loop() {
  // Check if data is available via Bluetooth
  if (SerialBT.available()) {

    while (SerialBT.available()) {
      char c = SerialBT.read();  // Read each character from Bluetooth
      receivedData += c;         // Append the character to the string
    }

    // Send data to ATmega32 based on Bluetooth command
    if ((receivedData == "RELAY_SWITCH1") && (relayState == "off")) {
      relayState = "on";
      Serial.print("@relayON-1#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData == "RELAY_SWITCH1") && (relayState == "on")) {
      relayState = "off";
      Serial.print("@relayOFF-1#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData == "MOTOR_SWITCH1") && (motorState == "off")) {
      motorState = "on";
      Serial.print("@motorON-1#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData == "MOTOR_SWITCH1") && (motorState == "on")) {
      motorState = "off";
      Serial.print("@motorOFF-1#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData == "RGB_RED")) {
      Serial.print("@rgb-RED#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData == "RGB_GREEN")){
      Serial.print("@rgb-GREEN#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData == "RGB_BLUE")){
      Serial.print("@rgb-BLUE#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData == "RGB_YELLOW")){
      Serial.print("@rgb-YELLOW#");  // Send to ATmega32
      receivedData = "";

    } else if ((receivedData.startsWith("LCD_")) && (receivedData.endsWith("#")))  {
      String lcdMessage = receivedData.substring(4);  // Extract message after "lcd-"
      Serial.print("@lcd-" + lcdMessage);  // Send to ATmega32 formatted
      receivedData = "";

    }
  }
}
