import paho.mqtt.client as mqtt
import json
import csv

# Thông tin MQTT Broker
BROKER = "192.168.94.40"
PORT = 8883  # Cổng TLS
TOPIC = "home/sensor/mq-7/co"
USERNAME = "user_subscribe"
PASSWORD = "kien2"

# Đường dẫn tới file chứng chỉ CA (định dạng PEM)
CA_CERT_PATH = "broker.crt"

# File lưu dữ liệu thu thập
FILE_NAME = "mq7_data.csv"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker successfully")
        client.subscribe(TOPIC)
    else:
        print(f"Failed to connect, return code {rc}")

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()
        data = json.loads(payload)
        sensor = data.get("sensor", "")
        value = data.get("value", None)
        timestamp = data.get("timestamp", "")

        print(f"Received: Sensor={sensor}, Value={value}, Timestamp={timestamp}")

        # Ghi vào file CSV
        with open(FILE_NAME, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([timestamp, value])

    except Exception as e:
        print(f"Error processing message: {e}")

client = mqtt.Client()

client.username_pw_set(USERNAME, PASSWORD)

# Cấu hình TLS với chứng chỉ CA
client.tls_set(ca_certs=CA_CERT_PATH)

client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, keepalive=60)

print(f"Starting data collection on topic: {TOPIC}")

client.loop_forever()
