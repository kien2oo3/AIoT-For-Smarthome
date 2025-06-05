import os
import cv2
import face_recognition
import shutil
import hashlib
import numpy as np
from glob import glob

# === Cấu hình ===
source_base = "dataimage"
target_base = "dataimage_filtered_clean"
os.makedirs(target_base, exist_ok=True)

MAX_IMAGES_PER_PERSON = 60  # Số ảnh đa dạng cần giữ lại
DIVERSITY_THRESHOLD = 0.25  # Ngưỡng khoảng cách tối thiểu giữa các encoding

# === Tính hash để lọc ảnh trùng nội dung ===
def calculate_image_hash(image_path):
    with open(image_path, 'rb') as f:
        return hashlib.md5(f.read()).hexdigest()

# === Lọc từng người ===
person_dirs = [d for d in os.listdir(source_base) if os.path.isdir(os.path.join(source_base, d))]

for person in person_dirs:
    src_dir = os.path.join(source_base, person)
    tgt_dir = os.path.join(target_base, f"Authorized_{person}")
    os.makedirs(tgt_dir, exist_ok=True)

    image_paths = glob(os.path.join(src_dir, "*.jpg"))
    seen_hashes = set()
    selected_encodings = []
    selected_paths = []

    for img_path in image_paths:
        img = cv2.imread(img_path)
        if img is None or img.shape[0] < 100 or img.shape[1] < 100:
            continue

        img_hash = calculate_image_hash(img_path)
        if img_hash in seen_hashes:
            continue
        seen_hashes.add(img_hash)

        rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        face_locations = face_recognition.face_locations(rgb, model='hog')
        if len(face_locations) != 1:
            continue

        encs = face_recognition.face_encodings(rgb)
        if not encs:
            continue
        enc = encs[0]

        # Đảm bảo encoding này khác biệt với các encoding đã chọn
        if all(np.linalg.norm(enc - prev) > DIVERSITY_THRESHOLD for prev in selected_encodings):
            selected_encodings.append(enc)
            selected_paths.append(img_path)

        if len(selected_paths) >= MAX_IMAGES_PER_PERSON:
            break

    for img_path in selected_paths:
        shutil.copy(img_path, os.path.join(tgt_dir, os.path.basename(img_path)))

    print(f"{person}: {len(image_paths)} ảnh gốc → {len(selected_paths)} ảnh chất lượng & đa dạng đã chọn.")
