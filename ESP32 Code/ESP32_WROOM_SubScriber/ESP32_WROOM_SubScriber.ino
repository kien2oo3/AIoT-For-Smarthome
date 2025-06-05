#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

unsigned long previousConnectMQTTAttempt = 0;

const char* ssid = "Tầng 3";
const char* pass = "13572468";
const char* mqtt_server = "db6c39fa55a94251b9941e77a4756216.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "kienbui12003";
const char* mqtt_pass = "Kien!!!2";

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

void setup() {
  // Khởi tạo Serial Monitor và kết nối Wi-Fi
  Serial.begin(9600);

  // Kết nối Wifi
  setup_wifi();

  // Cấu hình MQTT Client
  wifiClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);  // Đặt hàm callback để xử lý tin nhắn đến

  // Kết nối MQTT Broker
  reconnectMQTT();
}

void loop() {
  client.loop();  // Xử lý các sự kiện MQTT

  // Kiểm tra kết nối Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost connection. Attempting to reconnect...");
    WiFi.reconnect();  // Thử kết nối lại Wi-Fi nếu bị mất kết nối
    delay(5000);  // Chờ 5 giây trước khi thử lại
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

// Hàm để kết nối lại MQTT nếu mất kết nối
void reconnectMQTT() {
  Serial.print("Connecting to MQTT Broker...");
  if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
    Serial.println("connected");
    // Đăng ký nhận tin nhắn từ topic
    client.subscribe("smart_home/living_room/temp_humi", 1);  // QoS = 1
  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
}

// Hàm callback để xử lý tin nhắn nhận được từ topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message: ");
  Serial.println(message);
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
