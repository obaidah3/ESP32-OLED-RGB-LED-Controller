## 🚀 ESP32 OLED & RGB LED Controller**

This project demonstrates how to control an SSD1306-based OLED display and a common-anode RGB LED with an ESP32. It features both physical and web-based interfaces:

* 🔘 **Physical button**: Cycles through predefined emoji-style faces on the OLED and changes the LED color to preset values.
* 🌐 **Web interface**: Access a simple HTML form to select a face and manually adjust the RGB channels.

   ![Image](https://github.com/user-attachments/assets/ba8d5556-762b-4f35-b37c-1f074043c988)

---

## ✨ Features

1. 😀 **OLED display** shows six different faces (happy, sad, neutral, excited, angry, surprised).
2. 🎨 **RGB LED** color updates to match each face or custom values via web form.
3. 🔄 **Physical button** on pin 4 cycles through faces and colors (debounced).
4. 📶 **Wi‑Fi connectivity** in STA + SoftAP mode:

   * Tries to connect to your local network (SSID/PASSWORD).
   * If STA fails within 20 s, falls back to SoftAP (`ESP32_OLED_AP` / `12345678`).
5. 💻 **Web server** on port 80 with an HTML form:
 
   * IP initialize After Wi‑Fi connectivity.

   
     ![Image](https://github.com/user-attachments/assets/fc089b96-f625-422e-abaa-19df2cd82180)
   


   * Paste IP in the URL.

   
     ![Image](https://github.com/user-attachments/assets/b1aab926-522e-4e7c-91a0-5b8a943abe47)



   * Select face index (0–5).
  
     
     ![Image](https://github.com/user-attachments/assets/8bf69cc7-0de3-4208-9bb9-21f38a8f6992)
     


   * Enter R, G, B values (0–255).


     ![Image](https://github.com/user-attachments/assets/ba8d5556-762b-4f35-b37c-1f074043c988)

---

## 🔌 Hardware Requirements

* ESP32 development board
* SSD1306 128×64 I²C OLED display
* Common‑anode RGB LED
* Pushbutton (active‑LOW)
* 3 resistors for LED (e.g. 220 Ω each)
* Breadboard and jumper wires

---

## 📌 Pin Connections

| Signal           | ESP32 Pin | Component              |
| ---------------- | --------- | ---------------------- |
| OLED SDA         | GPIO 21   | SSD1306 SDA            |
| OLED SCL         | GPIO 22   | SSD1306 SCL            |
| RGB LED Common + | 3.3 V     | RGB LED anode          |
| Red cathode      | GPIO 25   | through 220 Ω resistor |
| Green cathode    | GPIO 26   | through 220 Ω resistor |
| Blue cathode     | GPIO 27   | through 220 Ω resistor |
| Button (to GND)  | GPIO 4    | with internal pull‑up  |
| Serial debug     | TX0/RX0   | USB Serial Monitor     |

> **Note:** The RGB LED is common‑anode; pins drive cathodes with `analogWrite()`.

---

## 📥 Software Dependencies

Install the following libraries in the Arduino IDE:

* **Adafruit SSD1306**
* **Adafruit GFX**
* **WiFi** (included with ESP32 core)

---

## ⚙️ Configuration

1. Open the sketch in the Arduino IDE.
2. Update the Wi‑Fi credentials:

   ```cpp
   const char* ssid     = "<YOUR_SSID>";
   const char* password = "<YOUR_PASSWORD>";
   ```
3. (Optional) Change the SoftAP SSID and password if desired:

   ```cpp
   const char* ap_ssid  = "ESP32_OLED_AP";
   const char* ap_pass  = "12345678";
   ```
4. Verify board and port, then upload.

---

## 🚀 Usage

1. **Power up**: ESP32 boots, initializes OLED, attempts Wi‑Fi STA.
2. **Serial monitor** (115200 baud) shows:

   * `Connecting to WiFi...` dots until success or timeout
   * `Connected to STA, IP: 192.168.x.x` or
   * `STA failed, starting SoftAP` and SoftAP IP
3. **Physical control**:

   * Press button → cycles face (0–5) and updates LED color.
4. **Web control**:

   1. Browse to the ESP32’s IP in a browser (STA or AP IP).
   2. Use the form to choose a face or set custom R/G/B values.
   3. Submit → updates OLED and LED in real time.

---

## 🛠️ Customization

* **Add faces**: Implement new `drawXFace()` and extend `totalScreens` and `presetColors`.
* **Color presets**: Edit `presetColors` array to change default colors for each face.
* **Port changes**: Modify pin definitions at top.

---
## 😀 preview

***faces:**

![Image](https://github.com/user-attachments/assets/94677fe1-9e67-4df6-8d0d-ed8d8a1f1872)

![Image](https://github.com/user-attachments/assets/839c1946-3cbb-45b8-8b72-0bf77a422346)

![Image](https://github.com/user-attachments/assets/b189f9e2-90e5-47f3-aee4-f48d6842d3bb)

![Image](https://github.com/user-attachments/assets/6a8fd5fa-e049-49c8-908c-2a0ab353a364)

![Image](https://github.com/user-attachments/assets/94beb38c-73bf-4ab6-b32e-ee6667b6b182)


---

## 📝 License

This project is released under the MIT License. See [LICENSE](LICENSE.md) for details.
