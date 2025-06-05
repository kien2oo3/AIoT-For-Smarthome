import cv2
import face_recognition
import pickle
import numpy as np
# from blynk_controller import send_blynk_command
import time

FACE_DISTANCE_THRESHOLD = 0.4

# Load model
with open("face_model.pkl", "rb") as f:
    known_face_encodings, known_face_names = pickle.load(f)

# Biến lưu thời gian xác thực khuôn mặt
face_timers = {}
FACE_HOLD_TIME = 2  # số giây phải giữ đúng khuôn mặt để xác thực

# Biến lưu trạng thái nhận diện
last_recognized_name = None
start_time = None

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

    # Nếu mặt quá gần hoặc phẳng → khả năng giả mạo
    if avg_depth < 300 or std_dev < 20:
        return False
    return True

def recognize_and_control(frame, depth_frame):
    global last_recognized_name, start_time

    small_rgb = cv2.resize(frame, (0, 0), fx=0.5, fy=0.5)
    rgb = cv2.cvtColor(small_rgb, cv2.COLOR_BGR2RGB)

    face_locations = face_recognition.face_locations(rgb)
    face_encodings = face_recognition.face_encodings(rgb, face_locations)

    current_time = time.time()
    name = "Nguoi la!"
    coords = None

    for face_location, face_enc in zip(face_locations, face_encodings):
        distances = face_recognition.face_distance(known_face_encodings, face_enc)
        idx = np.argmin(distances)
        min_dist = distances[idx]

        # Vì đã resize 0.5 nên phải nhân 2 lại toạ độ
        top, right, bottom, left = face_location
        coords = (left*2, top*2, right*2, bottom*2)

        # Kiểm tra mặt thật
        if not is_real_face(depth_frame, coords):
            name = "Mat gia!"
            face_timers.clear()
            continue

        if min_dist < FACE_DISTANCE_THRESHOLD:
            # Kiểm tra có phải mặt thật không
            if is_real_face(depth_frame, coords):
                name = known_face_names[idx]
                if last_recognized_name is not None and last_recognized_name == name:
                    if start_time is None:
                        start_time = time.time()
                    if time.time() - start_time >= 2:
                        # send_blynk_command(1)  # Mở cửa
                        print("Mở cửa")
                        last_recognized_name = None
                        start_time = None
                else:
                    last_recognized_name = name
                    start_time = time.time()
            else:
                name = "Picture/Video fake!"
                last_recognized_name = None
                start_time = None
        else:
            name = "Nguoi la!"
            last_recognized_name = None
            start_time = None

        return frame, name, coords

    last_recognized_name = None
    start_time = None

    return frame, None, None