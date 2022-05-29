"""
用摄像头捕捉人脸

"""
import traceback
import cv2
import numpy as np
# 人脸识别
face_detector=cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_frontalface_default.xml')
# 眼睛捕捉
eye_cascade = cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_eye.xml')
# 笑容捕捉
smile_cascade = cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_smile.xml')

def CatchData_from_Video():
    # 从摄像头中捕捉人脸数据
    cap=cv2.VideoCapture(0)# 这是调用笔记本电脑内置的摄像头
    # cnt记录图片张数
    cnt=0
    while True:
        success,img=cap.read()
        # 转为灰度图片
        gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
        # 检测人脸
        faces=face_detector.detectMultiScale(gray,1.3,5)

        for (x,y,w,h) in faces:
            cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0))
            cnt+=1
            cv2.imwrite('./MyData/'+str(cnt)+'me.jpg',gray[y:y+h,x:x+w])
            cv2.imshow('image',img)

        # 保持画面持续
        k=cv2.waitKey(1)

        if k==27:
            # 通过esc退出
            break
        elif cnt>=2:
            break
    # 关闭摄像头
    cap.release()
    cv2.destroyAllWindows()
CatchData_from_Video()



