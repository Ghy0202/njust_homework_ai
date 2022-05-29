"""基于GPU：Update以及优化时间复杂度"""
from torch.utils.data import DataLoader
from models import FaceCNN
import torch.nn as nn
from torch import optim
import torch
from torch.utils import data
import numpy as np
from sklearn.model_selection import train_test_split
# 记录时间
import datetime
import time
# 绘图
import matplotlib.pyplot as plt

PATH="./data"
# 检测cuda是否可用
cuda_available=torch.cuda.is_available()
print("CUDA:",cuda_available)

# 重写Dataset类
class vectorDataset(data.Dataset):
    # 初始化
    # 初始化
    def __init__(self,type="train"):
        super(vectorDataset, self).__init__()
        data = np.loadtxt("./data/features.txt", dtype=float, delimiter=',')
        labels=np.loadtxt("./data/label.csv",dtype=int,delimiter=',')
        # print(data)
        # print(labels.shape)
        train_x,test_x,train_y,test_y=train_test_split(data,labels,test_size=0.2)
        if(type=="train"):
            self.features=train_x
            self.labels=train_y
        else:
            self.features=test_x
            self.labels=test_y


    #读取某一图片，item为索引号
    def __getitem__(self,item):
        face=self.features[item]
        # 像素值标准化
        face_normalized = face.reshape(1, 48, 48) / 255.0  # 为与pytorch中卷积神经网络API的设计相适配，需reshape原图
        face_tensor = torch.from_numpy(face_normalized)  # 将python中的numpy数据类型转化为pytorch中的tensor数据类型
        face_tensor = face_tensor.type('torch.FloatTensor')  # 指定为'torch.FloatTensor'型，否则送进模型后会因数据类型不匹配而报错
        label = self.labels[item]
        return face_tensor, label

    def __len__(self):
        return self.labels.shape[0]

# 模型训练
def validate(model, dataset, batch_size):
    val_loader = data.DataLoader(dataset, batch_size)
    result, num = 0.0, 0
    for images, labels in val_loader:
        if cuda_available:
            images = images.cuda()
            labels = labels.cuda()
        pred = model.forward(images)
        pred = np.argmax(pred.data.numpy(), axis=1)
        labels = labels.data.numpy()
        result += np.sum((pred == labels))
        num += len(images)
    acc = result / num
    return acc

def train(epochs=40,batch_size=128,lr=0.01,weight_decay=0,root=PATH):
    """训练模型"""
    # 加载数据集
    train_dataset=vectorDataset()
    val_dataset=vectorDataset("val")
    # 载入数据并且分割batch
    train_loader=DataLoader(train_dataset,batch_size)
    val_loader = DataLoader(val_dataset, batch_size)
    # 模型
    if cuda_available:
        model = FaceCNN().cuda()
    else:
        model = FaceCNN()
    # 损失函数
    loss_function=nn.CrossEntropyLoss()
    # 优化器
    # optimizer=optim.SGD(model.parameters(),lr=lr,weight_decay=weight_decay)
    optimizer=optim.Adam(model.parameters(),lr=lr,weight_decay=weight_decay)
    # 可视化
    losses=[]
    # 按照批次训练模型
    for epoch in range(epochs):
        loss=0
        model.train()
        for images,labels in train_loader:
            # 判断数据是否需要
            if cuda_available:
                images=images.cuda()
                labels=labels.cuda()
            # 梯度清零
            optimizer.zero_grad()
            # 前向传播
            print(images.size())
            output=model.forward(images)
            print(output.size())
            print(output)
            # 误差计算
            #print(output.dtype,labels.dtype)
            loss+=loss_function(output,labels.long()).item()
            # 误差的反向传播
            loss_function(output, labels.long()).backward()
            # 更新参数
            optimizer.step()
        losses.append(loss)
        # 每一轮的损失
        print('After {} epochs , the loss_rate is : '.format(epoch + 1), loss)
        if epoch % 5 == 0:
            model.eval()  # 模型评估
            acc_train = validate(model, train_dataset, batch_size)
            acc_val = validate(model, val_dataset, batch_size)
            print('After {} epochs , the acc_train is : '.format(epoch + 1), acc_train)
            print('After {} epochs , the acc_val is : '.format(epoch + 1), acc_val)
    # 绘制图
    plt.plot(losses,c='r')
    now=datetime.datetime.now().date()
    plt.savefig("./imag/"+str(now)+".jpg")
    # 模型保存
    torch.save(model,"./model_data/sw.pkl")

train()