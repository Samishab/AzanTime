// نُور — حساس حرارة DS18B20 (سلك ثلاثي) على لوحة ESP32 ذات 30 دبوساً.
// غير مُجرَّب على جهاز فعلي بعد: جرّبه وأخبرنا بالنتيجة.
// الرابط الذي يُكتب في إعدادات الشاشة: http://عنوان-اللوحة/temp  (يعيد {"temperature":25.3})
//
// التوصيل:  أحمر (VCC) -> 3V3   |  أسود (GND) -> GND   |  أصفر (DATA) -> D4 (GPIO4)
// مقاومة 4.7 كيلو أوم بين DATA و3V3 إن كان الحساس سلكاً مجرداً بلا لوحة صغيرة.
// المكتبات: OneWire و DallasTemperature (من Library Manager)
#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* WIFI_NAME = "YOUR_WIFI_NAME";      // اسم شبكة الواي فاي (2.4GHz فقط)
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";  // كلمتها

#define ONE_WIRE_PIN 4
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
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
  pinMode(ONE_WIRE_PIN, INPUT_PULLUP);   // مقاومة رفع داخلية احتياطية؛ الأفضل مع ذلك استعمال المقاومة 4.7 كيلو أوم
  sensors.begin();
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
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    lastTemp = (t == DEVICE_DISCONNECTED_C || t < -50) ? NAN : t;   // -127 = الحساس غير موصول
  }
}
