// Khai báo thông tin mạng và Blynk
#define BLYNK_TEMPLATE_ID "TMPL6bBp0Lwdq"         // Khai báo nếu sử dụng Blynk 2.0
#define BLYNK_TEMPLATE_NAME "Smart House"       // Khai báo nếu sử dụng Blynk 2.0
#define BLYNK_AUTH_TOKEN "gBecD2sooSgIHuZn4bIIxx1MPXblwwcO" // Mã xác thực của bạn từ Blynk

#define BLYNK_PRINT Serial // Kích hoạt việc in thông tin Blynk trên Serial Monitor

// Danh sách TOPIC:
#define DHT11_TOPIC "smart_home/living_room/temp_humi"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
char ssid[] = "Tầng 3";      // Tên mạng Wi-Fi
char pass[] = "13572468";    // Mật khẩu Wi-Fi

// MQTT HiveMQ
const char* mqtt_server = "db6c39fa55a94251b9941e77a4756216.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "kienbui12003";
const char* mqtt_pass = "Kien!!!2";
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

unsigned long previousConnectMQTTAttempt = 0;
unsigned long previousConnectBlynkAttempt = 0;

// Khai báo chân kết nối cảm biến DHT11 và Relay
#define DHTPIN 2             // Chân kết nối DHT11
#define RELAY_PIN 12         // Chân Relay

#define DHTTYPE DHT11        // Loại cảm biến DHT

Servo myServo;

DHT dht(DHTPIN, DHTTYPE);   // Khởi tạo đối tượng DHT
BlynkTimer timer;           // Tạo đối tượng BlynkTimer để quản lý các bộ định thời

void setup() {
  // Khởi tạo Serial Monitor và kết nối Wi-Fi
  Serial.begin(115200);

  // Kết nối Wifi
  setup_wifi(); 

  // Cấu hình MQTT client
  wifiClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);

  // Kết nối đến MQTT Broker
  reconnectMQTT();

  // Kết nối với Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Khởi tạo cảm biến DHT
  dht.begin();

  myServo.attach(5); // Gắn vào chân D5 (GPIO5)
  
  // Thiết lập chân Relay và bắt đầu ở trạng thái tắt
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 

  // Thiết lập bộ định thời để gửi dữ liệu cảm biến mỗi 1 giây
  timer.setInterval(5000L, sendTempHumi);
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

// Hàm kết nối đến MQTT Broker với Client là ESP32-C6
void reconnectMQTT() {
  Serial.print("Connecting to MQTT...");
  if (client.connect("ESP32C6Client", mqtt_user, mqtt_pass)) {
    Serial.println("connected");
  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
}

// Hàm gửi dữ liệu nhiệt độ và độ ẩm lên Blynk
void sendTempHumi() {
  // Đọc giá trị từ cảm biến DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Kiểm tra nếu cảm biến không trả về giá trị hợp lệ
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!"); // In thông báo lỗi nếu không đọc được
    return;
  }

  // Gửi lên MQTT Broker
  // 1. Tạo đối tượng JSON (128 byte)
  StaticJsonDocument<128> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["relay"] = digitalRead(RELAY_PIN) == 1 ? "on" : "off";

  // 2. Chuyển thành chuỗi JSON
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  // 3. In ra Serial Monitor
  Serial.print("Chuỗi JSON:");
  Serial.println(jsonBuffer); // {"temperature":25.5,"humidity":60,"relay":"on"}

  // 4. Gửi chuỗi JSON lên MQTT Broker
  client.publish(DHT11_TOPIC, jsonBuffer, false);

  // Gửi dữ liệu nhiệt độ và độ ẩm lên các chân ảo của Blynk
  Blynk.virtualWrite(1, temperature);  // Gửi nhiệt độ lên Virtual Pin V1
  Blynk.virtualWrite(2, humidity);     // Gửi độ ẩm lên Virtual Pin V2
}

// Hàm xử lý lệnh bật/tắt Relay từ Blynk (Virtual Pin V3)
BLYNK_WRITE(V3) {
  int relayState = param.asInt();  // Đọc giá trị từ Virtual Pin V3
  
  if (relayState == 1) {
    digitalWrite(RELAY_PIN, HIGH); // Bật Relay (quạt, đèn, v.v.)
    Serial.println("Relay ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Tắt Relay
    Serial.println("Relay OFF");
  }
}

BLYNK_WRITE(V4) {
  int servoState = param.asInt();  // Đọc giá trị từ Virtual Pin V3
  
  if (servoState == 1) {
    myServo.write(180);
    Serial.println("Relay ON");
  } else {
    myServo.write(0);
    Serial.println("Relay OFF");
  }
}

// Hàm đồng bộ hóa chân ảo khi kết nối với Blynk
BLYNK_CONNECTED() {
  // Đồng bộ hóa chân ảo (V1, V2, V3)
  Blynk.syncVirtual(0, 1, 2, 3, 4);  
  Serial.println("Blynk synced!");
}

// Hàm loop
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
    // Chờ 5 giây
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
    // Chờ 5 giây
    if(currentMillis - previousConnectMQTTAttempt >= 5000){
      if(!client.connected()){
        Serial.println("MQTT lost connection. Attempting to reconnect...");
        reconnectMQTT();  // Thử kết nối lại MQTT nếu bị mất kết nối
      }
      previousConnectMQTTAttempt = currentMillis;
    }
  }
}
