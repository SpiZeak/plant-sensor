#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Target MAC address
uint8_t targetMacAddress[] = {0xC4, 0x5B, 0xBE, 0x6C, 0xDB, 0x7C};

// Callback when data is sent
void onDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Send Status: ");
  Serial.println(sendStatus == 0 ? "Delivery Success" : "Delivery Fail");

  // Enter deep sleep mode for 30 minutes
  ESP.deepSleep(4e6); // 1e7 is equivalent to 10,000,000 microseconds = 10 seconds
  // ESP.deepSleep(1.8e9); // 1.8e9 is equivalent to 1,800,000,000 microseconds = 30 minutes
}

void setup()
{
  // Disable pin D4
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);

  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while (!Serial)
  {
  }

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Read A0 pin
  int sensorValue = analogRead(A0);
  Serial.print("\rSensor Value: ");
  Serial.println(sensorValue);

  // Register the send callback
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onDataSent);

  // Add the peer
  esp_now_add_peer(targetMacAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  Serial.println("Sending data...");

  // Send data
  esp_now_send(targetMacAddress, (uint8_t *)&sensorValue, sizeof(sensorValue));
}

void loop()
{
}
