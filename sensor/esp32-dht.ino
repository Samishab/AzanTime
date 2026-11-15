// نُور — حساس حرارة DHT22 أو DHT11 (وحدة بثلاثة أرجل) على لوحة ESP32 ذات 30 دبوساً.
// غير مُجرَّب على جهاز فعلي بعد: جرّبه وأخبرنا بالنتيجة.
// الرابط الذي يُكتب في إعدادات الشاشة: http://عنوان-اللوحة/temp  (يعيد {"temperature":25.3})
//
// التوصيل:  (+) أو VCC -> 3V3   |  (-) أو GND -> GND   |  OUT أو DATA أو S -> D4 (GPIO4)
// المكتبات: DHT sensor library (Adafruit) و Adafruit Unified Sensor (من Library Manager)
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* WIFI_NAME = "YOUR_WIFI_NAME";      // اسم شبكة الواي فاي (2.4GHz فقط)
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";  // كلمتها

#define DHT_PIN 4
#define DHT_TYPE DHT22     // غيّرها إلى DHT11 إن كان حساسك DHT11
DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

float lastTemp = NAN;
unsigned long lastRead = 0;

void connectWifi() {
  WiFi.mode(WIFI_STA);
  // لتثبيت العنوان (اختياري، غيّر الأرقام لتناسب شبكتك) أزل علامة التعليق عن السطرين:
  // IPAddress ip(192, 168, 1, 60), gw(192, 168, 1, 1), mask(255, 255, 255, 0);
  // WiFi.config(ip, gw, mask);
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) delay(300);
}

void handleTemp() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-store");
  if (isnan(lastTemp)) {
    server.send(503, "application/json", "{\"error\":\"no reading\"}");
    return;
  }
  server.send(200, "application/json", "{\"temperature\":" + String(lastTemp, 1) + "}");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWifi();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());   // اكتب هذا العنوان في إعدادات الشاشة
  server.on("/temp", handleTemp);
  server.begin();
}

void loop() {
  server.handleClient();
  if (WiFi.status() != WL_CONNECTED) connectWifi();
  if (lastRead == 0 || millis() - lastRead > 5000) {
    lastRead = millis();
    float t = dht.readTemperature();
    lastTemp = isnan(t) ? NAN : t;
  }
}
