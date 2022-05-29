"""模型的训练与保存"""
from data_utils import FaceDataset
from torch.utils.data import DataLoader
from models import FaceCNN
import torch.nn as nn
from torch import optim
import torch
from torch.utils import data
import numpy as np
# 记录时间
import datetime
import time
# 绘图
import matplotlib.pyplot as plt
# 数据地址
DATA_PATH="./data/train.csv"
PATH="./data"
cuda_available=torch.cuda.is_available()
print("CUDA:",cuda_available)

def validate(model, dataset, batch_size):
    val_loader = data.DataLoader(dataset, batch_size)
    result, num = 0.0, 0
    for images, labels in val_loader:
        pred = model.forward(images)
        pred = np.argmax(pred.data.numpy(), axis=1)
        labels = labels.data.numpy()
        result += np.sum((pred == labels))
        num += len(images)
    acc = result / num
    return acc

def train(epochs=45,batch_size=128,lr=0.01,weight_decay=0,root=PATH):
    """训练模型"""
    # 加载数据集
    train_dataset=FaceDataset(root)
    # 载入数据并且分割batch
    train_loader=DataLoader(train_dataset,batch_size)
    # 模型
    if cuda_available:
        model = FaceCNN().cuda()
    else:
        model = FaceCNN()
    # 损失函数
    loss_function=nn.CrossEntropyLoss()
    # 优化器
    optimizer=optim.SGD(model.parameters(),lr=lr,weight_decay=weight_decay)
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
            output=model.forward(images)
            # 误差计算
            loss+=loss_function(output,labels).item()
            # 误差的反向传播
            loss_function(output, labels).backward()
            # 更新参数
            optimizer.step()
        losses.append(loss)
        # 每一轮的损失
        print('After {} epochs , the loss_rate is : '.format(epoch + 1), loss)
        if epoch % 5 == 0:
            model.eval()  # 模型评估
            acc_train = validate(model, train_dataset, batch_size)
            acc_val = validate(model, train_dataset, batch_size)
            print('After {} epochs , the acc_train is : '.format(epoch + 1), acc_train)
            print('After {} epochs , the acc_val is : '.format(epoch + 1), acc_val)
    # 绘制图
    plt.plot(losses,c='r')
    now=datetime.datetime.now().date()
    plt.savefig("./imag/"+str(now)+".jpg")
    # 模型保存
    torch.save(model,"./model_data/face_cnn2.pkl")


if __name__=="__main__":
    train()



