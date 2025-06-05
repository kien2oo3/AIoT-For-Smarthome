# main.py
import cv2
from face_recognition_module import recognize_and_control
from astra_camera import start_camera, get_frames, stop_camera

# Khởi động camera Astra
dev, depth_stream, color_stream = start_camera()

while True:
    # Lấy frame từ camera Astra
    depth_frame, color_frame = get_frames(depth_stream, color_stream)

    # Nhận diện khuôn mặt và điều khiển thiết bị
    frame, name, coords, min_dist = recognize_and_control(color_frame, depth_frame)

    # Nếu phát hiện khuôn mặt thì vẽ thông tin lên frame
    if coords:
        left, top, right, bottom = coords
        color = (0, 255, 0) if name != "Nguoi la!" else (0, 0, 255)
        cv2.rectangle(frame, (left, top), (right, bottom), color, 2)
        cv2.rectangle(frame, (left, top - 35), (right, top), color, cv2.FILLED)
        cv2.putText(frame, name, (left + 6, top - 6), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)
        cv2.putText(frame, f"{min_dist:.2f}", (left, bottom + 20), cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, 2)

    cv2.imshow("Smart Door AI - Astra Mini Pro", frame)

    # Thoát bằng phím 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Dừng camera và đóng cửa sổ
stop_camera()
cv2.destroyAllWindows()
