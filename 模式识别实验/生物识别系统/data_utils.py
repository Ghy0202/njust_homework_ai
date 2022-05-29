"""
原始数据：.csv
"""
import pandas as pd
import cv2
import numpy as np
import os
import torch
from torch.utils import data

"""读取CSV文件转成图片文件：便于后续可视化"""
DATA_PATH="./data/train.csv"
PATH="./data"
def data_Pre():
    # 处理原始的train.csv文件
    df=pd.read_csv(DATA_PATH)
    df_label=df[['label']]
    df_data=df[['feature']]
    df_label.to_csv('./data/label.csv',index=False,header=False)
    df_data.to_csv('./data/feature.csv',index=False,header=False)
    data=np.loadtxt('./data/feature.csv')

    #指定图片存放路径
    for i in range(data.shape[0]):
        face_array=data[i,:].reshape((48,48))
        cv2.imwrite(PATH+'//'+'{}.jpg'.format(i),face_array)
        print(i)

def toText():
    # 将数据保存为txt
    data = np.loadtxt('./data/feature.csv')
    labels=[]
    features=[]
    for i in range(data.shape[0]):
        print(type(data[i,:]))
        print((data[i,:]).shape)
        features.append(data[i,:])
    np.savetxt("./data/features.txt", features, fmt='%f', delimiter=',')

toText()
def label_Map():
    # 生成图片和对应表情标签
    df_label=pd.read_csv(PATH+"//"+'label.csv',header=None)
    files=os.listdir(PATH)
    jpg_list=[]
    label_list=[]
    for file in files:
        if os.path.splitext(file)[1]=='.jpg':
            jpg_list.append(file)
            index=int(os.path.splitext(file)[0])
            label_list.append(df_label.iat[index,0])

    jpgs=pd.Series(jpg_list)
    labels=pd.Series(label_list)
    df=pd.DataFrame()
    df['path']=jpgs
    df['label']=labels
    df.to_csv(PATH+"//dataset.csv",index=False,header=False)

# 重写Dataset类
class FaceDataset(data.Dataset):
    # 初始化
    # 初始化
    def __init__(self, root):
        super(FaceDataset, self).__init__()
        # root为train或val文件夹的地址
        self.root = root
        # 读取data-label对照表中的内容
        df_path = pd.read_csv(root + "//"+'dataset.csv', header=None, usecols=[0])  # 读取第一列文件名
        df_label = pd.read_csv(root +"//"+ 'dataset.csv', header=None, usecols=[1])  # 读取第二列label
        # 将其中内容放入numpy，方便后期索引
        self.path = np.array(df_path)[:, 0]
        self.label = np.array(df_label)[:, 0]

    #读取某一图片，item为索引号
    def __getitem__(self,item):
        face=cv2.imread(self.root+"//"+self.path[item])
        face_gry=cv2.cvtColor(face,cv2.COLOR_RGB2GRAY)
        # 直方图均衡化
        face_hist = cv2.equalizeHist(face_gry)
        # 像素值标准化
        face_normalized = face_hist.reshape(1, 48, 48) / 255.0  # 为与pytorch中卷积神经网络API的设计相适配，需reshape原图

        face_tensor = torch.from_numpy(face_normalized)  # 将python中的numpy数据类型转化为pytorch中的tensor数据类型
        face_tensor = face_tensor.type('torch.FloatTensor')  # 指定为'torch.FloatTensor'型，否则送进模型后会因数据类型不匹配而报错
        label = self.label[item]
        return face_tensor, label

    def __len__(self):
        return self.path.shape[0]









