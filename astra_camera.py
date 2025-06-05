# Quản lý kết nối camera Astra
from primesense import openni2
import numpy as np
import cv2
import os
import requests

def start_camera():
    sdk_path = os.path.join(os.getcwd(), "OpenNI2")
    openni2.initialize(sdk_path)
    dev = openni2.Device.open_any()
    depth_stream = dev.create_depth_stream()
    color_stream = dev.create_color_stream()

    depth_stream.start()
    color_stream.start()
    return dev, depth_stream, color_stream

def get_frames(depth_stream, color_stream):
    depth_frame = depth_stream.read_frame()
    color_frame = color_stream.read_frame()

    depth_data = np.frombuffer(depth_frame.get_buffer_as_uint16(), dtype=np.uint16).reshape((480, 640))
    color_data = np.frombuffer(color_frame.get_buffer_as_uint8(), dtype=np.uint8).reshape((480, 640, 3))
    return depth_data, color_data


# Hàm gửi ảnh kèm thông báo qua Discord bot
def send_discord_photo(token, channel_id, photo_path, caption):
    try:
        url = f"https://discord.com/api/v10/channels/{channel_id}/messages"
        headers = {
            "Authorization": f"Bot {token}"
        }
        with open(photo_path, 'rb') as photo_file:
            files = {
                'file': photo_file
            }
            data = {
                'content': caption
            }
            response = requests.post(url, headers=headers, data=data, files=files, timeout=5)
            response.raise_for_status()  # Nếu lỗi HTTP sẽ ném exception

        return response
    except requests.exceptions.RequestException as e:
        print(f"Có lỗi khi gửi tin nhắn đến Discord: {e}")
        return None

# Hàm chụp ảnh từ camera Astra Mini Pro và gửi thông báo qua Discord
def capture_photo_and_send_discord(name):
    # Lấy frame màu từ Astra Mini Pro
    dev, depth_stream, color_stream = start_camera()
    depth_data, color_data = get_frames(depth_stream, color_stream)

    # Lưu ảnh màu vào tệp
    photo_path = "photo_fake_face.jpg"
    cv2.imwrite(photo_path, color_data)  # Lưu ảnh màu vào file

    # Thông tin bot Discord
    bot_token = "MTM3Njc2MjYzMTA2NjYxNTg0MA.GAdtQc.SpgAHiFIBgruTS_jXgB1X1KBCCxIt7-guaaQtM"
    channel_id = "1376770567788695684"

    if name == "Mat gia!":
        caption = "🚨 Cảnh báo: Phát hiện có người giả mạo khuôn mặt bạn! ⚠️"
        print("🚨 Cảnh báo: Phát hiện có người giả mạo khuôn mặt bạn! ⚠️")
    else:
        caption = "🚨 Cảnh báo: Phát hiện khuôn mặt người lạ trên Camera! ⚠️"
        print("🚨 Cảnh báo: Phát hiện khuôn mặt người lạ trên Camera! ⚠️")

    send_discord_photo(bot_token, channel_id, photo_path, caption)


def stop_camera():
    openni2.unload()
