// Khai báo thông tin mạng và Blynk
#define BLYNK_TEMPLATE_ID "TMPL6bBp0Lwdq"         // Khai báo nếu sử dụng Blynk 2.0
#define BLYNK_TEMPLATE_NAME "Smart House"       // Khai báo nếu sử dụng Blynk 2.0
#define BLYNK_AUTH_TOKEN "gBecD2sooSgIHuZn4bIIxx1MPXblwwcO" // Mã xác thực của bạn từ Blynk

#define BLYNK_PRINT Serial // Kích hoạt việc in thông tin Blynk trên Serial Monitor

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Wi-Fi credentials
char ssid[] = "Tầng 3";      // Tên mạng Wi-Fi
char pass[] = "13572468";    // Mật khẩu Wi-Fi

// Khai báo chân kết nối cảm biến DHT11 và Relay
#define DHTPIN 4             // Chân kết nối DHT11
#define RELAY_PIN 12         // Chân Relay

#define DHTTYPE DHT11        // Loại cảm biến DHT

DHT dht(DHTPIN, DHTTYPE);   // Khởi tạo đối tượng DHT
BlynkTimer timer;           // Tạo đối tượng BlynkTimer để quản lý các bộ định thời

void setup() {
  // Khởi tạo Serial Monitor và kết nối Wi-Fi
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  
  // Đợi kết nối Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  // Kết nối với Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Khởi tạo cảm biến DHT
  dht.begin();
  
  // Thiết lập chân Relay và bắt đầu ở trạng thái tắt
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 

  // Thiết lập bộ định thời để gửi dữ liệu cảm biến mỗi 1 giây
  timer.setInterval(1000L, sendTempHumi);
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

// Hàm đồng bộ hóa chân ảo khi kết nối với Blynk
BLYNK_CONNECTED() {
  // Đồng bộ hóa chân ảo (V1, V2, V3)
  Blynk.syncVirtual(1, 2, 3);  
  Serial.println("Blynk synced!");
}

void loop() {
  Blynk.run();         // Xử lý các sự kiện từ Blynk
  timer.run();         // Chạy các bộ định thời đã cài đặt (định kỳ gửi dữ liệu cảm biến)
  
  // Bắt lỗi khi kết nối không thành công với Blynk
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost connection. Attempting to reconnect...");
    WiFi.reconnect();  // Thử kết nối lại Wi-Fi nếu bị mất kết nối
    delay(5000);  // Chờ 5 giây trước khi thử lại
  }
}
