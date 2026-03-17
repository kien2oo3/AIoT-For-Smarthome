import speech_recognition as sr
from gtts import gTTS
import requests
import os
import time
import uuid
# import pygame
from playsound import playsound
from blynk_controller import get_temp_for_blynk,get_humi_for_blynk, get_co_for_blynk, control_fan_for_blynk, control_heat_lamp_for_blynk, auto_control_heat_lamp_for_blynk, control_hallway_lamp_for_blynk, auto_control_hallway_lamp_for_blynk
from datetime import datetime


# Hàm phát giọng nói Tiếng Việt
def speak(text):
    filename = f"voice_{uuid.uuid4()}.mp3"
    tts = gTTS(text=text, lang='vi')
    tts.save(filename)
    playsound(filename)
    os.remove(filename)


# Hàm nhận lệnh giọng nói
def listen_command():
    r = sr.Recognizer()
    with sr.Microphone() as source:
        print("\U0001F3A4 Đang lắng nghe...")
        audio = r.listen(source)
    try:
        command = r.recognize_google(audio, language="vi-VN")
        print(f"Bạn nói: {command}")
        return command.lower()
    except:
        print("\u26A0\uFE0F Không nghe rõ, thử lại nhé!")
        return ""

# Hàm check khuôn mặt thành công thì chào
def check_face_event():
    if os.path.exists("trigger.txt"):
        with open("trigger.txt", "r") as f:
            data = f.read()
        if data.startswith("GREET:"):
            owner_name = data.split(":")[1]
            greet_owner(owner_name)
            os.remove("trigger.txt")
        elif data.startswith("ALERT:"):
            owner_name = data.split(":")[1]
            greet_owner(owner_name)
            os.remove("trigger.txt")

# Hàm chào hỏi khi nhận diện khuôn mặt
def greet_owner(name):
    if name != "Mat gia!" and name !="Nguoi la!":
        text = f"Chào mừng anh {name} đẹp trai đã về nhà!"
        speak(text)
        # Tắt đèn sưởi thông minh trước
        status1 = auto_control_heat_lamp_for_blynk(0)
        if status1 == "ok":
            status = control_heat_lamp_for_blynk(150)
            if status == "ok":
                speak(f"Em đã bật đèn cho anh rồi nha!")
                print(f"Em đã bật đèn cho anh rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh bật đèn ạ!")
                print("Em chưa thể gửi lệnh bật đèn ạ!")
        else:
            speak("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
            print("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
    else:
        text = f"Bạn đang giả mạo khuôn mặt của chủ nhà, tôi đã thông báo cho chủ nhà"
        speak(text)

# Main loop
if __name__ == "__main__":
    while True:
        check_face_event()
        command = listen_command()
        if "siri" in command:
            speak("Em chào anh, em là Siri!")
        elif "bây giờ là mấy giờ" in command:
            currentTime = datetime.now()
            result = f"{currentTime.hour} giờ {currentTime.minute} phút!"
            speak(f"Dạ thưa anh bây giờ là {result}")
            print(f"Dạ thưa anh bây giờ là {result}")
        elif "nhiệt độ hiện tại" in command:
            temp = get_temp_for_blynk()
            if temp is not None:
                speak(f"Nhiệt độ phòng khách hiện tại là: {temp} độ C!")
                print(f"Nhiệt độ phòng khách hiện tại là: {temp} độ C!")
            else:
                speak("Em chưa thể lấy dữ liệu nhiệt độ phòng khách!")
                print("Em chưa thể lấy dữ liệu nhiệt độ phòng khách!")
        elif "độ ẩm hiện tại" in command:
            humi = get_humi_for_blynk()
            if humi is not None:
                speak(f"Độ ẩm phòng khách hiện tại là: {humi} %!")
                print(f"Độ ẩm phòng khách hiện tại là: {humi} %!")
            else:
                speak("Em chưa thể lấy dữ liệu độ ẩm phòng khách!")
                print("Em chưa thể lấy dữ liệu độ ẩm phòng khách!")
        elif "khí co" in command:
            coValue = get_co_for_blynk()
            if coValue is not None:
                speak(f"Khí Cê Ô trong phòng hiện tại là: {coValue} PPM!")
                print(f"Khí Cê Ô trong phòng hiện tại là: {coValue} PPM!")
            else:
                speak("Em chưa thể lấy dữ liệu khí CO trong phòng!")
                print("Em chưa thể lấy dữ liệu khí CO trong phòng!")
        elif "bật quạt chế độ lớn" in command:
            status = control_fan_for_blynk(255)
            if status == "ok":
                speak(f"Em đã bật quạt chế độ lớn rồi nha!")
                print(f"Em đã bật quạt chế độ lớn rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh bật quạt ạ!")
                print("Em chưa thể gửi lệnh bật quạt ạ!")
        elif "bật quạt" in command:
            status = control_fan_for_blynk(100)
            if status == "ok":
                speak(f"Em đã bật quạt chế độ trung bình rồi nha!")
                print(f"Em đã bật quạt chế độ trung bình rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh bật quạt ạ!")
                print("Em chưa thể gửi lệnh bật quạt ạ!")
        elif "tắt quạt" in command:
            status = control_fan_for_blynk(0)
            if status == "ok":
                speak(f"Em đã tắt quạt rồi nha!")
                print(f"Em đã tắt quạt rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh tắt quạt ạ!")
                print("Em chưa thể gửi lệnh tắt quạt ạ!")
        elif "bật đèn sưởi thông minh" in command:
            status = auto_control_heat_lamp_for_blynk(1)
            if status == "ok":
                speak(f"Em đã bật đèn sưởi chế độ thông minh rồi nha!")
                print(f"Em đã bật đèn sưởi chế độ thông minh rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh bật đèn sưởi thông minh ạ!")
                print("Em chưa thể gửi lệnh bật đèn sưởi thông minh ạ!")
        elif "tắt đèn sưởi thông minh" in command:
            status = auto_control_heat_lamp_for_blynk(0)
            if status == "ok":
                speak(f"Em đã tắt đèn sưởi chế độ thông minh rồi nha!")
                print(f"Em đã tắt đèn sưởi chế độ thông minh rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh tắt đèn sưởi thông minh ạ!")
                print("Em chưa thể gửi lệnh tắt đèn sưởi thông minh ạ!")
        elif "bật đèn sưởi chế độ lớn" in command:
            # Tắt đèn sưởi thông minh trước
            status1 = auto_control_heat_lamp_for_blynk(0)
            if status1 == "ok":
                status = control_heat_lamp_for_blynk(150)
                if status == "ok":
                    speak(f"Em đã bật đèn sưởi chế độ lớn rồi nha!")
                    print(f"Em đã bật đèn sưởi chế độ lớn rồi nha!")
                else:
                    speak("Em chưa thể gửi lệnh bật đèn sưởi ạ!")
                    print("Em chưa thể gửi lệnh bật đèn sưởi ạ!")
            else:
                speak("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
                print("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
        elif "bật đèn sưởi" in command:
            # Tắt đèn sưởi thông minh trước
            status1 = auto_control_heat_lamp_for_blynk(0)
            if status1 == "ok":
                status = control_heat_lamp_for_blynk(100)
                if status == "ok":
                    speak(f"Em đã bật đèn sưởi chế độ trung bình rồi nha!")
                    print(f"Em đã bật đèn sưởi chế độ trung bình rồi nha!")
                else:
                    speak("Em chưa thể gửi lệnh bật đèn sưởi ạ!")
                    print("Em chưa thể gửi lệnh bật đèn sưởi ạ!")
            else:
                speak("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
                print("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
        elif "tắt đèn sưởi" in command:
            # Tắt đèn sưởi thông minh trước
            status1 = auto_control_heat_lamp_for_blynk(0)
            if status1 == "ok":
                status = control_heat_lamp_for_blynk(0)
                if status == "ok":
                    speak(f"Em đã tắt đèn sưởi rồi nha!")
                    print(f"Em đã tắt đèn sưởi rồi nha!")
                else:
                    speak("Em chưa thể gửi lệnh tắt đèn sưởi ạ!")
                    print("Em chưa thể gửi lệnh tắt đèn sưởi ạ!")
            else:
                speak("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
                print("Em chưa thể gửi lệnh tắt đèn sưởi thông minh nên chưa bật được đèn sưởi thủ công!")
        elif "bật đèn hành lang thông minh" in command:
            status = auto_control_hallway_lamp_for_blynk(1)
            if status == "ok":
                speak(f"Em đã bật đèn hành lang chế độ thông minh rồi nha!")
                print(f"Em đã bật đèn hành lang chế độ thông minh rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh bật đèn hành lang thông minh ạ!")
                print("Em chưa thể gửi lệnh bật đèn hành lang thông minh ạ!")
        elif "tắt đèn hành lang thông minh" in command:
            status = auto_control_hallway_lamp_for_blynk(0)
            if status == "ok":
                speak(f"Em đã tắt đèn hành lang chế độ thông minh rồi nha!")
                print(f"Em đã tắt đèn hành lang chế độ thông minh rồi nha!")
            else:
                speak("Em chưa thể gửi lệnh tắt đèn hành lang thông minh ạ!")
                print("Em chưa thể gửi lệnh tắt đèn hành lang thông minh ạ!")
        elif "bật đèn hành lang" in command:
            # Tắt đèn hành lang thông minh trước
            status1 = auto_control_hallway_lamp_for_blynk(0)
            if status1 == "ok":
                status = control_hallway_lamp_for_blynk(1)
                if status == "ok":
                    speak(f"Em đã bật đèn hành lang rồi nha!")
                    print(f"Em đã bật đèn hành lang rồi nha!")
                else:
                    speak("Em chưa thể gửi lệnh bật đèn hành lang ạ!")
                    print("Em chưa thể gửi lệnh bật đèn hành lang ạ!")
            else:
                speak("Em chưa thể gửi lệnh tắt đèn hành lang thông minh nên chưa bật được đèn hành lang thủ công!")
                print("Em chưa thể gửi lệnh tắt đèn hành lang thông minh nên chưa bật được đèn hành lang thủ công!")
        elif "tắt đèn hành lang" in command:
            # Tắt đèn hành lang thông minh trước
            status1 = auto_control_hallway_lamp_for_blynk(0)
            if status1 == "ok":
                status = control_hallway_lamp_for_blynk(0)
                if status == "ok":
                    speak(f"Em đã tắt đèn hành lang rồi nha!")
                    print(f"Em đã tắt đèn hành lang rồi nha!")
                else:
                    speak("Em chưa thể gửi lệnh tắt đèn hành lang ạ!")
                    print("Em chưa thể gửi lệnh tắt đèn hành lang ạ!")
            else:
                speak("Em chưa thể gửi lệnh tắt đèn hành lang thông minh nên chưa bật được đèn hành lang thủ công!")
                print("Em chưa thể gửi lệnh tắt đèn hành lang thông minh nên chưa bật được đèn hành lang thủ công!")
        elif "tạm biệt" in command:
            speak("Chào anh, hẹn gặp lại anh sau nhé!")
            print("Chào anh, hẹn gặp lại anh sau nhé!")
            break
        time.sleep(1)

