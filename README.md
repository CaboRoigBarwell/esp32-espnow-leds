ESP32 ESP-NOW LED Control (Arduino IDE)

Contents
- `esp32_master/esp32_master.ino`  - hosts a small web UI (AP) and sends ESP-NOW messages
- `esp32_peer/esp32_peer.ino`      - ESP32-C3 peer that receives messages and toggles two LEDs

Quick start
1. Open Arduino IDE.
2. Import the zip: Sketch -> Include Library -> Add .ZIP Library... (or open the sketch folder directly).
3. Configure the sketches if needed:
   - In `esp32_master.ino`, verify the MAC addresses for Device A and B (already set to the ones you provided).
   - In `esp32_peer.ino`, set `LED0_PIN` and `LED1_PIN` to the correct GPIOs for your ESP32-C3 super mini.
   - Ensure `WIFI_CHANNEL` is the same in both sketches (default: 1).
4. Flash two copies of `esp32_peer.ino` to each ESP32-C3 (change pins if needed).
5. Flash `esp32_master.ino` to the master ESP32.
6. Open Serial Monitor to view logs. Connect to the master AP (`ESP32-AP`) from a browser and open `http://192.168.4.1/` to see the control page.

Notes & troubleshooting
- ESP-NOW requires sender and receiver to be on the same Wi-Fi channel.
- If messages do not arrive: check channel, MAC addresses, and Serial logs.
- You can test broadcast messages by sending to the broadcast address if needed.

MAC addresses included:
- A: 40:4C:CA:3C:49:30
- B: 40:4C:CA:3C:4C:C0

If you want, I can also:
- Update the sketches with the exact LED GPIOs for your board, or
- Produce a single zip containing a combined README and a windows-friendly instructions file.
