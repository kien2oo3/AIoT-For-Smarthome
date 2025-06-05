// 🚀🚀🚀🚀🚀🚀🚀🚀 KHAI BÁO BLYNK LOCAL SERVER 🚀🚀🚀🚀🚀🚀🚀🚀
// #define BLYNK_TEMPLATE_ID "TMPL6bBp0Lwdq"         
// #define BLYNK_TEMPLATE_NAME "Smart House"       
#define BLYNK_AUTH_TOKEN "3x1_Yw6KFOuYXgBzsL9S_zt6cXNSGK9x"
#define BLYNK_SERVER_IP "192.168.94.40"
// #define BLYNK_SERVER_IP "192.168.0.22"
#define BLYNK_SERVER_PORT 8080
#define BLYNK_PRINT Serial // Kích hoạt việc in thông tin Blynk trên Serial Monitor
// 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀


// ⚡⚡⚡⚡⚡⚡⚡ KHAI BÁO CÁC THƯ VIỆN ⚡⚡⚡⚡⚡⚡⚡
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>  
#include <MQUnifiedsensor.h>
#include "time.h"
// ⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡


// 🎨🎨🎨🎨🎨🎨🎨🎨🎨 DANH SÁCH MÀU 🎨🎨🎨🎨🎨🎨🎨🎨🎨
#define COLOR_GREEN "#2ecc71"
#define COLOR_RED "#e74c3c"

// 🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨🎨

// 🌟🌟🌟🌟🌟🌟🌟 DANH SÁCH TOPIC 🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟
#define DHT11_TOPIC "home/sensor/dht11/temp-humi"
#define DHT11_STATUS_TOPIC "home/sensor/dht11/status"
#define MQ7_TOPIC "home/sensor/mq-7/co"
#define MQ2_TOPIC "home/sensor/mq-2/gas"
#define RC522_TOPIC "home/sensor/rc522/rfid"
#define FLAME_TOPIC "home/sensor/flame/command"
#define PIR_TOPIC "home/sensor/hc-sr501/command"

// 🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟


// 🌐🌐🌐🌐🌐🌐🌐🌐🌐 BIẾN TOÀN CỤC 🌐🌐🌐🌐🌐🌐🌐🌐🌐
bool isShowMess = true;  // Show lời chào trong LCD Widget Blynk App
// Biến kiểm tra kết nối MQTT và Blynk
unsigned long previousConnectMQTTAttempt = 0;
unsigned long previousConnectBlynkAttempt = 0;
// Biến lưu trạng thái các cảm biến 
bool isDHTError = false;
bool isRFIDError = false;
// bool isMQ7Error = false;
// bool isMQ2Error = false;
// Biến xử lý chuỗi JSON

String lastRFIDState = "";  // Trạng thái cuối của RFID

StaticJsonDocument<160> jsonDoc;
char jsonBuffer[160];
// 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐


// ✨✨✨✨✨✨✨✨✨✨ KHAI BÁO PINS ✨✨✨✨✨✨✨✨✨
// 🎚️ Virtual Pins🎚️
// Update Status:
#define DHT_LED_STATUS_VP 10      // V10 - Trạng thái hoạt động của cảm biến DHT11 trên Blynk app
#define RFID_LED_STATUS_VP 11     // V11 - Trạng thái hoạt động của cảm biến RFID RC522 trên Blynk app
#define MQ7_LED_STATUS_VP 12      // V12 - Trạng thái hoạt động của cảm biến MQ-7 trên Blynk app
#define FLAME_LED_STATUS_VP 13    // V13 - Trạng thái hoạt động của cảm biến lửa trên Blynk app
#define PIR_LED_STATUS_VP 14      // V14 - Trạng thái hoạt động của cảm biến hồng ngoại trên Blynk app
#define MQ2_LED_STATUS_VP 15      // V15 - Trạng thái hoạt động của cảm biến MQ-2 trên Blynk app
// Update data:
#define LCD_VALUE_LINE1_VP 0      // V0 - Màn hình LCD hiển thị lời chào dòng 1 trên Blynk app
#define LCD_VALUE_LINE2_VP 1      // V1 - Màn hình LCD hiển thị lời chào dòng 2 trên Blynk app
#define FLAME_VALUE_DIGI_VP 2     // V2 - Hiển thị giá trị 0/1 (không có lửa/có lửa) trên Blynk app
#define DHT_VALUE_CEL_VP 3        // V3 - Hiển thị giá trị nhiệt độ C  trên Blynk app
#define DHT_VALUE_HUMI_VP 4       // V4 - Hiển thị giá trị độ ẩm  trên Blynk app
#define DHT_VALUE_Fah_VP 5        // V5 - Hiển thị giá trị nhiệt độ F  trên Blynk app
#define MQ7_VALUE_PPM_VP 6        // V6 - Hiển thị giá trị khí CO  trên Blynk app
#define MQ2_VALUE_PPM_VP 7        // V7 - Hiển thị giá trị khí gas trên Blynk app

// 🎚️End Virtual Pins 🎚️

// 📶 GPIO Pins
#define DHT_PIN 2                     // GPIO2 - Chân AO DHT11 
#define RFID_SS_PIN  7                // GPIO7 - Chip Select
#define RFID_RST_PIN 0                // GPIO0 - Reset
#define MQ7_ANALOG_PIN  3             // GPIO3 - Chân analog đọc MQ-7
#define FLAME_DIGITAL_PIN 15          // GPIO15 - Chân DO Flame Sensor
#define HCSR501_DIGITAL_PIN 23        // GPIO23 - Chân DO HC SR501 Sensor
#define MQ2_ANALOG_PIN 1              // GPIO1 - Chân analog đọc MQ-2
// 📶End GPIO Pins

// Khai báo cảm biến DHT11
#define DHT_TYPE DHT11      
DHT dht(DHT_PIN, DHT_TYPE); 
WidgetLED dhtStatus(DHT_LED_STATUS_VP);         // LED trạng thái cảm biến DHT11

// Khai báo cảm biến RFID-RC522
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);
WidgetLED rfidStatus(RFID_LED_STATUS_VP);       // LED trạng thái cảm biến RFID

// Khai báo cảm biến lửa
WidgetLED flameStatus(FLAME_LED_STATUS_VP);     // LED trạng thái cảm biến lửa

// Khai báo cảm biến PIR
WidgetLED pirStatus(PIR_LED_STATUS_VP);         // LED trạng thái cảm biến PIR

// ====== Cấu hình MQ-7, MQ-2 ======
#define BOARD "ESP32"
#define Voltage_Resolution 5          // Mức điện áp ADC (của cảm biến, thường là 5V)
#define ADC_Bit_Resolution 12         // ESP32-C6 có ADC 12-bit
// ====== Cấu hình MQ-7 ======
#define MQ7_Type "MQ-7"               // MQ-7
#define RatioMQ7CleanAir 27.5         // RS / R0 = 27.5 ppm

// ====== Cấu hình MQ-2 ======
#define MQ2_Type "MQ-2"
#define RatioMQ2CleanAir 9.83           // Tỉ lệ mặc định cho MQ-2

// ====== Biến cảm biến MQ-7 ======
MQUnifiedsensor MQ7(BOARD, Voltage_Resolution, ADC_Bit_Resolution, MQ7_ANALOG_PIN, MQ7_Type);
WidgetLED mq7Status(MQ7_LED_STATUS_VP);     // LED trạng thái cảm biến MQ-7

// ====== Cảm biến MQ-2 ======
MQUnifiedsensor MQ2(BOARD, Voltage_Resolution, ADC_Bit_Resolution, MQ2_ANALOG_PIN, MQ2_Type);
WidgetLED mq2Status(MQ2_LED_STATUS_VP);     // LED trạng thái cảm biến MQ-2

// ✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨


// 🛜🛜🛜🛜🛜🛜🛜🛜🛜 CONNECT INFO 🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜
// Wi-Fi credentials
// char ssid[] = "Tầng 3";      // Tên mạng Wi-Fi
// char pass[] = "13572468";     // Mật khẩu Wi-Fi
char ssid[] = "Wifi vip";      // Tên mạng Wi-Fi
char pass[] = "11122003";     // Mật khẩu Wi-Fi

// MQTT HiveMQ Cloud
// const char* mqtt_server = "db6c39fa55a94251b9941e77a4756216.s1.eu.hivemq.cloud";
// const int mqtt_port = 8883;
// const char* mqtt_user = "kienbui12003";
// const char* mqtt_pass = "Kien!!!2";
// WiFiClientSecure wifiClient;
// PubSubClient client(wifiClient);

// MQTT HiveMQ Local Server
// const char* mqtt_server = "192.168.0.22";
const char* mqtt_server = "192.168.94.40";
const char* mqtt_user = "user_publish";
const char* mqtt_pass = "kien1";
const int mqtt_port = 8883;   // Cổng TLS
// WiFiClient wifiClient;
// PubSubClient client(wifiClient);
WiFiClientSecure secureClient;
PubSubClient client(secureClient);
// Chứng chỉ CA của Broker (định dạng PEM)
const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n"
"MIIDpzCCAo+gAwIBAgIUHpkm/SqtLLtPz+61Z/yFQrxBX58wDQYJKoZIhvcNAQEL\n"
"BQAwYzELMAkGA1UEBhMCVk4xDjAMBgNVBAgMBUhhbm9pMQ4wDAYDVQQHDAVIYW5v\n"
"aTEQMA4GA1UECgwHS2llbklvVDELMAkGA1UECwwCSVQxFTATBgNVBAMMDDE5Mi4x\n"
"NjguMC4yMjAeFw0yNTA1MjIxNDAwMjRaFw0yNjA1MjIxNDAwMjRaMGMxCzAJBgNV\n"
"BAYTAlZOMQ4wDAYDVQQIDAVIYW5vaTEOMAwGA1UEBwwFSGFub2kxEDAOBgNVBAoM\n"
"B0tpZW5Jb1QxCzAJBgNVBAsMAklUMRUwEwYDVQQDDAwxOTIuMTY4LjAuMjIwggEi\n"
"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCDxwvlkSWnUS55UDY55hHs5agE\n"
"r2gBltzjtJacp/PdZsYsC/qTnFSIRCjObCjI3zClWUU84oKlsJ+IXEZks4Q/EQ8i\n"
"st5RhBrZ+E8gIgwuXPa9ptb1LH1kZyQF/em8AEK+QGwFAt8KP90uStSgvWoVoMr1\n"
"Foz4XHwTh84vnFEkHufpB8kubyDaVgQ5dtsA9yR0IQLVpjhV7PYRg5fKyFHnxpk8\n"
"hKyXpydOtUMd069aWY9W7zKJNXa3iBq0h982NUKcYjk6aBYBM7AEG9rnfRhI1HU5\n"
"lvvGqA3UF2DC74oDi85MjMpvlR3pBJB0DLRdgOKB/Gd3++DohM/oPmY5qjIhAgMB\n"
"AAGjUzBRMDAGA1UdEQQpMCeHBMCoABaHBMCoXiiCCWxvY2FsaG9zdIIObXlkZXZp\n"
"Y2UubG9jYWwwHQYDVR0OBBYEFOOd/gzWtsicA3wb1oDyWDr5xR+RMA0GCSqGSIb3\n"
"DQEBCwUAA4IBAQBz0+y5h9HTibfAqvVFrA09VcEWqz+/57v9hnWLGW8pObsgMrgJ\n"
"wI7GEC8U66Y7gzkLtqYi+yNHr0BnAd1YijieG7XesFHm+ZVW/gNvK4RjJNC9yGAl\n"
"FnlclqJ0SC56szXh+WLVGQLp1rQ0lmka9lx1HsPutgDYO7KgodrgOOCTkraJKxqn\n"
"DKPp9ZCIk/KHQ8KxjGj/aNI9g4U86+qLe8HHeD/FOgyiM66fpDTOu2P3bgmehPtG\n"
"hqETq+QgBZ+rZQSpvRa4hmJgUV5Ygf2uFSh8DBu47df6M9jbN/pi7SPUXxCVVfqO\n"
"EQKuFNny5Rp5LGxMUvMc6gVg52J9MeCL7CZ7\n"
"-----END CERTIFICATE-----\n";
// 🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜


// Tạo đối tượng BlynkTimer để quản lý các bộ định thời
BlynkTimer timer;   

// Cấu hình NTP Server 
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;     // Múi giờ GMT+7
const int   daylightOffset_sec = 0;
void setupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


// ✨✨✨✨✨✨✨✨✨✨ HÀM SETUP ✨✨✨✨✨✨✨✨✨✨
void setup() {
  // 🎛️ Khởi tạo Serial Monitor
  delay(100);
  Serial.begin(115200);

  // 🛜 Kết nối Wifi
  setup_wifi(); 

  // ✨ Thiết lập ngày, giờ
  setupTime();

  // 🛜 Cấu hình MQTT client
  // wifiClient.setInsecure();       // Dùng cho HiveMQ cloud
  // secureClient.setInsecure();     // Không bảo mật
  secureClient.setCACert(ca_cert);   // Sử dụng chứng chỉ từ Broker
  client.setServer(mqtt_server, mqtt_port);

  // 🛜 Kết nối với Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, BLYNK_SERVER_IP, BLYNK_SERVER_PORT);

  // 🛜 Kết nối đến MQTT Broker
  reconnect_to_mqtt();

  // 🎛️ Khởi tạo Flame Sensor
  pinMode(FLAME_DIGITAL_PIN, INPUT);
  flameStatus.on();
  Blynk.setProperty(FLAME_LED_STATUS_VP, "color", COLOR_GREEN);

  // 🎛️ Khởi tạo PIR Sensor
  pinMode(HCSR501_DIGITAL_PIN, INPUT);
  pirStatus.on();
  Blynk.setProperty(PIR_LED_STATUS_VP, "color", COLOR_GREEN);

  // 🎛️ Khởi tạo cảm biến RFID-RC522
  // UID = 3D 67 36 2
  SPI.begin(4, 5, 6);   // SCK, MISO, MOSI theo chuẩn nanoESP32-C6
  mfrc522.PCD_Init();
  rfidStatus.on();
  Blynk.setProperty(RFID_LED_STATUS_VP, "color", COLOR_GREEN);
  
  // 🎛️ Khởi tạo cảm biến DHT và khởi tạo trạng thái hoạt động
  dht.begin();
  dhtStatus.on();
  Blynk.setProperty(DHT_LED_STATUS_VP, "color", COLOR_GREEN);

  // 🎛️ Khởi tạo cảm biến MQ-7
  setup_mq7_sensor();

  // 🎛️ Khởi tạo cảm biến MQ-2
  setup_mq2_sensor();

  Serial.println("🚀 Lets go");
  
  // ♻️Thiết lập bộ định thời để gửi dữ liệu cảm biến mỗi 1 giây
  timer.setInterval(5000L, send_temp_humi); //5s
  timer.setInterval(5000L, send_lcd_message);
  timer.setInterval(2000L, send_uid_rc522);
  timer.setInterval(10000L, send_CO_ppm);  //10s
  timer.setInterval(5000L, send_GAS_ppm);  //5s
  timer.setInterval(2000L, send_flame_command);
  timer.setInterval(1000L, send_pir_command);
}
// ✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨


// 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀  HÀM LOOP 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀
void loop() {

  Blynk.run();            // Xử lý các sự kiện từ Blynk
  timer.run();           // Chạy các bộ định thời đã cài đặt (định kỳ gửi dữ liệu cảm biến)
  client.loop();        // Chạy các tác vụ MQTT

  // Kiểm tra kết nối Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost connection. Attempting to reconnect...");
    WiFi.reconnect();  // Thử kết nối lại Wi-Fi nếu bị mất kết nối
    delay(5000);  // Chờ 5 giây trước khi thử lại
  }

  // Kiểm tra kết nối Blynk
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();
    // Kiểm tra liên tục mỗi 5 giây
    if(currentMillis - previousConnectBlynkAttempt >= 5000){
      if(!Blynk.connected()){
        Serial.println("Blynk lost connection. Attempting to reconnect...");
        Blynk.connect();  // Thử kết nối lại với Blynk
      }
      previousConnectBlynkAttempt = currentMillis;
    }
  }

  // Kiểm tra kết nối MQTT và tự động kết nối lại nếu mất kết nối
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();
    // Kiểm tra liên tục mỗi 5 giây
    if(currentMillis - previousConnectMQTTAttempt >= 5000){
      if(!client.connected()){
        Serial.println("MQTT lost connection. Attempting to reconnect...");
        reconnect_to_mqtt();  // Thử kết nối lại MQTT nếu bị mất kết nối
      }
      previousConnectMQTTAttempt = currentMillis;
    }
  }
}
// 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀


// 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐 HÀM ĐỒNG BỘ DỮ LIỆU VIRTUAL PINS  🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐
// Hàm đồng bộ hóa chân ảo khi kết nối với Blynk
BLYNK_CONNECTED() {
  // Đồng bộ hóa chân ảo (V1, V2, V3)
  Blynk.syncVirtual(V0, V1, V2, V3, V4, V5, V6, V7, V10, V11, V12, V13, V14, V15);  
  Serial.println("Blynk synced!");
}
// 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐


// 📝📝📝📝📝📝📝📝📝📝 HÀM TỰ ĐỊNH NGHĨA 📝📝📝📝📝📝📝📝📝📝📝📝📝
// Hàm kết nối Wifi
void setup_wifi() {
  delay(500);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {  // Kiểm tra trạng thái Wi-Fi
    delay(500);
    Serial.print(".");  // In dấu chấm trong khi kết nối
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  // In địa chỉ IP sau khi kết nối
}

// Hàm kết nối đến MQTT Broker với Client là ESP32-C6
void reconnect_to_mqtt() {
  Serial.print("Connecting to MQTT...");
  if (client.connect("ESP32C6Client", mqtt_user, mqtt_pass)) {
    Serial.println("connected");
  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
}

// Hàm lấy ngày, giờ
String get_current_datetime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "00-00-0000 00:00:00";   // Trả về chuỗi mặc định nếu lỗi NTP
  }

  char timeString[20];
  strftime(timeString, sizeof(timeString), "%d-%m-%Y %H:%M:%S", &timeinfo);
  return String(timeString);
}

// 📤 Hàm dùng chung để publish dữ liệu cảm biến lên MQTT Broker
void publish_message_to_broker(const char* topic, const char* sensor, int value) {
  // 1️. Xóa dữ liệu cũ trong StaticJsonDocument để tránh xung đột
  jsonDoc.clear();

  // 2️. Tạo cấu trúc JSON với 3 trường: sensor, value, timestamp
  jsonDoc["sensor"] = sensor;                    // Tên cảm biến
  jsonDoc["value"] = value;                      // Giá trị đo được
  jsonDoc["timestamp"] = get_current_datetime();   // Thời gian hiện tại

  // 3️. Chuyển đổi đối tượng JSON thành chuỗi để gửi đi
  size_t jsonLength = serializeJson(jsonDoc, jsonBuffer);

  // 4️. Kiểm tra quá trình serialize có thành công không
  if(jsonLength == 0){
    Serial.println("❌ Serialize JSON failed!");   // Thông báo lỗi nếu chuyển đổi thất bại
    return;
  }

  // 5️. Kiểm tra kết nối WiFi & MQTT trước khi gửi dữ liệu
  if(WiFi.status() == WL_CONNECTED && client.connected()){
    // Gửi chuỗi JSON lên MQTT Broker với retain flag = true
    client.publish(topic, (const uint8_t *)jsonBuffer, jsonLength, true);

    // Ghi log thành công lên Serial Monitor
    Serial.print("📝 Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker successfully!");
  } else {
    // Ghi log khi thất bại do mất kết nối
    Serial.print("⚠️ Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker failed!");
  }
}

void publish_message_to_broker(const char* topic, const char* sensor, float value) {
  // 1️. Xóa dữ liệu cũ trong StaticJsonDocument để tránh xung đột
  jsonDoc.clear();

  // 2️. Tạo cấu trúc JSON với 3 trường: sensor, value, timestamp
  jsonDoc["sensor"] = sensor;                    // Tên cảm biến
  jsonDoc["value"] = value;                      // Giá trị đo được
  jsonDoc["timestamp"] = get_current_datetime();   // Thời gian hiện tại

  // 3️. Chuyển đổi đối tượng JSON thành chuỗi để gửi đi
  size_t jsonLength = serializeJson(jsonDoc, jsonBuffer);

  // 4️. Kiểm tra quá trình serialize có thành công không
  if(jsonLength == 0){
    Serial.println("❌ Serialize JSON failed!");   // Thông báo lỗi nếu chuyển đổi thất bại
    return;
  }

  // 5️. Kiểm tra kết nối WiFi & MQTT trước khi gửi dữ liệu
  if(WiFi.status() == WL_CONNECTED && client.connected()){
    // Gửi chuỗi JSON lên MQTT Broker với retain flag = true
    client.publish(topic, (const uint8_t *)jsonBuffer, jsonLength, true);

    // Ghi log thành công lên Serial Monitor
    Serial.print("📝 Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker successfully!");
  } else {
    // Ghi log khi thất bại do mất kết nối
    Serial.print("⚠️ Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker failed!");
  }
}

void publish_message_to_broker(const char* topic, const char* sensor, String value) {
  // 1️. Xóa dữ liệu cũ trong StaticJsonDocument để tránh xung đột
  jsonDoc.clear();

  // 2️. Tạo cấu trúc JSON với 3 trường: sensor, value, timestamp
  jsonDoc["sensor"] = sensor;                    // Tên cảm biến
  jsonDoc["value"] = value;                      // Giá trị đo được
  jsonDoc["timestamp"] = get_current_datetime();   // Thời gian hiện tại

  // 3️. Chuyển đổi đối tượng JSON thành chuỗi để gửi đi
  size_t jsonLength = serializeJson(jsonDoc, jsonBuffer);

  // 4️. Kiểm tra quá trình serialize có thành công không
  if(jsonLength == 0){
    Serial.println("❌ Serialize JSON failed!");   // Thông báo lỗi nếu chuyển đổi thất bại
    return;
  }

  // 5️. Kiểm tra kết nối WiFi & MQTT trước khi gửi dữ liệu
  if(WiFi.status() == WL_CONNECTED && client.connected()){
    // Gửi chuỗi JSON lên MQTT Broker với retain flag = true
    client.publish(topic, (const uint8_t *)jsonBuffer, jsonLength, true);

    // Ghi log thành công lên Serial Monitor
    Serial.print("📝 Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker successfully!");
  } else {
    // Ghi log khi thất bại do mất kết nối
    Serial.print("⚠️ Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker failed!");
  }
}

// 📤 Hàm publish dữ liệu cảm biến DHT11 lên MQTT Broker
void publish_message_dht_to_broker(const char* topic, const char* sensor, float temp, float humi) {
  // 1️. Xóa dữ liệu cũ trong StaticJsonDocument để tránh xung đột
  jsonDoc.clear();

  // 2️. Tạo cấu trúc JSON với 3 trường: sensor, values, timestamp
  jsonDoc["sensor"] = sensor;
  JsonObject values = jsonDoc.createNestedObject("values");
  values["temperature"] = temp;
  values["humidity"] = humi;
  jsonDoc["timestamp"] = get_current_datetime();

  // 3️. Chuyển đổi đối tượng JSON thành chuỗi để gửi đi
  size_t jsonLength = serializeJson(jsonDoc, jsonBuffer);

  // 4️. Kiểm tra quá trình serialize có thành công không
  if(jsonLength == 0){
    Serial.println("❌ Serialize JSON failed!");   // Thông báo lỗi nếu chuyển đổi thất bại
    return;
  }

  // 5️. Kiểm tra kết nối WiFi & MQTT trước khi gửi dữ liệu
  if(WiFi.status() == WL_CONNECTED && client.connected()){
    // Gửi chuỗi JSON lên MQTT Broker với retain flag = true
    client.publish(topic, (const uint8_t *)jsonBuffer, jsonLength, true);

    // Ghi log thành công lên Serial Monitor
    Serial.print("📝 Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker successfully!");
  } else {
    // Ghi log khi thất bại do mất kết nối
    Serial.print("⚠️ Publish ");
    Serial.print(jsonBuffer);
    Serial.println(" to Broker failed!");
  }
}

// Hàm hiển thị doạn văn lên LDC Widget
void send_lcd_message(){
  if(isShowMess){
    Blynk.virtualWrite(LCD_VALUE_LINE1_VP, "Hello, Mr.Kien");
    Blynk.virtualWrite(LCD_VALUE_LINE2_VP, "What's up?");
  }else{
    Blynk.virtualWrite(LCD_VALUE_LINE1_VP, "My smart home");
    Blynk.virtualWrite(LCD_VALUE_LINE2_VP, "Have a nice day!");
  }
  isShowMess = !isShowMess;
}

// Hàm gửi tín hiệu cảm biến lửa
void send_flame_command(){
  int value = digitalRead(FLAME_DIGITAL_PIN) == LOW ? 1 : 0;
  // GỬi lên Blynk Server
  if(WiFi.status() == WL_CONNECTED && Blynk.connected()){
    Blynk.virtualWrite(FLAME_VALUE_DIGI_VP, value);
  }else{
    Serial.print("🔄Publish fire status to Blynk failed!");
  }
  // Gửi lên MQTT Broker
  publish_message_to_broker(FLAME_TOPIC, "Flame sensor", value);
}

// Hàm gửi tín hiệu cảm biến PIR
void send_pir_command(){
  int value = digitalRead(HCSR501_DIGITAL_PIN);
  publish_message_to_broker(PIR_TOPIC, "PIR sensor", value);
}

// Hàm khởi tạo cảm biến MQ-7
void setup_mq7_sensor(){
  // === Thiết lập công thức tính ppm ===
  MQ7.setRegressionMethod(1);      // _PPM = a*ratio^b
  MQ7.setA(99.042);                // Cấu hình phương trình để tính nồng độ khí CO
  MQ7.setB(-1.518);
  /*
    Exponential regression:
  GAS     | a      | b
  H2      | 69.014  | -1.374
  LPG     | 700000000 | -7.703
  CH4     | 60000000000000 | -10.54
  CO      | 99.042 | -1.518
  Alcohol | 40000000000000000 | -12.35
  */

  // === Khởi tạo cảm biến ===
  MQ7.init();
  mq7Status.on();   // Trạng thái đang hoạt động trên app
  Blynk.setProperty(MQ7_LED_STATUS_VP, "color", COLOR_GREEN);

  MQ7.setRL(10);                   // Điện trở tải 10k ôm

  Serial.print("⏳ Đang hiệu chuẩn MQ-7 (môi trường không khí sạch)...");
  float r0 = 0;
  for (int i = 0; i < 10; i++) {
    MQ7.update();
    r0 += MQ7.calibrate(RatioMQ7CleanAir);
    Serial.print(".");
  }
  r0 /= 10;

  if (isinf(r0) || r0 == 0) {
    Serial.print("Lỗi hiệu chuẩn R0. Sử dụng R0 mặc định (10.0 kΩ)");
    MQ7.setR0(10.0); // Gán giá trị mặc định để demo tiếp
  }else{
    MQ7.setR0(r0);
    Serial.println("hoàn thành.");
  }
}

// Hàm đọc giá trị khí CO ppm
void send_CO_ppm() {
  // Đọc cảm biến
  MQ7.update();
  float co_ppm = MQ7.readSensor();

  // // Kiểm tra lỗi
  // if (isnan(co_ppm) || co_ppm < 0.1) {
  //   Serial.println("⚠️Lỗi đọc cảm biến MQ-7");
  //   if(!isMQ7Error){
  //     Blynk.setProperty(MQ7_LED_STATUS_VP, "color", COLOR_RED);
  //     isMQ7Error = true;
  //   }
  //   return;
  // }

  // // Nếu còn hoạt động
  // if(isMQ7Error){
  //   Blynk.setProperty(MQ7_LED_STATUS_VP, "color", COLOR_GREEN);
  //   isMQ7Error = false;
  // }

  // Gửi dữ liệu CO lên chân ảo của Blynk
  if(WiFi.status() == WL_CONNECTED && Blynk.connected()){
    Blynk.virtualWrite(MQ7_VALUE_PPM_VP, String(co_ppm, 2));
  }else{
    Serial.print("🔄Publish CO gas to Blynk failed!");
  }
  // Gửi lên MQTT Broker
  publish_message_to_broker(MQ7_TOPIC, "MQ-7", co_ppm);
}

// Hàm khởi tạo cảm biến MQ-2
void setup_mq2_sensor(){
  // === Thiết lập công thức tính ppm ===
  MQ2.setRegressionMethod(1);           // PPM = a * ratio^b

  // Đo LPG
  MQ2.setA(574.25); MQ2.setB(-2.222);

  /*
    Exponential regression:
    Gas     | a       | b
    H2      | 987.99  | -2.162
    LPG     | 574.25  | -2.222
    CO      | 36974   | -3.109
    Alcohol | 3616.1  | -2.675
    Propane | 658.71  | -2.168
    Smoke   | 4257.1  | -2.23   // Tuỳ biến
  */

  // === Khởi tạo cảm biến ===
  MQ2.init();
  mq2Status.on();   // Trạng thái đang hoạt động trên app
  Blynk.setProperty(MQ2_LED_STATUS_VP, "color", COLOR_GREEN);

  MQ2.setRL(10.0);  // nếu chắc chắn module MQ-2 dùng điện trở tải 10kΩ

  // ==== Hiệu chuẩn cảm biến ====
  Serial.print("⏳ Đang hiệu chuẩn MQ-2 (môi trường không khí sạch)...");
  float r0 = 0;
  for (int i = 0; i < 10; i++) {
    MQ2.update();
    r0 += MQ2.calibrate(RatioMQ2CleanAir);
    Serial.print(".");
  }
  r0 /= 10;

  if (isinf(r0) || r0 == 0) {
    Serial.print("Lỗi hiệu chuẩn R0. Sử dụng R0 mặc định (10.0 kΩ)");
    MQ2.setR0(10.0);
  } else {
    MQ2.setR0(r0);
    Serial.println("hoàn thành.");
  }
}

// Hàm đọc giá trị khí Gas ppm
void send_GAS_ppm() {
  // Đọc cảm biến
  MQ2.update();
  float gas_ppm = MQ2.readSensor();

  // // Kiểm tra lỗi
  // if (isnan(gas_ppm) || gas_ppm < 0.1) {
  //   Serial.println("⚠️Lỗi đọc cảm biến MQ-2");
  //   if(!isMQ2Error){
  //     Blynk.setProperty(MQ2_LED_STATUS_VP, "color", COLOR_RED);
  //     isMQ2Error = true;
  //   }
  //   return;
  // }

  // // Nếu còn hoạt động
  // if(isMQ2Error){
  //   Blynk.setProperty(MQ2_LED_STATUS_VP, "color", COLOR_GREEN);
  //   isMQ2Error = false;
  // }

  // Gửi dữ liệu CO lên chân ảo của Blynk
  if(WiFi.status() == WL_CONNECTED && Blynk.connected()){
    Blynk.virtualWrite(MQ2_VALUE_PPM_VP, String(gas_ppm, 2));
  }else{
    Serial.print("🔄Publish CO gas to Blynk failed!");
  }
  // Gửi lên MQTT Broker
  publish_message_to_broker(MQ2_TOPIC, "MQ-2", gas_ppm);
}

// Hàm kiểm tra RC522 có phản hồi không
bool check_rfid_status() {
  // Thử gửi lệnh kiểm tra phiên bản firmware
  byte version = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  if (version == 0x00 || version == 0xFF) {
    return false;  // Lỗi cảm biến
  } else {
    return true;   // Cảm biến OK
  }
}

// Hàm đọc thẻ RFID, trả về UID dạng String hoặc "" nếu không có thẻ
String readRFID_RC522() {
  if (!mfrc522.PICC_IsNewCardPresent()) return "";
  if (!mfrc522.PICC_ReadCardSerial()) return "";

  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
    if (i != mfrc522.uid.size - 1) uidStr += " ";
  }
  uidStr.toUpperCase();
  return uidStr;
}

void send_uid_rc522(){
  // Nếu RFID không còn hoạt động
  if(!check_rfid_status()){
    Serial.println("⚠️Đọc cảm biến RFID-RC522 thất bại!"); // In thông báo lỗi nếu không đọc được
    if(!isRFIDError){
      Blynk.setProperty(RFID_LED_STATUS_VP, "color", COLOR_RED);
      isRFIDError = true;
    }
    return;
  }
  
  String uid = readRFID_RC522();
  if(isRFIDError){
    Blynk.setProperty(RFID_LED_STATUS_VP, "color", COLOR_GREEN);
    isRFIDError = false;
  }
  // Gửi lên MQTT Broker
  if(lastRFIDState != uid){
    publish_message_to_broker(RC522_TOPIC, "RFID-RC522", uid);
    lastRFIDState = uid;
  }

}

// Hàm gửi dữ liệu nhiệt độ và độ ẩm
void send_temp_humi() {
  // Đọc giá trị từ cảm biến DHT11
  float celsius = dht.readTemperature();
  float fahrenheit = dht.readTemperature(true);
  float humidity = dht.readHumidity();

  // Kiểm tra nếu cảm biến không trả về giá trị hợp lệ
  if (isnan(celsius) || isnan(fahrenheit) || isnan(humidity)) {
    Serial.println("⚠️Đọc cảm biến DHT 11 thất bại!"); // In thông báo lỗi nếu không đọc được
    if(!isDHTError){
      // Gửi Topic trạng thái cảm biến DHT11
      publish_message_to_broker(DHT11_STATUS_TOPIC, "DHT11 status", "error");
      // Chuyển đèn trnajg thái trên app sang màu đỏ
      Blynk.setProperty(DHT_LED_STATUS_VP, "color", COLOR_RED);
      isDHTError = true;
    }
    Blynk.virtualWrite(DHT_VALUE_CEL_VP, "nan");     // Gửi NaN lên Virtual Pin V3
    Blynk.virtualWrite(DHT_VALUE_HUMI_VP, "nan");     // Gửi NaN lên Virtual Pin V4
    Blynk.virtualWrite(DHT_VALUE_Fah_VP, "nan");     // Gửi NaN lên Virtual Pin V5
    return;
  }
  // Trạng thái hoạt động của cảm biến DHT11
  if(isDHTError){
    // Gửi Topic trạng thái cảm biến DHT11
      publish_message_to_broker(DHT11_STATUS_TOPIC, "DHT11 status", "normal");
      // Chuyển đèn trnajg thái trên app sang màu xanh
    Blynk.setProperty(DHT_LED_STATUS_VP, "color", COLOR_GREEN);
    isDHTError = false;
  }

  // Gửi dữ liệu nhiệt độ và độ ẩm lên các chân ảo của Blynk
  if(WiFi.status() == WL_CONNECTED && Blynk.connected()){
    Blynk.virtualWrite(DHT_VALUE_CEL_VP, String(celsius, 2));  // Gửi nhiệt độ C lên Virtual Pin V3
    Blynk.virtualWrite(DHT_VALUE_HUMI_VP, String(humidity, 2));     // Gửi độ ẩm lên Virtual Pin V4
    Blynk.virtualWrite(DHT_VALUE_Fah_VP, String(fahrenheit, 2));     // Gửi nhiệt độ F lên Virtual Pin V5
  }else{
    Serial.print("🔄Publish temp-humi to Blynk failed!");
  }

  // Gửi lên MQTT Broker
  publish_message_dht_to_broker(DHT11_TOPIC, "DHT11", celsius, humidity);
}
// 📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝
