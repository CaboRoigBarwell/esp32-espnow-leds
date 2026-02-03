#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WebServer.h>

// --- CONFIG ---
const int WIFI_CHANNEL = 1; // make sure peers use same channel
WebServer server(80);

// Peers' MAC addresses (update if needed)
uint8_t macA[6] = {0x40,0x4C,0xCA,0x3C,0x49,0x30}; // Device A
uint8_t macB[6] = {0x40,0x4C,0xCA,0x3C,0x4C,0xC0}; // Device B

struct Msg {
  uint8_t ledIndex; // 0 or 1
  uint8_t state;    // 0 = OFF, 1 = ON
};

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send to ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X%s", mac_addr[i], i < 5 ? ":" : "\n");
  }
  Serial.printf("status: %d\n", status);
}

bool addPeer(const uint8_t *peerMac) {
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = WIFI_CHANNEL;
  peerInfo.encrypt = false;
  if (esp_now_is_peer_exist(peerInfo.peer_addr)) return true;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return false;
  }
  return true;
}

void handleRoot() {
  server.send(200, "text/html",
    "<html><body>"
    "<h3>ESP-NOW LED Control</h3>"
    "<div>" 
    "<button onclick=\"set('A',0,1)\">A LED1 ON</button> "
    "<button onclick=\"set('A',0,0)\">A LED1 OFF</button><br>"
    "<button onclick=\"set('A',1,1)\">A LED2 ON</button> "
    "<button onclick=\"set('A',1,0)\">A LED2 OFF</button><br><br>"
    "<button onclick=\"set('B',0,1)\">B LED1 ON</button> "
    "<button onclick=\"set('B',0,0)\">B LED1 OFF</button><br>"
    "<button onclick=\"set('B',1,1)\">B LED2 ON</button> "
    "<button onclick=\"set('B',1,0)\">B LED2 OFF</button>"
    "</div>"
    "<script>function set(dev,i,s){fetch(`/set?dev=${dev}&i=${i}&s=${s}`).then(r=>r.text()).then(t=>console.log(t))}</script>"
    "</body></html>"
  );
}

void handleSet() {
  String dev = server.arg("dev");
  int idx = server.arg("i").toInt();
  int st = server.arg("s").toInt();
  Msg m = { (uint8_t)idx, (uint8_t)st };
  uint8_t *target = nullptr;
  if (dev == "A") target = macA;
  else if (dev == "B") target = macB;
  else { server.send(400, "text/plain", "Invalid device"); return; }

  if (!addPeer(target)) {
    server.send(500, "text/plain", "Failed to add peer");
    return;
  }
  esp_err_t res = esp_now_send(target, (uint8_t*)&m, sizeof(m));
  server.send(200, "text/plain", res == ESP_OK ? "OK" : "SEND_ERR");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-AP", "", WIFI_CHANNEL);
  Serial.printf("AP IP: %s\n", WiFi.softAPIP().toString().c_str());

  // enforce channel
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_send_cb(onSent);

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
