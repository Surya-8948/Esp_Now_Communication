#include <ESP8266WiFi.h>
#include <espnow.h>
//  PUT BOARD A's MAC ADDRESS HERE
uint8_t BOARD_A_MAC[] = {0xA4, 0xCF, 0x12, 0x34, 0x56, 0x78};

void onReceive(uint8_t *mac, uint8_t *data, uint8_t len) {
  // FIX #2: Safe null-terminated copy
  char buf[251];
  if (len > 250) len = 250;
  memcpy(buf, data, len);
  buf[len] = '\0';

  Serial.print("Received: ");
  Serial.println(buf);
  // Reply back to Board A
  char reply[] = "Hi A, got it!";
  int sendResult = esp_now_send(BOARD_A_MAC, (uint8_t *)reply, strlen(reply) + 1);
  if (sendResult != 0) {
    Serial.print("Reply send failed to queue. Error: ");
    Serial.println(sendResult);
  }
}

void onSent(uint8_t *mac, uint8_t status) {
  Serial.print("Send status: ");
  Serial.println(status == 0 ? "Success (delivered)" : "Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(onReceive);
  esp_now_register_send_cb(onSent);

  // FIX #3: Check peer registration
  int result = esp_now_add_peer(BOARD_A_MAC, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  if (result != 0) {
    Serial.print("Failed to add peer. Error code: ");
    Serial.println(result);
    return;
  }

  Serial.println("Board B ready");
}

void loop() {
  // Nothing here — replies are sent from onReceive()
}
