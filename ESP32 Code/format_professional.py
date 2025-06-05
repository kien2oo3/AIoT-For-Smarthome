#⚡⚡⚡⚡⚡⚡⚡ KHAI BÁO CÁC THƯ VIỆN ⚡⚡⚡⚡⚡⚡⚡
from BlynkLib import Blynk         # Giúp kết nối Raspberry Pi với nền tảng Blynk
from BlynkTimer import BlynkTimer  # Giúp tạo và quản lý các bộ định thời (timer) trong Blynk
import adafruit_dht
import board
import time
import RPi.GPIO as GPIO
# import threading
# ⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡⚡


# 📝📝📝📝📝📝📝📝 KHAI BÁO HẰNG 📝📝📝📝📝📝📝📝📝
#define BLYNK_TEMPLATE_ID "TMPL6M3twTNI3"
#define BLYNK_TEMPLATE_NAME "Smart House"
BLYNK_AUTH_TOKEN = "gBecD2sooSgIHuZn4bIIxx1MPXblwwcO"
# 📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝📝


#✨✨✨✨✨✨✨✨ KHỞI TẠO BLYNK ✨✨✨✨✨✨✨✨
blynk = Blynk(BLYNK_AUTH_TOKEN)     # 🔑 Authentication
timer = BlynkTimer()                # ⏲️ Timer Setup
#✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨ ✨✨✨✨✨


#🚀🚀🚀🚀🚀🚀🚀🚀🚀 SETUP GPIOs 🚀🚀🚀🚀🚀🚀🚀🚀
# GPIO.setwarnings(False)
# GPIO.setmode(GPIO.BCM)

# DOOR_PIN = 21
# ROOM_PIN = 20

# GPIO.setup(DOOR_PIN, GPIO.OUT)
# GPIO.setup(ROOM_PIN, GPIO.OUT)

# GPIO.output(DOOR_PIN, GPIO.LOW)
# GPIO.output(ROOM_PIN, GPIO.LOW)
#🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀


#✨✨✨✨✨✨✨✨ KHỞI TẠO DHT11 ✨✨✨✨✨✨✨✨
dhtDevice = adafruit_dht.DHT11(board.D20)
#✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨ ✨✨✨✨✨


#🚀🚀🚀🚀🚀🚀🚀 MÃ MÀU ANSI 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀
RED = "\033[91m"
GREEN = "\033[92m"
BLUE = "\033[94m"
RESET = "\033[0m"
# 🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀🚀


# 🛜 Virtual pin Control
# @blynk.on("V3")
# def v4_write_handler(value):
#     if int(value[0])==1:
#         GPIO.output(ROOM_PIN, GPIO.HIGH)
#         print(GREEN+"Đèn phòng đang bật!"+RESET)
#     else:
#         GPIO.output(ROOM_PIN, GPIO.LOW)
#         print(RED+"Đèn phòng đã tắt!"+RESET)

# @blynk.on("V4")
# def v4_write_handler(value):
#     if int(value[0])==1:
#         GPIO.output(DOOR_PIN, GPIO.HIGH)
#         print(GREEN+"Đèn tín hiệu cửa đang bật!"+RESET)
#     else:
#         GPIO.output(DOOR_PIN, GPIO.LOW)
#         print(RED+"Đèn tín hiệu cửa đã tắt!"+RESET)


# 📝📝📝📝📝📝📝📝 HÀM TỰ ĐỊNH NGHĨA 📝📝📝📝📝📝📝📝📝📝📝
# Hàm để gửi nhiệt độ và độ ẩm tới Blynk
def send_temp_humi():
    try:
        temperature = dhtDevice.temperature
        humidity = dhtDevice.humidity
        print(f"Temp: {temperature}C    Humidity: {humidity}%")
        blynk.virtual_write(1, temperature)  # Gửi nhiệt độ tới Virtual Pin V1
        blynk.virtual_write(2, humidity)     # Gửi độ ẩm tới Virtual Pin V2
    except RuntimeError as error:
        print(RED + error.args[0] + RESET)
    except Exception as error:
        dhtDevice.exit()
        raise error

# 💡💡💡💡💡💡Gọi các hàm lần đầu tiên 💡💡💡💡💡💡💡💡
# send_temp_humi()

# 🔄🔄🔄🔄🔄🔄 Timer interval: Cài đặt bộ định thời để gửi dữ liệu cảm biến 🔄🔄🔄🔄🔄🔄
# timer.set_interval(1, sendGasValueForBlynk)
timer.set_interval(1, send_temp_humi)


# 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐 CONNECTION STATUS 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐
# Hàm để đồng bộ dữ liệu từ các chân ảo
@blynk.on("connected")
def blynk_connected():
    print(GREEN + "✅Đã đồng bộ chân ảo Blynk với GPIO của Raspberry Pi!" + RESET)
    blynk.sync_virtual(0, 1, 2, 3, 4)

# 🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐🌐

# ⏳ THÊM THỜI GIAN CHỜ 1 GIÂY Ở ĐÂY
time.sleep(1)  # 🕒 Chờ 1 giây để hệ thống ổn định

# 🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀 Vòng lặp chính 🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀
try:
    print(BLUE +"🔄 Chương trình đang khởi động..." + RESET)
    print("⏳ Đợi 1 giây để hoàn tất thiết lập...")
    time.sleep(1)  # 🕒 Thêm dòng này để chờ
    print(GREEN + "🚀 Chương trình đã sẵn sàng! Nhấn Ctrl+C để kết thúc." + RESET)
    while True:
        blynk.run()  # Xử lý các sự kiện từ Blynk
        timer.run()  # Chạy các bộ định thời đã cài đặt
        time.sleep(0.1)
except KeyboardInterrupt:
    print(BLUE + "\nChương trình kết thúc do người dùng yêu cầu..." + BLUE)
finally:
    GPIO.cleanup()
    print(GREEN + "GPIO đã được dọn dẹp. Hẹn gặp lại!" + RESET)

# 🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀🌀
