#include <Arduino.h>
#include <espnow.h>
#include <FirebaseClient.h>
#include <config.h>
#include <WiFiManager.h>

WiFiManager wm;

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

  // Get the MAC address
  String macStr = WiFi.macAddress();
  uint8_t mac[6];
  sscanf(macStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
  String shortMac = macToBase36(mac);

  // Connect to Wi-Fi
  String ssid = "PLANT SENSOR " + shortMac;
  std::vector<const char *> wm_menu = {"wifi", "exit"};
  wm.setShowInfoUpdate(false);
  wm.setShowInfoErase(false);
  wm.setMenu(wm_menu);
  wm.autoConnect(ssid.c_str());

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
  // Do nothing
}
