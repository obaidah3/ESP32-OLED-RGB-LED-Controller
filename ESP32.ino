// Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// Wi-Fi credentials (user's router)
const char* ssid     = "Vodafone_VDSL_1019";
const char* password = "EEssaamm@1973";

// SoftAP fallback credentials in case STA connection fails
const char* ap_ssid  = "ESP32_OLED_AP";
const char* ap_pass  = "12345678";

// Web server on port 80
WiFiServer server(80);

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RGB LED pins (Common Anode)
#define RED_PIN    25
#define GREEN_PIN  26
#define BLUE_PIN   27

// Button pin and debounce handling
#define BUTTON_PIN    4
volatile bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // milliseconds

// Current face state
int currentScreen = 0;
const int totalScreens  = 6;
int ledR = 128, ledG = 128, ledB = 128;

// Preset colors for each face (R, G, B)
const uint8_t presetColors[6][3] = {
  {173, 216, 230},  // 0: Light Blue
  {255, 165, 0},    // 1: Orange
  {255, 255, 0},    // 2: Yellow
  {0, 255, 0},      // 3: Green
  {255, 0, 0},      // 4: Red
  {255, 0, 255}     // 5: Purple
};

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting...");

  pinMode(RED_PIN,   OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN,  OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Button interrupt with software debounce
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), []() {
    unsigned long now = millis();
    if (now - lastDebounceTime > debounceDelay) {
      buttonPressed = true;
      lastDebounceTime = now;
    }
  }, FALLING);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Failed to initialize OLED"));
    while (true);
  }
  display.clearDisplay(); display.display();

  // Set Wi-Fi to both STA + AP mode
  WiFi.mode(WIFI_AP_STA);

  // Attempt connection to user network
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  bool sta_connected = false;
  while (millis() - start < 20000) {
    if (WiFi.status() == WL_CONNECTED) {
      sta_connected = true;
      break;
    }
    delay(500);
    Serial.print('.');
  }

  if (sta_connected) {
    Serial.println();
    Serial.print("Connected to STA, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nSTA failed, starting SoftAP...");
    WiFi.softAP(ap_ssid, ap_pass);
    Serial.print("SoftAP IP: "); Serial.println(WiFi.softAPIP());
    Serial.print("AP SSID: "); Serial.println(ap_ssid);
  }

  server.begin();
  Serial.println("Web server started");

  updateDisplay();
  applyColor();
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false;
    // Move to next face
    currentScreen = (currentScreen + 1) % totalScreens;

    // Set the color from preset palette
    ledR = presetColors[currentScreen][0];
    ledG = presetColors[currentScreen][1];
    ledB = presetColors[currentScreen][2];

    updateDisplay();
    applyColor();
    Serial.printf("Button → Face %d, Color R=%d, G=%d, B=%d\n", currentScreen, ledR, ledG, ledB);
  }

  WiFiClient client = server.available();
  if (!client) { delay(200); return; }

  Serial.println("Client connected");

  // Read HTTP request line
  String req = client.readStringUntil('\r');
  Serial.print("Request line: "); Serial.println(req);

  // Skip HTTP headers
  while (client.available()) {
    String l = client.readStringUntil('\n');
    if (l == "\r") break;
  }

  // Parse GET parameters
  if (req.startsWith("GET ") && req.indexOf("face=") > 0) {
    parseParameters(req);
    updateDisplay();
    applyColor();
    Serial.printf("Updated: face=%d, R=%d, G=%d, B=%d\n", currentScreen, ledR, ledG, ledB);
  }

  // Send HTML response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html lang='en'>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<title>OLED & LED Control</title>");
  client.println("<style>");
  client.println("body { display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; font-family: Arial, sans-serif; background-color: #f2f2f2; }");
  client.println(".box { background: white; padding: 30px; border-radius: 15px; box-shadow: 0 0 15px rgba(0,0,0,0.2); max-width: 400px; width: 90%; }");
  client.println("h2 { text-align: center; margin-bottom: 20px; }");
  client.println("form { display: flex; flex-direction: column; gap: 15px; }");
  client.println("label { font-weight: bold; }");
  client.println("select, input[type='number'] { padding: 8px; border: 1px solid #ccc; border-radius: 5px; }");
  client.println("input[type='submit'] { padding: 10px; background-color: #007BFF; color: white; border: none; border-radius: 5px; cursor: pointer; }");
  client.println("input[type='submit']:hover { background-color: #0056b3; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<div class='box'>");
  client.println("<h2>OLED & LED Control</h2>");
  client.println("<div style='margin-bottom: 20px; padding: 10px; background: #f9f9f9; border-left: 4px solid #007BFF;'>");
  client.println("<strong>Current Face:</strong> Face " + String(currentScreen) + "<br>");
  client.println("<strong>LED Color:</strong> R: " + String(ledR) + ", G: " + String(ledG) + ", B: " + String(ledB));
  client.println("</div>");

  client.println("<form method='GET'>");
  client.println("<label for='face'>Select face:</label>");
  client.println("<select name='face' id='face'>");
  for (int i = 0; i < totalScreens; i++) {
    client.printf("<option value='%d'%s>Face %d</option>", i, i == currentScreen ? " selected" : "", i);
  }
  client.println("</select>");

  client.println("<label for='r'>Red (0–255):</label>");
  client.println("<input type='number' id='r' name='r' min='0' max='255' value='" + String(ledR) + "'>");

  client.println("<label for='b'>Blue (0–255):</label>");
  client.println("<input type='number' id='b' name='b' min='0' max='255' value='" + String(ledB) + "'>");

  client.println("<label for='g'>Green (0–255):</label>");
  client.println("<input type='number' id='g' name='g' min='0' max='255' value='" + String(ledG) + "'>");

  client.println("<input type='submit' value='Update'>");
  client.println("</form>");
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");

  client.stop();
  Serial.println("Client disconnected\n");
}

void updateDisplay() {
  display.clearDisplay();
  switch (currentScreen) {
    case 0: drawHappyFace();    break;
    case 1: drawSadFace();      break;
    case 2: drawNeutralFace();  break;
    case 3: drawExcitedFace();  break;
    case 4: drawAngryFace();    break;
    case 5: drawSurprisedFace();break;
    default: drawNeutralFace(); break;
  }
  display.display();
}

void applyColor() {
  analogWrite(RED_PIN,   ledR);
  analogWrite(GREEN_PIN, ledG);
  analogWrite(BLUE_PIN,  ledB);
}

void parseParameters(const String& req) {
  currentScreen = getValue(req, "face").toInt();
  ledR          = getValue(req, "r").toInt();
  ledG          = getValue(req, "g").toInt();
  ledB          = getValue(req, "b").toInt();
}

String getValue(const String& data, const char* name) {
  int idx = data.indexOf(name);
  if (idx < 0) return "0";
  int start = data.indexOf('=', idx) + 1;
  int end   = data.indexOf('&', start);
  if (end < 0) end = data.indexOf(' ', start);
  return data.substring(start, end);
}

// Draw arc from startAngle to endAngle at (x, y) with given radius and color
void drawArcOnDisplay(int x, int y, int w, int h, int sa, int ea, uint16_t c) {
  for (int i = sa; i <= ea; i++) {
    float a = i * PI/180.0;
    display.drawPixel(x + cos(a)*w, y + sin(a)*h, c);
  }
}

// Face drawing functions
void drawHappyFace()    { display.drawCircle(64,32,20,SSD1306_WHITE); display.fillCircle(56,26,2,SSD1306_WHITE); display.fillCircle(72,26,2,SSD1306_WHITE); drawArcOnDisplay(64,38,10,10,30,150,SSD1306_WHITE); }
void drawSadFace()      { display.drawCircle(64,32,20,SSD1306_WHITE); display.fillCircle(56,26,2,SSD1306_WHITE); display.fillCircle(72,26,2,SSD1306_WHITE); drawArcOnDisplay(64,44,10,10,210,330,SSD1306_WHITE); }
void drawNeutralFace()  { display.drawCircle(64,32,20,SSD1306_WHITE); display.fillCircle(56,26,2,SSD1306_WHITE); display.fillCircle(72,26,2,SSD1306_WHITE); display.drawLine(56,40,72,40,SSD1306_WHITE); }
void drawExcitedFace()  { display.drawCircle(64,32,20,SSD1306_WHITE); display.fillCircle(52,26,3,SSD1306_WHITE); display.fillCircle(76,26,3,SSD1306_WHITE); drawArcOnDisplay(64,38,12,12,20,160,SSD1306_WHITE); }
void drawAngryFace()    { display.drawCircle(64,32,20,SSD1306_WHITE); display.fillCircle(56,26,2,SSD1306_WHITE); display.fillCircle(72,26,2,SSD1306_WHITE); display.drawLine(52,20,60,24,SSD1306_WHITE); display.drawLine(76,20,68,24,SSD1306_WHITE); drawArcOnDisplay(64,44,10,8,210,330,SSD1306_WHITE); }
void drawSurprisedFace(){ display.drawCircle(64,32,20,SSD1306_WHITE); display.fillCircle(56,26,2,SSD1306_WHITE); display.fillCircle(72,26,2,SSD1306_WHITE); display.drawCircle(64,40,4,SSD1306_WHITE); }
