import cv2
import face_recognition
import pickle
import numpy as np
from blynk_controller import send_blynk_open_gate
from astra_camera import capture_photo_and_send_discord
import time
import os

FACE_DISTANCE_THRESHOLD = 0.4

# Load model
with open("face_model.pkl", "rb") as f:
    known_face_encodings, known_face_names = pickle.load(f)

# Biến lưu thời gian xác thực khuôn mặt
face_timers = {}
fake_face_timers = {}
FACE_HOLD_TIME = 3  # số giây phải giữ đúng khuôn mặt để xác thực

# Hàm kiểm tra khuôn mặt giả mạo
def is_real_face(depth_frame, coords):
    if coords is None:
        return False

    left, top, right, bottom = coords
    # Giới hạn tránh lỗi vượt kích thước
    h, w = depth_frame.shape
    left, top = max(0, left), max(0, top)
    right, bottom = min(w, right), min(h, bottom)

    face_depth = depth_frame[top:bottom, left:right]
    valid_depths = face_depth[face_depth > 0]

    if len(valid_depths) == 0:
        return False  # Không có dữ liệu chiều sâu

    avg_depth = valid_depths.mean()
    std_dev = np.std(valid_depths)

    print(f"[DEBUG] avg_depth: {avg_depth}, std_dev: {std_dev}")

    # Kiểm tra mặt thật dựa trên chiều sâu và độ lệch chuẩn
    if avg_depth < 300:
        print("[DEBUG] Mặt quá gần – khả năng giả mạo.")
        return False
    if std_dev == 0 or std_dev < 10:
        print("[DEBUG] Mặt quá phẳng – có thể là ảnh/mặt giả.")
        return False
    if std_dev < 20:
        print("[DEBUG] Mặt có độ sâu thấp – cần xác minh thêm.")
        return True  # Chấp nhận nhưng có thể kết hợp thêm logic xác minh
    return True


# Hàm tăng cường ảnh khi môi trường không tối ưu
# def preprocess_image(frame):
#     gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
#     equ = cv2.equalizeHist(gray)
#     return cv2.cvtColor(equ, cv2.COLOR_GRAY2BGR)

# Biến toàn cục lưu thời điểm hành động cuối cùng (khởi tạo ngoài hàm)
last_open_gate_time = 0
last_fake_face_alert_time = 0
last_unknown_face_alert_time = 0
ACTION_COOLDOWN = 5  # Thời gian chờ giữa các hành động (giây)

def recognize_and_control(color_frame, depth_frame):
    global last_open_gate_time, last_fake_face_alert_time, last_unknown_face_alert_time

    rgb_frame = color_frame[:, :, ::-1]  # Chuyển BGR sang RGB
    face_locations = face_recognition.face_locations(rgb_frame)
    face_encodings = face_recognition.face_encodings(rgb_frame, face_locations)

    current_time = time.time()
    name = "Nguoi la!"
    coords = None
    min_dist = None

    for (top, right, bottom, left), face_enc in zip(face_locations, face_encodings):
        distances = face_recognition.face_distance(known_face_encodings, face_enc)
        idx = np.argmin(distances)
        min_dist = distances[idx]

        coords = (left, top, right, bottom)

        # Kiểm tra mặt thật
        if not is_real_face(depth_frame, coords):
            name = "Mat gia!"
            if name not in fake_face_timers:
                fake_face_timers[name] = current_time
            else:
                elapsed = current_time - fake_face_timers[name]
                if elapsed >= 10 and (current_time - last_fake_face_alert_time) >= ACTION_COOLDOWN:
                    if not os.path.exists("trigger.txt"):
                        with open("trigger.txt", "w") as f:
                            f.write(f"ALERT:{name}")
                    try:
                        capture_photo_and_send_discord(name)
                    except Exception as e:
                        print(f"[LỖI] Gửi ảnh thất bại: {e}")
                    fake_face_timers.clear()
                    last_fake_face_alert_time = current_time
            face_timers.clear()
            continue

        # Nếu là mặt thật, nhận diện khuôn mặt
        if min_dist < FACE_DISTANCE_THRESHOLD:
            name = known_face_names[idx]
        else:
            name = "Nguoi la!"

        if name != "Nguoi la!":
            fake_face_timers.clear()
            if name not in face_timers:
                face_timers[name] = current_time
            else:
                elapsed = current_time - face_timers[name]
                if elapsed >= FACE_HOLD_TIME and (current_time - last_open_gate_time) >= ACTION_COOLDOWN:
                    print("Mở cửa")
                    if not os.path.exists("trigger.txt"):
                        with open("trigger.txt", "w") as f:
                            f.write(f"GREET:{name}")
                    try:
                        send_blynk_open_gate()
                    except Exception as e:
                        print(f"[LỖI] Gửi tín hiệu mở cổng thất bại: {e}")
                    face_timers.pop(name, None)
                    last_open_gate_time = current_time
        else:  # name == "Nguoi la!"
            face_timers.clear()
            if name not in fake_face_timers:
                fake_face_timers[name] = current_time
            else:
                elapsed = current_time - fake_face_timers[name]
                if elapsed >= 5 and (current_time - last_unknown_face_alert_time) >= ACTION_COOLDOWN:
                    if not os.path.exists("trigger.txt"):
                        with open("trigger.txt", "w") as f:
                            f.write(f"ALERT:{name}")
                    try:
                        capture_photo_and_send_discord(name)
                    except Exception as e:
                        print(f"[LỖI] Gửi ảnh thất bại: {e}")
                    fake_face_timers.clear()
                    last_unknown_face_alert_time = current_time

    return color_frame, name, coords, min_dist



