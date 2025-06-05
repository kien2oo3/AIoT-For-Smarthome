import paho.mqtt.client as mqtt
import ssl
import pandas as pd
import json
import joblib
import requests
import time

# ==== Cấu hình MQTT ====
MQTT_USERNAME = "user_subscribe_flame_ai"
MQTT_PASSWORD = "demo"
# MQTT_BROKER = "192.168.0.22"
MQTT_BROKER = "192.168.94.40"
MQTT_PORT = 8883
CA_CERT_PATH = "broker.crt"

TOPIC_FLAME = "home/sensor/flame/command"
TOPIC_MQ7 = "home/sensor/mq-7/co"
TOPIC_DHT = "home/sensor/dht11/temp-humi"

# ==== Cấu hình Blynk ====
BLYNK_TOKEN_PUBLISHER = "3x1_Yw6KFOuYXgBzsL9S_zt6cXNSGK9x"
BLYNK_TOKEN_SUBSCRIBER = "TATin2iAt40gLapsSG22f9-agy9mJZrp"
BLYNK_SERVER = "192.168.94.40"
# BLYNK_SERVER = "192.168.0.22"
LED_VPIN_PUBLISHER = "V13"
TEXT_VPIN_SUBSCRIBER = "V13"

# ==== Khai báo màu đèn ====
COLOR_RED = "%23E74C3C"
COLOR_GREEN = "%232ECC71"

prevColor = COLOR_GREEN

# ==== Tải mô hình AI ====
model = joblib.load("flame_ai_model.pkl")
print("✅ AI Model loaded!")

# ==== Biến lưu dữ liệu tạm ====
current_data = {
    "Flame_State": None,
    "CO_ppm": None,
    "Temperature": None,
    "Humidity": None
}

# ==== Hàm gọi API Blynk ====
def send_fire_status_to_publisher(color):
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_PUBLISHER}/update/{LED_VPIN_PUBLISHER}?color={color}"
    try:
        requests.get(url)
    except:
        print("❌ Lỗi khi gọi API Blynk!")
        
def send_fire_status_to_subscriber(mess):
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_SUBSCRIBER}/update/{TEXT_VPIN_SUBSCRIBER}?value={mess}"
    try:
        requests.get(url)
    except:
        print("❌ Lỗi khi gọi API Blynk!")

# ==== Xử lý khi nhận đủ dữ liệu ====
def process_ai():
    global prevColor
    df = pd.DataFrame([current_data])
    prediction = model.predict(df)[0]
    send_fire_status_to_subscriber(prediction)    # Gửi trạng thái cảm biến lửa lên Blynk cửa Subscriber
    print(f"🤖 AI Dự đoán: {prediction}")

    if prediction == "Flame Sensor Error":
        # send_fire_status_to_publisher(COLOR_RED)
        if prevColor != COLOR_RED:
            send_fire_status_to_publisher(COLOR_RED)   # Gửi trạng thái cảm biến lửa lên Blynk cửa Publisher
            prevColor = COLOR_RED
    else:
        # send_fire_status_to_publisher(COLOR_GREEN)
        if prevColor != COLOR_GREEN:
            send_fire_status_to_publisher(COLOR_GREEN)  # Gửi trạng thái cảm biến lửa lên Blynk cửa Publisher
            prevColor = COLOR_GREEN

# ==== Xử lý MQTT message ====
def on_message(client, userdata, msg):
    global current_data

    payload = json.loads(msg.payload.decode())
    topic = msg.topic

    if topic == TOPIC_FLAME:
        current_data["Flame_State"] = payload.get("value")

    elif topic == TOPIC_MQ7:
        current_data["CO_ppm"] = payload.get("value")

    elif topic == TOPIC_DHT:
        values = payload.get("values")
        current_data["Temperature"] = values.get("temperature")
        current_data["Humidity"] = values.get("humidity")

    # Khi đủ dữ liệu thì chạy AI
    if None not in current_data.values():
        process_ai()
        # Reset để nhận dữ liệu mới
        current_data.update(dict.fromkeys(current_data, None))

# ==== Kết nối MQTT ====
client = mqtt.Client()
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
# Cấu hình TLS với CA certificate
client.tls_set(ca_certs=CA_CERT_PATH,
               certfile=None,
               keyfile=None,
               cert_reqs=ssl.CERT_REQUIRED,
               tls_version=ssl.PROTOCOL_TLS_CLIENT,
               ciphers=None)
client.on_message = on_message
client.connect(MQTT_BROKER, MQTT_PORT)
client.subscribe([(TOPIC_FLAME, 0), (TOPIC_MQ7, 0), (TOPIC_DHT, 0)])

print("🚀 AI Realtime đang chạy... Nhấn Ctrl+C để dừng!")

try:
    client.loop_forever()
except KeyboardInterrupt:
    print("🛑 Dừng AI Monitoring!")
