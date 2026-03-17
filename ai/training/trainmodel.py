# import os
# import face_recognition
# import pickle
# import numpy as np
#
# # Thư mục chứa ảnh khuôn mặt
# data_dir = "dataimage/"
#
# # Danh sách lưu đặc trưng khuôn mặt và nhãn
# known_face_encodings = []
# known_face_names = []
#
# # Quét tất cả thư mục trong dataimage (mỗi thư mục là tên của 1 người)
# for person_name in os.listdir(data_dir):
#     person_dir = os.path.join(data_dir, person_name)
#     encodings = []
#
#     # Duyệt tất cả ảnh trong thư mục của từng người
#     for img_name in os.listdir(person_dir):
#         img_path = os.path.join(person_dir, img_name)
#
#         # Đọc ảnh và trích xuất đặc trưng khuôn mặt
#         image = face_recognition.load_image_file(img_path)
#         face_enc = face_recognition.face_encodings(image)
#
#         if face_enc:  # Chỉ lấy nếu phát hiện khuôn mặt
#             encodings.append(face_enc[0])
#
#     if encodings:
#         # Tính vector trung bình để mô hình nhận diện chính xác hơn
#         avg_encoding = np.mean(encodings, axis=0)
#         known_face_encodings.append(avg_encoding)
#         known_face_names.append(person_name)
#
# # Lưu model vào file
# with open("face_model.pkl", "wb") as f:
#     pickle.dump((known_face_encodings, known_face_names), f)
#
# print(f"Training hoàn tất! Đã lưu {len(known_face_names)} khuôn mặt vào 'face_model.pkl'")
#
#
#

# import os
# import face_recognition
# import pickle
# import numpy as np
#
# # Thư mục chứa ảnh đã lọc
# data_dir = "dataimage_filtered_clean"
#
# # Danh sách lưu đặc trưng khuôn mặt và tên
# known_face_encodings = []
# known_face_names = []
#
# print("🚀 Bắt đầu huấn luyện mô hình nhận diện khuôn mặt...")
# print(f"📁 Đọc dữ liệu từ: {data_dir}")
#
# # Duyệt từng người
# for person_name in os.listdir(data_dir):
#     person_dir = os.path.join(data_dir, person_name)
#     if not os.path.isdir(person_dir):
#         continue
#
#     print(f"🧠 Xử lý: {person_name}")
#     encodings = []
#
#     for img_name in os.listdir(person_dir):
#         img_path = os.path.join(person_dir, img_name)
#         try:
#             image = face_recognition.load_image_file(img_path)
#             face_enc = face_recognition.face_encodings(image)
#
#             if face_enc:
#                 encodings.append(face_enc[0])
#         except Exception as e:
#             print(f"⚠️ Lỗi xử lý ảnh {img_name}: {e}")
#
#     if encodings:
#         avg_encoding = np.mean(encodings, axis=0)
#         known_face_encodings.append(avg_encoding)
#         known_face_names.append(person_name)
#         print(f"✅ {person_name}: {len(encodings)} encoding hợp lệ")
#     else:
#         print(f"❌ {person_name}: Không có encoding hợp lệ nào")
#
# # Lưu mô hình
# with open("face_model.pkl", "wb") as f:
#     pickle.dump((known_face_encodings, known_face_names), f)
#
# print("\n🎉 Huấn luyện hoàn tất!")
# print(f"📦 Mô hình lưu tại: face_model.pkl")
# print(f"👥 Tổng số người nhận diện: {len(known_face_names)}")
# for name in known_face_names:
#     print(f"   - {name}")

# trainmodel_final.py — Huấn luyện mô hình nhận diện khuôn mặt chuẩn nhất

# import os
# import face_recognition
# import pickle
# import numpy as np
# import random
#
# # === CẤU HÌNH ===
# data_dir = "dataimage_filtered_clean"  # Thư mục chứa các thư mục người dùng
# model_output = "face_model.pkl"
# max_encodings_per_person = 60  # Số vector encoding tối đa mỗi người
#
# # === BIẾN KẾT QUẢ ===
# known_face_encodings = []
# known_face_names = []
#
# print("🚀 Bắt đầu huấn luyện mô hình nhận diện khuôn mặt...")
# print(f"📁 Đọc ảnh từ thư mục: {data_dir}")
#
# # === XỬ LÝ TỪNG NGƯỜI ===
# for person_name in os.listdir(data_dir):
#     person_dir = os.path.join(data_dir, person_name)
#     if not os.path.isdir(person_dir):
#         continue
#
#     print(f"\n🧠 Xử lý người: {person_name}")
#     encodings = []
#
#     for img_name in os.listdir(person_dir):
#         img_path = os.path.join(person_dir, img_name)
#         try:
#             image = face_recognition.load_image_file(img_path)
#             face_enc = face_recognition.face_encodings(image)
#
#             if face_enc:
#                 encodings.append(face_enc[0])
#         except Exception as e:
#             print(f"⚠️ Lỗi xử lý ảnh {img_name}: {e}")
#
#     if encodings:
#         random.shuffle(encodings)  # Trộn ngẫu nhiên để tránh lệch theo thứ tự ảnh
#         selected_encodings = encodings[:min(max_encodings_per_person, len(encodings))]
#
#         known_face_encodings.extend(selected_encodings)
#         known_face_names.extend([person_name] * len(selected_encodings))
#
#         print(f"✅ {person_name}: đã lưu {len(selected_encodings)} vector encoding")
#     else:
#         print(f"❌ {person_name}: không có encoding hợp lệ")
#
# # === LƯU MÔ HÌNH ===
# with open(model_output, "wb") as f:
#     pickle.dump((known_face_encodings, known_face_names), f)
#
# # === TỔNG KẾT ===
# print("\n🎉 Huấn luyện hoàn tất!")
# print(f"📦 Mô hình đã lưu vào: {model_output}")
# print(f"👥 Tổng số người: {len(set(known_face_names))}")
# for name in set(known_face_names):
#     print(f"   - {name}: {known_face_names.count(name)} vector")
# print(f"🧠 Tổng số vector: {len(known_face_encodings)}")

import os
import face_recognition
import pickle
import numpy as np
import random
from PIL import Image
import cv2

# === CẤU HÌNH ===
data_dir = "../../dataimage_filtered_clean"  # Thư mục chứa ảnh huấn luyện
model_output = "../models/face_model.pkl"
max_encodings_per_person = 100  # Số encoding tối đa mỗi người

# === BIẾN LƯU TRỮ ===
known_face_encodings = []
known_face_names = []

print("🚀 Bắt đầu huấn luyện mô hình nhận diện khuôn mặt...")
print(f"📁 Đọc dữ liệu từ thư mục: {data_dir}")

# === VÒNG LẶP QUA TỪNG NGƯỜI ===
for person_name in os.listdir(data_dir):
    person_dir = os.path.join(data_dir, person_name)
    if not os.path.isdir(person_dir):
        continue

    print(f"\n🧠 Xử lý người: {person_name}")
    encodings = []

    for img_name in os.listdir(person_dir):
        img_path = os.path.join(person_dir, img_name)

        try:
            # Đọc và resize nhẹ ảnh để ổn định
            image = face_recognition.load_image_file(img_path)

            # Phát hiện khuôn mặt
            face_locs = face_recognition.face_locations(image)
            if len(face_locs) != 1:
                print(f"⚠️ Bỏ qua {img_name} – phát hiện {len(face_locs)} khuôn mặt")
                continue

            face_enc = face_recognition.face_encodings(image, face_locs)
            if face_enc:
                encodings.append(face_enc[0])

        except Exception as e:
            print(f"⚠️ Lỗi khi xử lý ảnh {img_name}: {e}")

    if encodings:
        # Random + giới hạn số lượng encoding/người
        random.shuffle(encodings)
        selected_encodings = encodings[:min(max_encodings_per_person, len(encodings))]

        known_face_encodings.extend(selected_encodings)
        known_face_names.extend([person_name] * len(selected_encodings))

        print(f"✅ {person_name}: đã lưu {len(selected_encodings)} vector encoding")
    else:
        print(f"❌ {person_name}: không có encoding hợp lệ nào")

# === LƯU MÔ HÌNH RA FILE ===
with open(model_output, "wb") as f:
    pickle.dump((known_face_encodings, known_face_names), f)

# === TỔNG KẾT ===
print("\n🎉 Huấn luyện hoàn tất!")
print(f"📦 Mô hình đã lưu tại: {model_output}")
print(f"👥 Tổng số người: {len(set(known_face_names))}")
for name in set(known_face_names):
    print(f"   - {name}: {known_face_names.count(name)} vector")
print(f"🧠 Tổng số vector encoding: {len(known_face_encodings)}")




