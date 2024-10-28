#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Target MAC address
uint8_t targetMacAddress[] = {0x80, 0x65, 0x99, 0xe3, 0xb7, 0xc6};

// Create peer interface
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  ESP.deepSleep(4e6); // 1e7 is equivalent to 10,000,000 microseconds = 10 seconds
  // ESP.deepSleep(1.8e9); // 1.8e9 is equivalent to 1,800,000,000 microseconds = 30 minutes
}

void setup()
{
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);

  delay(1000);

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Read sensor pin
  int sensorValue = analogRead(1);
  Serial.print("\rSensor Value: ");
  Serial.println(sensorValue);

  // Register the send callback
  esp_now_register_send_cb(onDataSent);

  // Add the peer
  memcpy(peerInfo.peer_addr, targetMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Sending data...");

  // Send data
  esp_err_t result = esp_now_send(targetMacAddress, (uint8_t *)&sensorValue, sizeof(sensorValue));
}

void loop()
{
}
