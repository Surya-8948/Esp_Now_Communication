#include <ESP8266WiFi.h>
#include <espnow.h>
//  PUT BOARD B's MAC ADDRESS HERE
uint8_t BOARD_B_MAC[] = {0xA4, 0xCF, 0x12, 0x34, 0x56, 0x79}; // change mac Address

// Callback: runs when data is RECEIVED
void onReceive(uint8_t *mac, uint8_t *data, uint8_t len) 
{
  char buf[251];                 // ESP-NOW max payload = 250
  if (len > 250) len = 250;      // guard against overflow
  memcpy(buf, data, len);
  buf[len] = '\0';               // null-terminate
  Serial.print("Received: ");
  Serial.println(buf);
}

// Callback: runs when data is SENT (hardware-level status)
void onSent(uint8_t *mac, uint8_t status)
{
  Serial.print("Send status: ");
  Serial.println(status == 0 ? "Success (delivered)" : "Fail");
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0)
  {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(onReceive);
  esp_now_register_send_cb(onSent);

  // FIX #3: Check add_peer() return value
  int result = esp_now_add_peer(BOARD_B_MAC, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  if (result != 0)
  {
    Serial.print("Failed to add peer. Error code: ");
    Serial.println(result);
    return;   // don't try to send to a peer that isn't registered
  }

  Serial.println("Board A ready");
}

void loop() 
{
  char msg[] = "Hello from A";

  // FIX #1: send len + 1 to include the null terminator
  // FIX #4: check return value of esp_now_send()
  int sendResult = esp_now_send(BOARD_B_MAC, (uint8_t *)msg, strlen(msg) + 1);

  if (sendResult != 0)
  {
    Serial.print("esp_now_send() failed to queue. Error: ");
    Serial.println(sendResult);
  }
  
  delay(2000);
}
