import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import classification_report, confusion_matrix
import joblib

# 1. Đọc dữ liệu
df = pd.read_csv("sensor_data.csv")

# 2. Tách features và label
X = df[["Flame_State", "CO_ppm", "Temperature", "Humidity"]]
y = df["Label"]

# 3. Chia train/test (80/20)
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 4. Khởi tạo và huấn luyện mô hình
model = DecisionTreeClassifier(max_depth=5, random_state=42)
model.fit(X_train, y_train)

# 5. Dự đoán và đánh giá
y_pred = model.predict(X_test)
print("🎯 Classification Report:")
print(classification_report(y_test, y_pred))

print("🌀 Confusion Matrix:")
print(confusion_matrix(y_test, y_pred))

# 6. Lưu mô hình
joblib.dump(model, "flame_ai_model.pkl")
print("✅ Mô hình đã được lưu thành 'flame_ai_model.pkl'")
