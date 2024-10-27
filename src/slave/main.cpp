#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <FirebaseClient.h>
#include <config.h>

void connectToWiFi()
{
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
  }

  Serial.println("Connected!");
}

String macToBase36(uint8_t *macAddress)
{
  // Combine the MAC address bytes into a single 48-bit integer
  unsigned long long macInt = 0;
  for (int i = 0; i < 6; i++)
  {
    macInt = (macInt << 8) | macAddress[i];
  }

  // Convert integer to Base36 string
  String base36 = "";
  const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  while (macInt > 0)
  {
    base36 = chars[macInt % 36] + base36;
    macInt /= 36;
  }

  return base36;
}

String constructMac(uint8_t *macAddress)
{
  String mac = "";
  for (int i = 0; i < 6; i++)
  {
    if (macAddress[i] < 16)
      mac += "0"; // Add leading zero for single hex digits
    mac += String(macAddress[i], HEX);
    if (i < 5)
      mac += ":";
  }
  return mac;
}

// Callback function that will be executed when data is received
void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  String shortMac = macToBase36(mac);
  Serial.println("\r\nReceived data from: " + shortMac);
  Serial.print("Decoded MAC: ");
  Serial.println(constructMac(mac));

  if (len == sizeof(int))
  {
    // Copy the bytes into an integer variable
    int receivedValue;
    memcpy(&receivedValue, incomingData, sizeof(int));

    // Print the received integer
    Serial.println("Data (int): " + String(receivedValue));
  }
  else
  {
    Serial.println("Unexpected data length!");
  }

  for (int i = 0; i <= 1; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(80);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize serial communication at 115200 baud
  Serial.begin(115200);

  // Wait for serial communication to be established
  delay(1000);

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Connect to the Wi-Fi network
  connectToWiFi();

  // Initialize ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  Serial.println("\rESP-NOW Initialized");

  // Register the receive callback
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataRecv);
}

void loop()
{
}
