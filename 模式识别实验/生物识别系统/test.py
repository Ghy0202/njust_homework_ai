"""调用摄像头：运用训练好的CNN模型"""
import traceback

import cv2
import torch
from models import FaceCNN
import numpy as np
# 人脸识别
face_detector=cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_frontalface_default.xml')
# 眼睛捕捉
eye_cascade = cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_eye.xml')
# 笑容捕捉
smile_cascade = cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_smile.xml')


def OpenVideoTest(bs=128):

    cap = cv2.VideoCapture(0)
    model = torch.load("./model_data/face_cnn2.pkl")
    model.eval()
    print("模型加载成功！")
    while cap.isOpened():
        ret, frame = cap.read()
        #cv2.imshow('windowName', frame)

        # 获取变量
        height, width, channel = frame.shape
        gray_image = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        FONT = cv2.FONT_HERSHEY_SIMPLEX
        FONT_SCALE = 0.8
        FONT_THICKNESS = 2
        lable_color = (10, 10, 255)
        lable = "Emotion Detection made by GHY"
        lable_dimension = cv2.getTextSize(lable, FONT, FONT_SCALE, FONT_THICKNESS)[0]
        textX = int((frame.shape[1] - lable_dimension[0]) / 2)
        textY = int((frame.shape[0] + lable_dimension[1]) / 2)
        cv2.putText(frame, lable, (5,50 ), FONT, FONT_SCALE, (0, 0, 0), FONT_THICKNESS)
        faces = face_detector.detectMultiScale(gray_image)

        try:
            for (x, y, w, h) in faces:
                cv2.rectangle(frame, pt1=(x, y), pt2=(x + w, y + h), color=(255, 0, 0), thickness=2)
                roi_gray = gray_image[y - 5:y + h + 5, x - 5:x + w + 5]
                roi_gray = cv2.resize(roi_gray, (48, 48))
                image_pixels = roi_gray
                image_pixels = np.expand_dims(image_pixels, axis=0)
                image_pixels=image_pixels.transpose()
                image_pixels = image_pixels.reshape(1, 48, 48) / 255.0
                image_list=[]
                for i in range(bs):
                    image_list.append(image_pixels)
                image_pixels=np.asarray(image_list)
                image_pixels=torch.from_numpy(image_pixels)
                image_pixels=torch.tensor(image_pixels,dtype=torch.float32)
                print(image_pixels.size())
                # # TODO:这里是加载模型
                predictions = model.forward(image_pixels)
                print(type(predictions),predictions.size())
                predictions=predictions.detach().numpy()
                max_index = np.argmax(predictions[0])
                print(max_index)
                emotion_detection = ['angry', 'disgust', 'fear', 'happy', 'sad', 'surprise', 'neutral']
                emotion_prediction = emotion_detection[max_index]
                # # 将结果写到图片上
                cv2.putText(frame, "Sentiment: {}".format(emotion_prediction), (0, textY + 22 + 5), FONT, 0.7, lable_color, 2)
                lable_violation = 'Confidence: {}'.format(str(np.round(np.max(predictions[0]) , 1)) + "%")
                violation_text_dimension = cv2.getTextSize(lable_violation, FONT, FONT_SCALE, FONT_THICKNESS)[0]
                violation_x_axis = int(frame.shape[1] - violation_text_dimension[0])
                cv2.putText(frame, lable_violation, (violation_x_axis, textY + 22 + 5), FONT, 0.7, lable_color, 2)
        except (Exception, BaseException) as e:
            print(e)
            exstr = traceback.format_exc()
            print(exstr)
            pass

        cv2.imshow("CNN",frame)
        if cv2.waitKey(1) == ord('q'):
            break
        # 点击窗口关闭按钮退出程序
        if cv2.getWindowProperty('CNN', cv2.WND_PROP_AUTOSIZE) < 1:
            break


    cap.release()
    cv2.destroyAllWindows()

OpenVideoTest()
def TestModel():
    # 查看模型是否可以正常调用
    from all import vectorDataset, validate
    model = torch.load("./model_data/face_cnn2.pkl")
    model.eval()
    print("模型加载成功！")
    val_dataset = vectorDataset("val")
    acc_val=validate(model,val_dataset,128)
    print('Test the acc_val is : ', acc_val)







