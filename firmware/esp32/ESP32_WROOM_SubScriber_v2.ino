#define IP_LOCAL_HOST "192.168.94.40"
// #define IP_LOCAL_HOST "192.168.0.22"
// 🚀🚀🚀🚀🚀🚀🚀🚀 KHAI BÁO BLYNK LOCAL SERVER 🚀🚀🚀🚀🚀🚀🚀🚀
// #define BLYNK_TEMPLATE_ID "TMPL6bBp0Lwdq"         
// #define BLYNK_TEMPLATE_NAME "Smart House"       
#define BLYNK_AUTH_TOKEN "TATin2iAt40gLapsSG22f9-agy9mJZrp"
#define BLYNK_SERVER_IP IP_LOCAL_HOST
// #define BLYNK_SERVER_IP "192.168.94.40"
#define BLYNK_SERVER_PORT 8080
#define BLYNK_PRINT Serial // Kích hoạt việc in thông tin Blynk trên Serial Monitor
// 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀


// ⚡⚡⚡⚡⚡⚡⚡ KHAI BÁO CÁC THƯ VIỆN ⚡⚡⚡⚡⚡⚡⚡
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
// ⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡


// 🌟🌟🌟🌟🌟🌟🌟 DANH SÁCH TOPIC 🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟
#define DHT11_TOPIC "home/sensor/dht11/temp-humi"
#define MQ7_TOPIC "home/sensor/mq-7/co"
#define MQ2_TOPIC "home/sensor/mq-2/gas"
#define RC522_TOPIC "home/sensor/rc522/rfid"
#define FLAME_TOPIC "home/sensor/flame/command"
#define PIR_TOPIC "home/sensor/hc-sr501/command"

// 🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟🌟


// 🌐🌐🌐🌐🌐🌐🌐🌐🌐 BIẾN TOÀN CỤC 🌐🌐🌐🌐🌐🌐🌐🌐🌐
// Password đúng và Uid đúng
#define PASSWORD_VALID "11122003"
#define UID_VALID "3D 67 36 2"

// Biến lưu trữ giá trị nhận độ độ ẩm nhận được từ Topic
float temp = 0;
float humi = 0;

// Biến lưu trữ giá trị khí CO và Gas
float coVal = 0;
float gasVal = 0;

// Biến lưu mật khẩu người dùng nhập
String myPass = "";

// Biến lưu UID từ thẻ từ
String uid = "";

// Biến lưu trạng thái flame sensor
// int fireState = 0;

// Biến lưu trạng thái cảm biến PIR
int pirState = 0;
unsigned long lastMotionTime = 0;    // Thời điểm cuối cùng phát hiện chuyển động
bool isRelayHallwayOn = false;       // Đèn hành lang có bật không
bool isHallwayAuto = false;          // Đèn hành lang tự động khi phát hiện chuyển động

// Biến kiểm soát trạng thái và thời gian gửi tin nhắn Telegram
unsigned long lastAlertHumi = 0;
bool isSentAlertHumi = false;
unsigned long lastAlertTemp = 0;
bool isSentAlertTemp = false;

// Biến kiểm soát trạng thái và thời gian gửi tin nhắn lên Telegram
unsigned long lastAlertGas = 0;
bool isSentAlertGas = false;

bool isHeatLampAuto = false;         // Đèn sưởi tự động khi nhiệt độ cao

// Biến kiểm tra kết nối MQTT và Blynk
unsigned long previousConnectMQTTAttempt = 0;
unsigned long previousConnectBlynkAttempt = 0;
// 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐


// ✨✨✨✨✨✨✨✨✨✨ KHAI BÁO GPIO PINS ✨✨✨✨✨✨✨✨✨
// 🎚️ Virtual Pins🎚️
#define LCD_GATE_LINE1_VP 0               // V0 - Dòng 1 LCD Widget trên Blynk app
#define LCD_GATE_LINE2_VP 1               // V1 - Dòng 2 LCD Widget trên Blynk app
//                                        // V2 - Nhập mật khẩu cổng nhà trên Blynk app
//                                        // V3 - Đóng mở cổng thủ công trên Blynk app
//                                        // V4 - Đèn hành lang trên Blynk app
//                                        // V5 - Chế độ đèn hành lang thông minh trên Blynk app
//                                        // V6 - Bật/tắt quạt trên Blynk app
//                                        // V7 - Điều chỉnh đèn sưởi trên Blynk app
//                                        // V8 - Đèn sưởi tự động trên Blynk app
#define LCD_TEMP_HUMI_LINE1_VP 9          // V9 - Dòng 1 LCD Widget trạng thái nhiệt độ trên Blynk app
#define LCD_TEMP_HUMI_LINE2_VP 10         // V10 - Dòng 2 LCD Widget trạng thái độ ẩm trên Blynk app
#define LCD_CO_GAS_LINE1_VP    11         // V11 - Dòng 1 LCD Widget trạng thái khí CO trên Blynk app
#define LCD_CO_GAS_LINE2_VP    12         // V12 - Dòng 2 LCD Widget trạng thái khí Gas trên Blynk app


// 🎚️End Virtual Pins 🎚️

// 📶 GPIO Pins
#define GATE_GREEN_LED_PIN     13               // GPIO13 - Led xanh lá của cổng nhà
#define GATE_RED_LED_PIN       12               // GPIO12 - Led đỏ của cổng nhà
#define RELAY_HALLWAY_LAMP_PIN 15               // GPIO15 - IN1 relay bật đèn hành lang
// #define RELAY_FAN_PIN 2                      // GPIO2 - IN2 relay bật quạt trong phòng
// #define L298N_LAMP_ENB 4                     // GPIO4 - Bật đèn sưởi khi độ ẩm cao
#define RELAY_SIREN_PIN        5                // GPIO5 - IN3 relay bật còi báo động
#define SERVO_PIN              25               // GPIO25 - Động cơ Servo

// L298N - Điều khiển quạt làm mát và đèn sưởi bằng PWM
#define L298N_FAN_IN1    33                     // GPIO33: Điều khiển chiều quay quạt
#define L298N_FAN_ENA    26                     // GPIO26: Điều chỉnh tốc độ quạt bằng PWM (ENA)
#define L298N_LAMP_IN3   32                     // GPIO32: Điều khiển trạng thái đèn sưởi
#define L298N_LAMP_ENB   27                     // GPIO27: Điều chỉnh độ sáng đèn bằng PWM (ENB)
// 📶End GPIO Pins
// ✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨


// 🛜🛜🛜🛜🛜🛜🛜🛜🛜 CONNECT INFO 🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜
// Wifi infor
// const char* ssid = "Tầng 3";
// const char* pass = "13572468";
const char* ssid = "Wifi vip";
const char* pass = "11122003";

// My bot discord
String discordBotToken = "MTM3Njc2MjYzMTA2NjYxNTg0MA.GAdtQc.SpgAHiFIBgruTS_jXgB1X1KBCCxIt7-guaaQtM";       // Token bot Discord
String discordTempHumiChannelId = "1376770567788695685";                                                   // ID kênh cảnh báo nhiệt độ, độ ẩm
String discordGasChannelId = "1376924907404787804";                                                        // ID kênh cảnh báo khí gas

// MQTT HiveMQ Cloud
// const char* mqtt_server = "db6c39fa55a94251b9941e77a4756216.s1.eu.hivemq.cloud";
// const int mqtt_port = 8883;
// const char* mqtt_user = "kienbui12003";
// const char* mqtt_pass = "Kien!!!2";
// WiFiClientSecure wifiClient;
// PubSubClient client(wifiClient);

// MQTT HiveMQ Local Server
const char* mqtt_server = IP_LOCAL_HOST;
const int mqtt_port = 8883;
const char* mqtt_user = "user_subscribe";
const char* mqtt_pass = "kien2";
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
// BlynkTimer timer;           
Servo myServo;

// ✨✨✨✨✨✨✨✨✨✨ HÀM SETUP ✨✨✨✨✨✨✨✨✨✨
void setup() {
  // Khởi tạo Serial Monitor
  delay(100);
  Serial.begin(9600);

  // THiết lập nội dung hiển thị trên LCD
  // Blynk.virtualWrite(LCD_GATE_LINE1_VP, "Pass: False");
  // Blynk.virtualWrite(LCD_GATE_LINE2_VP, "Gate: Closed");

  // Thiết lập relay điều khiển đèn hành lang
  pinMode(RELAY_HALLWAY_LAMP_PIN, OUTPUT);
  digitalWrite(RELAY_HALLWAY_LAMP_PIN, LOW);

  // Thiết lập relay bật còi báo động
  pinMode(RELAY_SIREN_PIN, OUTPUT);
  digitalWrite(RELAY_SIREN_PIN, LOW); 

  // Thiết lập led trạng thái cổng
  pinMode(GATE_GREEN_LED_PIN, OUTPUT);
  pinMode(GATE_RED_LED_PIN, OUTPUT);
  // digitalWrite(GATE_GREEN_LED_PIN, LOW);
  // digitalWrite(GATE_RED_LED_PIN, HIGH);

  // Thiết lập L298N điều khiển quạt và đèn
  // Cấu hính chân ra
  pinMode(L298N_FAN_IN1, OUTPUT);
  pinMode(L298N_LAMP_IN3, OUTPUT);
  pinMode(L298N_FAN_ENA, OUTPUT);
  pinMode(L298N_LAMP_ENB, OUTPUT);
  // Bật chiều điện đúng hướng (OUT1 và OUT3 sẽ có điện)
  digitalWrite(L298N_FAN_IN1, HIGH);      // Quạt sẵn sàng quay
  digitalWrite(L298N_LAMP_IN3, HIGH);     // Đèn sẵn sàng sáng
  // Gán giá trị PWM ban đầu (tắt quạt và đèn)
  analogWrite(L298N_FAN_ENA, 0);          // Quạt tắt (0/255)
  analogWrite(L298N_LAMP_ENB, 0);         // Đèn tắt (0/255)

  // 🛜 Kết nối Wifi
  setup_wifi();

  // 🛜 Cấu hình MQTT Client
  // wifiClient.setInsecure();       // Dùng cho HiveMQ cloud
  // secureClient.setInsecure();     // Không bảo mật
  secureClient.setCACert(ca_cert);   // Sử dụng chứng chỉ từ Broker
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);  // Đặt hàm callback để xử lý tin nhắn đến

  // 🛜 Kết nối với Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, BLYNK_SERVER_IP, BLYNK_SERVER_PORT);

  // 🛜 Kết nối MQTT Broker
  reconnectMQTT();

  myServo.attach(SERVO_PIN); 

  // THiết lập nội dung hiển thị trên LCD
  Blynk.virtualWrite(LCD_TEMP_HUMI_LINE1_VP, "Normal");
  Blynk.virtualWrite(LCD_TEMP_HUMI_LINE2_VP, "Normal");

  // THiết lập nội dung hiển thị trên LCD
  Blynk.virtualWrite(LCD_CO_GAS_LINE1_VP, "Safe");
  Blynk.virtualWrite(LCD_CO_GAS_LINE2_VP, "Safe");

  // Thiết lập bộ định thời để gửi dữ liệu cảm biến mỗi 1 giây
  // timer.setInterval(2000L, controlHeatLampAutoMode);
  // timer.setInterval(2000L, checkHumidity);
  // timer.setInterval(2000L, checkTemperature);
  // timer.setInterval(500L, check_pir_state);
  // timer.setInterval(1000L, check_uid_command);
  // // timer.setInterval(1000L, checkFireState);
  // timer.setInterval(2000L, checkGas_CO);

}
// ✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨


// 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀  HÀM LOOP 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀
void loop() {
  Blynk.run();
  // timer.run();
  client.loop();  // Xử lý các sự kiện MQTT

  // Kiểm tra kết nối Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost connection. Attempting to reconnect...");
    WiFi.reconnect();  // Thử kết nối lại Wi-Fi nếu bị mất kết nối
    delay(5000);  // Chờ 5 giây trước khi thử lại
  }

  // Kiểm tra kết nối Blynk
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();
    // Chờ 5 giây
    if(currentMillis - previousConnectBlynkAttempt >= 5000){
      if(!Blynk.connected()){
        Serial.println("Blynk lost connection. Attempting to reconnect...");
        Blynk.connect();  // Thử kết nối lại với Blynk
      }
      previousConnectBlynkAttempt = currentMillis;
    }
  }

  // Kiểm tra kết nối MQTT Broker
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();
    if(currentMillis - previousConnectMQTTAttempt >= 5000){
      if(!client.connected()){
        Serial.println("MQTT lost connection. Attempting to reconnect...");
        reconnectMQTT();  // Thử kết nối lại MQTT nếu bị mất kết nối
      }
      previousConnectMQTTAttempt = currentMillis;
    }
  }
}
// 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀


// 🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜  BLYNK VIRTUAL CONTROL 🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜
// Hàm xử lý mật khẩu cổng từ Blynk (Virtual Pin V2)
BLYNK_WRITE(V2){
  myPass = param.asStr();   // Đọc giá trị từ chân V2 trên Blynk
  // Nếu mật khẩu đúng thì hiển thị lên LCD
  if(myPass.equals(PASSWORD_VALID)){
    Blynk.virtualWrite(LCD_GATE_LINE1_VP, "Pass: True");
  }else{
    Blynk.virtualWrite(LCD_GATE_LINE1_VP, "Pass: False");
  }
  // Đồng bộ hóa giá trị chân V3 từ Blynk
  Blynk.syncVirtual(V3);  
}

// Hàm xử lý lệnh đóng mở cổng từ Blynk (Virtual Pin V3)
BLYNK_WRITE(V3) {
  int relayState = param.asInt();  // Đọc giá trị từ Virtual Pin V3
  // Nếu trạng thái V3 là 1 và đúng password thì mở cổng
  if (relayState == 1 && myPass.equals(PASSWORD_VALID)) {
    myServo.write(90);
    digitalWrite(GATE_GREEN_LED_PIN, HIGH);
    digitalWrite(GATE_RED_LED_PIN, LOW);
    Blynk.virtualWrite(LCD_GATE_LINE2_VP, "Gate: Opening");
  } else {
    myServo.write(0);
    digitalWrite(GATE_GREEN_LED_PIN, LOW);
    digitalWrite(GATE_RED_LED_PIN, HIGH);
    Blynk.virtualWrite(LCD_GATE_LINE2_VP, "Gate: Closed");
  }
}

// Hàm xử lý bật đèn hành lang thủ công từ Blynk (Virtual Pin V4)
BLYNK_WRITE(V4){
  if(isHallwayAuto) return;
  int state = param.asInt();   // Đọc giá trị chân V4 trên Blynk
  if(state == 1){
    digitalWrite(RELAY_HALLWAY_LAMP_PIN, HIGH);
    isRelayHallwayOn = true;
  }else{
    digitalWrite(RELAY_HALLWAY_LAMP_PIN, LOW);
    isRelayHallwayOn = false;
  }
}

// Hàm xử lý kích hoạt đèn hành lang thông minh từ Blynk (Virtual Pin V5)
BLYNK_WRITE(V5){
  int state = param.asInt();   // Đọc giá trị chân V5 trên Blynk
  if(state == 1){
    isHallwayAuto = true;
  }else{
    isHallwayAuto = false;
    // Đồng bộ hóa với Blynk App
    // Blynk.syncVirtual(V4);

    // Ngắt relay và cập nhật chân V4 về 0
    // Blynk.virtualWrite(V4, 0);
    // digitalWrite(RELAY_HALLWAY_LAMP_PIN, LOW);
    // isRelayHallwayOn = false;
  }
}

// Hàm xử lý bật quạt thủ công từ Blynk (Virtual Pin V6)
BLYNK_WRITE(V6){
  int pwmValue  = param.asInt();   // Đọc giá trị chân V6 trên Blynk
  analogWrite(L298N_FAN_ENA, pwmValue);
}

// Hàm xử lý điều khiển đèn sưởi thủ công từ Blynk (Virtual Pin V7)
BLYNK_WRITE(V7){
  if(isHeatLampAuto) return;
  int pwmValue  = param.asInt();   // Đọc giá trị chân V7 trên Blynk
  analogWrite(L298N_LAMP_ENB, pwmValue);
}

// Hàm xử lý kích hoạt đèn sưởi thông minh từ Blynk (Virtual Pin V8)
BLYNK_WRITE(V8){
  int state = param.asInt();   // Đọc giá trị chân V8 trên Blynk
  if(state == 1){
    isHeatLampAuto = true;
  }else{
    isHeatLampAuto = false;
  }
}


// 🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜🛜


// 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐 HÀM ĐỒNG BỘ DỮ LIỆU VIRTUAL PINS  🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐
BLYNK_CONNECTED() {
  // Đồng bộ hóa chân ảo
  Blynk.syncVirtual(V2);              // Đồng bộ mật khẩu cổng
  delay(2000);
  // Blynk.syncVirtual(V3);              // Đồng bộ trạng thái cổng
  Blynk.syncVirtual(V4, V5, V6, V7, V8, V9, V10, V11, V12, V13);  
  Serial.println("Blynk synced!");
}
// 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐


// 📝📝📝📝📝📝📝📝📝📝 HÀM TỰ ĐỊNH NGHĨA 📝📝📝📝📝📝📝📝📝📝📝📝📝
// Hàm bật tắt đèn hành lang tự động
void check_pir_state(){
  if(!isHallwayAuto) return;
  if(pirState == 1){
    if(!isRelayHallwayOn){
      digitalWrite(RELAY_HALLWAY_LAMP_PIN, HIGH);
      Blynk.virtualWrite(V4, 1);
      isRelayHallwayOn = true;
    }
    lastMotionTime = millis();
  }else{
    if(isRelayHallwayOn){
      unsigned long currentMotionTime = millis();
      if(currentMotionTime - lastMotionTime >= 5000){
        digitalWrite(RELAY_HALLWAY_LAMP_PIN, LOW);
        Blynk.virtualWrite(V4, 0);
        isRelayHallwayOn = false;
      }
    }
  }
}

// Hàm kiểm tra thẻ từ mở cổng
void check_uid_command(){
  if(uid == ""){
    return;
  }
  if(uid == UID_VALID){
    Blynk.virtualWrite(V3, 1);
    Blynk.syncVirtual(V3);  
  }
}

// Hàm kiểm tra nhiệt độ
void checkTemperature(){
  if(temp == 0) return;
  if(temp>=35){
    if(!isSentAlertTemp){
      Blynk.virtualWrite(LCD_TEMP_HUMI_LINE1_VP, "High");
      sendDiscordMessage("🔥Cảnh báo nhiệt độ cao vượt mức quy định🔥", discordTempHumiChannelId);
      isSentAlertTemp = true;
      lastAlertTemp = millis();
    }else{
      unsigned long currentAlertTemp = millis();
      // Gửi lại sau 10 giây
      if(currentAlertTemp - lastAlertTemp >= 10000){
        isSentAlertTemp = false;
      }
    }
  }else{
    Blynk.virtualWrite(LCD_TEMP_HUMI_LINE1_VP, "Normal");
  }
}

// Hàm kiểm tra độ ẩm với chế độ tự động
void controlHeatLampAutoMode(){
  if(!isHeatLampAuto) return;
  if(humi == 0) return;
  if(humi >= 85){
    // Tăng đèn sưởi lên mức cao
    analogWrite(L298N_LAMP_ENB, 200);
    // Cập nhật giá trị lên chân V7
    Blynk.virtualWrite(V7, 200);
  }else if(humi >= 70){
    // Tăng đèn sưởi mức vừa
    analogWrite(L298N_LAMP_ENB, 100);
    // Cập nhật giá trị lên chân V7
    Blynk.virtualWrite(V7, 100);
  }else{
    // Giảm về 0
    analogWrite(L298N_LAMP_ENB, 0);
    Blynk.virtualWrite(V7, 0);
  }
}

// Hàm kiểm tra độ ẩm
void checkHumidity(){
  if(humi == 0) return;
  if(humi >= 85){
    if(!isSentAlertHumi){
      // Hiển thị LCD Blynk app
      Blynk.virtualWrite(LCD_TEMP_HUMI_LINE2_VP, "High");
      // Gửi cảnh báo
      sendDiscordMessage("🚨Cảnh báo độ ẩm cao vượt mức quy định🚨", discordTempHumiChannelId);
      isSentAlertHumi = true;
      lastAlertHumi = millis();
    }else{
      unsigned long currentAlertHumi = millis();
      // Gửi lại cảnh báo sau 10 giây
      if(currentAlertHumi - lastAlertHumi >= 10000){
        isSentAlertHumi = false;
      }
    }
  }else if(humi >= 70){
    // Hiển thị LCD Blynk app
    Blynk.virtualWrite(LCD_TEMP_HUMI_LINE2_VP, "Bit high");
  }else{
    Blynk.virtualWrite(LCD_TEMP_HUMI_LINE2_VP, "Normal");
  }
}

// Hàm gửi phân loại giá trị khí Gas 
void checkGasValue(){
  // Gas
  if(gasVal > 1000){
    unsigned long currentAlertGas = millis();
    // Nếu chưa gửi tin nhắn thì gửi
    if(!isSentAlertGas){
      // Gửi cảnh báo lên Telegram
      sendDiscordMessage("⚠️Cảnh báo: Nồng độ khí gas trong phòng bếp đang vượt ngưỡng quy định⚠️", discordGasChannelId);
      // Gửi thông báo lên LCD giao diện Blynk app
      Blynk.virtualWrite(LCD_CO_GAS_LINE1_VP, "Danger");
      // Bật còi
      digitalWrite(RELAY_SIREN_PIN, HIGH); 
      // Đánh dấu là đã gửi tin nhắn
      isSentAlertGas = true;
      lastAlertGas = currentAlertGas;
    }else{
      // Nếu đã gửi rồi thì 10 giây sau mới gửi tiếp    
      if(currentAlertGas - lastAlertGas >= 10000){
        isSentAlertGas = false;
      }
    }
  }else if(gasVal > 500){
    Blynk.virtualWrite(LCD_CO_GAS_LINE1_VP, "Alert");
  }else{
    // Tắt còi và gửi thông báo trạng thái đến LCD
    digitalWrite(RELAY_SIREN_PIN, LOW);
    Blynk.virtualWrite(LCD_CO_GAS_LINE1_VP, "Normal");
  }
  // CO
  if(coVal > 1000){
    Blynk.virtualWrite(LCD_CO_GAS_LINE2_VP, "Danger");
  }else if(coVal > 500){
    Blynk.virtualWrite(LCD_CO_GAS_LINE2_VP, "Alert");
  }else{
    Blynk.virtualWrite(LCD_CO_GAS_LINE2_VP, "Normal");
  }
}

// Hàm gửi phân loại giá trị khí CO
void checkCOValue(){
  // CO
  if(coVal > 1000){
    Blynk.virtualWrite(LCD_CO_GAS_LINE2_VP, "Danger");
  }else if(coVal > 500){
    Blynk.virtualWrite(LCD_CO_GAS_LINE2_VP, "Alert");
  }else{
    Blynk.virtualWrite(LCD_CO_GAS_LINE2_VP, "Normal");
  }
}

// // Hàm kiểm tra lửa
// void checkFireState(){
//   if(fireState == 1){
    
//   }else{
     
//   }
// }

// Hàm gửi tin nhắn Discord
void sendDiscordMessage(String message, String discordChannelId) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // API endpoint của Discord Bot
    String url = "https://discord.com/api/v10/channels/" + discordChannelId + "/messages";

    http.begin(url);
    http.addHeader("Authorization", "Bot " + discordBotToken);
    http.addHeader("Content-Type", "application/json");

    // Tạo JSON nội dung tin nhắn
    String payload = "{\"content\":\"" + message + "\"}";

    int httpCode = http.POST(payload);  // Gửi POST request

    if (httpCode > 0) {
      Serial.printf("✅ Đã gửi tin nhắn Discord! Mã phản hồi: %d\n", httpCode);
    } else {
      Serial.printf("❌ Lỗi gửi tin nhắn Discord! Mã lỗi: %d\n", httpCode);
    }

    http.end();
  } else {
    Serial.println("⚠️ WiFi chưa kết nối, không thể gửi tin nhắn!");
  }
}

// Hàm để kết nối lại MQTT nếu mất kết nối
void reconnectMQTT() {
  Serial.print("Connecting to MQTT Broker...");
  if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
    Serial.println("connected");
    // Đăng ký nhận tin nhắn từ topic
    client.subscribe(DHT11_TOPIC, 1);  // QoS = 1
    client.subscribe(PIR_TOPIC, 1);    // QoS = 1
    client.subscribe(RC522_TOPIC, 1);  // QoS = 1
    client.subscribe(FLAME_TOPIC, 1);  // QoS = 1
    client.subscribe(MQ2_TOPIC, 1);    // QoS = 1
    client.subscribe(MQ7_TOPIC, 1);    // QoS = 1
  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
}

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
// 📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝

// Hàm callback để xử lý tin nhắn nhận được từ topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("📝Message received on topic: ");
  Serial.print(topic + String(": "));
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  if(strcmp(topic, DHT11_TOPIC) == 0){
    // Nhận từ MQTT Broker
    // 1. Tạo đối tượng JSON (96 byte)
    StaticJsonDocument<96> doc;
    // 2. Giải mã tin nhắn dạng Json
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Parse failed: ");
      Serial.println(error.f_str());
      return;
    }
    // 3. Lấy dữ liệu từ JSON
    // const char* sensor = doc["sensor"];         // Lấy tên cảm biến
    temp = doc["values"]["temperature"];          // Lấy giá trị nhiệt độ
    humi = doc["values"]["humidity"];            // Lấy giá trị độ ẩm

    // Gọi các hàm xử lý logic
    checkHumidity();
    controlHeatLampAutoMode();
    checkTemperature();
  }else if(strcmp(topic, PIR_TOPIC) == 0){
    // Nhận từ MQTT Broker
    // 1. Tạo đối tượng JSON (48 byte)
    StaticJsonDocument<48> doc;
    // 2. Giải mã tin nhắn dạng Json
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Parse failed: ");
      Serial.println(error.f_str());
      return;
    }
    // 3. Lấy dữ liệu từ JSON
    // const char* sensor = doc["sensor"];         // Lấy tên cảm biến
    pirState = doc["value"];                      // Lấy trạng thái cảm biến

    // Gọi hàm xử lý logic
    check_pir_state();
  }else if(strcmp(topic, RC522_TOPIC) == 0){
    // Nhận từ MQTT Broker
    // 1. Tạo đối tượng JSON (48 byte)
    StaticJsonDocument<48> doc;
    // 2. Giải mã tin nhắn dạng Json
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Parse failed: ");
      Serial.println(error.f_str());
      return;
    }
    // 3. Lấy dữ liệu từ JSON
    // const char* sensor = doc["sensor"];               // Lấy tên cảm biến
    uid = doc["value"].as<String>();                    // Lấy id của thẻ

    // Gọi hàm xử lý logic
    check_uid_command();
  }else if(strcmp(topic, MQ2_TOPIC) == 0){
    // Nhận từ MQTT Broker
    // 1. Tạo đối tượng JSON (48 byte)
    StaticJsonDocument<48> doc;
    // 2. Giải mã tin nhắn dạng Json
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Parse failed: ");
      Serial.println(error.f_str());
      return;
    }
    // 3. Lấy dữ liệu từ JSON
    // const char* sensor = doc["sensor"];                  // Lấy tên cảm biến
    gasVal = doc["value"];                                  // Lấy giá trị khí gas

    // Gọi hàm xử lý logic
    checkGasValue();
  }else if(strcmp(topic, MQ7_TOPIC) == 0){
    // Nhận từ MQTT Broker
    // 1. Tạo đối tượng JSON (48 byte)
    StaticJsonDocument<48> doc;
    // 2. Giải mã tin nhắn dạng Json
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Parse failed: ");
      Serial.println(error.f_str());
      return;
    }
    // 3. Lấy dữ liệu từ JSON
    // const char* sensor = doc["sensor"];                 // Lấy tên cảm biến
    coVal = doc["value"];                                  // Lấy giá trị khí CO

    // Gọi hàm kiểm tra logic
    checkCOValue();
  }
  // else if(strcmp(topic, FLAME_TOPIC) == 0){
  //   // Nhận từ MQTT Broker
  //   // 1. Tạo đối tượng JSON (48 byte)
  //   StaticJsonDocument<48> doc;
  //   // 2. Giải mã tin nhắn dạng Json
  //   DeserializationError error = deserializeJson(doc, message);
  //   if (error) {
  //     Serial.print("Parse failed: ");
  //     Serial.println(error.f_str());
  //     return;
  //   }
  //   // 3. Lấy dữ liệu từ JSON
  //   // const char* sensor = doc["sensor"];               // Lấy tên cảm biến
  //   fireState = doc["value"];                           // Lấy trạng thái cảm biến lửa
  // }
  
}
