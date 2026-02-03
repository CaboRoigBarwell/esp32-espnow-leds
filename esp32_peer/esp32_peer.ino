#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// --- CONFIG: set these to the pins for your C3 super mini LEDs ---
const int LED0_PIN = 8; // LED1
const int LED1_PIN = 9; // LED2
const int WIFI_CHANNEL = 1; // must match master

struct Msg {
  uint8_t ledIndex;
  uint8_t state;
};

void onRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len != sizeof(Msg)) return;
  Msg m;
  memcpy(&m, incomingData, sizeof(m));
  Serial.printf("Recv from %02X:%02X:%02X:%02X:%02X:%02X -> led %d = %d\n",
    mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], m.ledIndex, m.state);
  int pin = (m.ledIndex == 0) ? LED0_PIN : LED1_PIN;
  digitalWrite(pin, m.state ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED0_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);

  WiFi.mode(WIFI_STA); // required for ESP-NOW receiver
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_recv_cb(onRecv);

  Serial.println("Ready to receive ESP-NOW messages");
}

void loop() {
  // nothing here; responses handled in callback
}
