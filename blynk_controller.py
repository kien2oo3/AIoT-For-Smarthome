# blynk_controller.py - Gửi lệnh mở cổng qua Blynk an toàn hơn
import requests

# Cấu hình Blynk Local Server
BLYNK_SERVER = "192.168.94.40"
# BLYNK_SERVER = "192.168.0.22"
BLYNK_TOKEN_PUBLISHER = "3x1_Yw6KFOuYXgBzsL9S_zt6cXNSGK9x"
BLYNK_TOKEN_SUBSCRIBER = "TATin2iAt40gLapsSG22f9-agy9mJZrp"

# Virtual pins publisher:
TEMP_VP = "V3"
HUMI_VP = "V4"
CO_VP = "V6"
# Virtual pins subscriber:
GATE_VP = "V3"
FAN_VP = "V6"
HALLWAY_LAMP_VP = "V4"
HALLWAY_LAMP_AUTO_VP = "V5"
HEAT_LAMP_VP = "V7"
HEAT_LAMP_AUTO_VP = "V8"

# ✨✨✨✨✨✨✨✨✨✨ Các hàm dùng trong nhận diện khuôn mặt ✨✨✨✨✨✨✨✨✨✨✨✨✨
def send_blynk_open_gate():
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_SUBSCRIBER}/update/{GATE_VP}?value=1"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        print("✅ Đã gửi lệnh mở cổng thành công!")
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")

# ✨✨✨✨✨✨✨✨✨✨ Các hàm dùng trong trợ lý ảo ✨✨✨✨✨✨✨✨✨✨✨✨✨
# Hàm lấy nhiệt độ qua Blynk
def get_temp_for_blynk():
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_PUBLISHER}/get/{TEMP_VP}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        data = response.json()  # Dữ liệu trả về là mảng JSON

        if isinstance(data, list) and len(data) > 0:
            return float(data[0])  # Trả về nhiệt độ là số thực
        else:
            print("Dữ liệu trả về không hợp lệ.")
            return None
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return None

def get_humi_for_blynk():
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_PUBLISHER}/get/{HUMI_VP}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        data = response.json()  # Dữ liệu trả về là mảng JSON

        if isinstance(data, list) and len(data) > 0:
            return float(data[0])  # Trả về độ ẩm là số thực
        else:
            print("Dữ liệu trả về không hợp lệ.")
            return None
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return None

def get_co_for_blynk():
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_PUBLISHER}/get/{CO_VP}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        data = response.json()  # Dữ liệu trả về là mảng JSON

        if isinstance(data, list) and len(data) > 0:
            return float(data[0])  # Trả về giá trị khí CO là số thực
        else:
            print("Dữ liệu trả về không hợp lệ.")
            return None
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return None

# Hàm bật tắt quạt
def control_fan_for_blynk(value):
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_SUBSCRIBER}/update/{FAN_VP}?value={value}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        print("✅ Đã gửi lệnh bật/tắt quạt thành công!")
        return "ok"
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return "err"

# Hàm bật đèn sưởi
def control_heat_lamp_for_blynk(value):
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_SUBSCRIBER}/update/{HEAT_LAMP_VP}?value={value}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        print("✅ Đã gửi lệnh bật/tắt đèn sưởi thành công!")
        return "ok"
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return "err"

# Hàm bật đèn sưởi thông minh
def auto_control_heat_lamp_for_blynk(value):
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_SUBSCRIBER}/update/{HEAT_LAMP_AUTO_VP}?value={value}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        print("✅ Đã gửi lệnh bật/tắt đèn sưởi thông minh thành công!")
        return "ok"
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return "err"

# Hàm bật đèn hành lang
def control_hallway_lamp_for_blynk(value):
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_SUBSCRIBER}/update/{HALLWAY_LAMP_VP}?value={value}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        print("✅ Đã gửi lệnh bật/tắt đèn hành lang thành công!")
        return "ok"
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return "err"

# Hàm bật đèn hành lang thông minh
def auto_control_hallway_lamp_for_blynk(value):
    url = f"http://{BLYNK_SERVER}:8080/{BLYNK_TOKEN_SUBSCRIBER}/update/{HALLWAY_LAMP_AUTO_VP}?value={value}"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()  # Bắt lỗi HTTP nếu có
        print("✅ Đã gửi lệnh bật/tắt đèn hành lang thông minh thành công!")
        return "ok"
    except Exception as e:
        print(f"❌ Lỗi khi truy vấn Blynk: {e}")
        return "err"