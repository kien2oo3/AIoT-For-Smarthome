import cv2
import os
import numpy as np
import face_recognition

def variance_of_laplacian(image):
    # Đánh giá độ nét ảnh (sharpness)
    return cv2.Laplacian(image, cv2.CV_64F).var()

def is_blurry(image, threshold=100.0):
    # Nếu độ nét < threshold thì coi là mờ
    return variance_of_laplacian(image) < threshold

def collect_face_data(folder_name, max_images=60, encoding_threshold=0.4):
    save_path = os.path.join("dataimage_filtered_clean", folder_name)
    os.makedirs(save_path, exist_ok=True)

    video = cv2.VideoCapture(0)
    collected_encodings = []

    count = 0
    print(f"📷 Bắt đầu thu thập dữ liệu cho '{folder_name}'... Nhấn 'q' để thoát.")

    while count < max_images:
        ret, frame = video.read()
        if not ret:
            continue

        rgb_frame = frame[:, :, ::-1]
        face_locations = face_recognition.face_locations(rgb_frame)

        if len(face_locations) != 1:
            cv2.putText(frame, "Phat hien 0 hoac nhieu hon 1 khuon mat, bo qua", (10,30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)
            cv2.imshow("Collecting Faces", frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
            continue

        top, right, bottom, left = face_locations[0]
        face_image = frame[top:bottom, left:right]

        gray_face = cv2.cvtColor(face_image, cv2.COLOR_BGR2GRAY)

        if is_blurry(gray_face):
            cv2.putText(frame, "Anh mo, bo qua...", (10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)
            cv2.imshow("Collecting Faces", frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
            continue

        encodings = face_recognition.face_encodings(rgb_frame, [face_locations[0]])
        if not encodings:
            continue
        encoding = encodings[0]

        if collected_encodings:
            distances = face_recognition.face_distance(collected_encodings, encoding)
            if np.min(distances) < encoding_threshold:
                cv2.putText(frame, "Anh trung lap, bo qua...", (10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,255,255), 2)
                cv2.imshow("Collecting Faces", frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
                continue

        img_path = os.path.join(save_path, f"face_{count+1}.jpg")
        cv2.imwrite(img_path, face_image)
        collected_encodings.append(encoding)
        count += 1

        cv2.putText(frame, f"Da luu: {count}/{max_images}", (10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,255,0), 2)
        cv2.rectangle(frame, (left, top), (right, bottom), (0,255,0), 2)
        cv2.imshow("Collecting Faces", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    video.release()
    cv2.destroyAllWindows()
    print(f"✅ Da thu thap {count} anh cho '{folder_name}' o '{save_path}'")

if __name__ == "__main__":
    folder = input("Nhap ten nguoi dung de luu anh: ").strip()
    collect_face_data(folder, max_images=60)
