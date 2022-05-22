"""
    基于Torch的CNN模型
    超参数说明：
    (1)batchsize：设置大一点是为了跑得快一些
    (2)dropout:防止过拟合
    (3)优化器：SGD以及Adam等
    (4)loss记录放在./log文件夹下
"""
import numpy as np
import torch
import torchvision
import torchvision.transforms as transforms
import torch.nn.functional as F
import torch.optim as optim
import torch.nn as nn
import argparse
import datetime
# 可视化与进度条
import json
from tqdm import tqdm
import matplotlib.pyplot as plt

class CNN(nn.Module):
    """初始化参数"""
    def __init__(self,stride=1,padding=0,dropout=0.1):
        super(CNN,self).__init__()
        self.conv1=torch.nn.Conv2d(in_channels=3,
                                   out_channels=64,
                                   kernel_size=5,
                                   stride=stride,
                                   padding=padding)
        self.pool=torch.nn.MaxPool2d(kernel_size=3,
                                     stride=stride*2)
        self.conv2=torch.nn.Conv2d(64,64,5)
        self.fc1=torch.nn.Linear(64*4*4,384)
        self.fc2=torch.nn.Linear(384,192)
        self.fc3=torch.nn.Linear(192,10)
        if dropout !=0:
            # 设置dropout
            self.dropout=nn.Dropout(dropout)
        else:
            self.dropout=None

    def forward(self,x):
        x=self.pool(F.relu(self.conv1(x)))
        x=self.pool(F.relu(self.conv2(x)))
        x=x.view(-1,64*4*4)
        x=self.fc1(x)
        if self.dropout is not None:
            x = self.dropout(x)
        x=F.relu(x)
        x=self.fc2(x)
        if self.dropout is not None:
            x = self.dropout(x)
        x=F.relu(x)
        x=self.fc3(x)
        return x

def test_CNN(stride=1,padding=0,lr=0.001,EPOCH=1,BATCH_SIZE=4,dropout=0.1):

    '''Step 1: 加载本地的cifar数据集&数据预处理'''
    # If num_output_channels == 3 : returned image is 3 channel with r == g == b
    transform = transforms.Compose([transforms.Grayscale(num_output_channels=3),
                                    transforms.ToTensor(),
                                    transforms.Normalize(mean=[0.485, 0.456, 0.406],std=[0.229, 0.224, 0.225])]
                                   )

    # 划分训练集
    trainset = torchvision.datasets.CIFAR10(root='D:/homework_for_ML/task4/data', train=True,
                                            download=False, transform=transform)


    trainloader = torch.utils.data.DataLoader(
        trainset, batch_size=BATCH_SIZE, shuffle=True, num_workers=1
    )
    # 划分测试集
    testset = torchvision.datasets.CIFAR10(root='D:/homework_for_ML/task4/data', train=False,
                                           download=False, transform=transform)
    testloader = torch.utils.data.DataLoader(
        testset, batch_size=BATCH_SIZE, shuffle=False, num_workers=1
    )
    print("加载数据集结束~")
    # 类别标签的定义
    classes = ('plane', 'car', 'bird', 'cat',
               'deer', 'dog', 'frog', 'horse', 'ship', 'truck')
    net=CNN(stride=stride,padding=padding)
    optimizer=optim.SGD(net.parameters(),lr=lr,momentum=0.9)
    loss_func=torch.nn.CrossEntropyLoss()#采用的是交叉熵损失
    all_loss=[]

    '''Step2:训练'''
    for epoch in range(EPOCH):
        cur_loss=0.0
        print("第",epoch+1,"次训练")
        for step,data in enumerate(tqdm(trainloader)):
            bx,by=data # 特征bx 标签by

            outputs=net.forward(bx)
            loss=loss_func(outputs,by)
            optimizer.zero_grad()# 梯度清零
            loss.backward()
            optimizer.step()
            cur_loss+=loss.item()
            if step%100==0 and step!=0:
                # 显示损失
                #print("当前损失：",cur_loss/100)
                all_loss.append(cur_loss/100)
                cur_loss=0.0


    print("结束训练")
    #显示损失曲线
    plt.plot(all_loss)
    #plt.show()
    filename = "./log/" + "CNN-" + datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + '.jpg'
    plt.savefig(filename)
    '''Step3:预测'''

    correct=0
    total=0
    step=0
    with torch.no_grad():
        for (images,labels) in testloader:
            outputs=net(images)
            # 测试
            step+=1
            numbers, predicted = torch.max(outputs.data, 1)
            if step%100==0:
                imshow(torchvision.utils.make_grid(images),predicted,labels)

            total+=labels.size(0)
            correct+=(predicted==labels).sum().item()
    print("*"*50)
    print('CNN测试集的正确率为:{:.2f}%'.format(100*correct/total))
    return 1.0*correct/total

def imshow(img,labels,real_labels=None):
    """
    :param img:
    :return:
    """
    classes = ('plane', 'car', 'bird', 'cat',
               'deer', 'dog', 'frog', 'horse', 'ship', 'truck')
    img=img/2+0.5
    npimg=img.numpy()
    plt.imshow(np.transpose(npimg,(1,2,0)))
    print(' '.join('%5s' % classes[labels[j]] for j in range(16)))
    print(' '.join('%5s' % classes[real_labels[j]] for j in range(16)))
    plt.show()

def show():
    classes = ('plane', 'car', 'bird', 'cat',
               'deer', 'dog', 'frog', 'horse', 'ship', 'truck')
    # 加载数据集合
    transform = transforms.Compose([
                                    transforms.Grayscale(num_output_channels=3),
                                    transforms.ToTensor(),
                                    transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
                                 ]
                                   )
    testset = torchvision.datasets.CIFAR10(root='D:/homework_for_ML/task4/data', train=False,
                                           download=False, transform=transform)
    testloader = torch.utils.data.DataLoader(
        testset, batch_size=16, shuffle=False, num_workers=1
    )
    dataiter = iter(testloader)
    images, labels = dataiter.next()

    print(' '.join('%5s' % classes[labels[j]] for j in range(16)))
    imshow(torchvision.utils.make_grid(images),labels)




    #测试
if __name__ == '__main__':
    # 显示图片
    #show()
    # 命令行调用

    parser=argparse.ArgumentParser()
    parser.add_argument("--batch_size", default=64, type=int)
    parser.add_argument("--epoch", default=1, type=int)
    parser.add_argument("--dropout", default=0, type=float)
    parser.add_argument("--lr", default=0.001, type=float)
    parser.add_argument("--stride",default=1,type=int)
    parser.add_argument("--padding",default=0,type=int)
    parser.add_argument("--acc",default=0,type=float)
    args = parser.parse_args()
    acc=test_CNN(lr=args.lr,BATCH_SIZE=args.batch_size,EPOCH=args.epoch,dropout=args.dropout)
    args.acc=acc
    # 记录实验设置的参数以及结果
    with open('./log/cnn_args.txt', 'a') as f:
        json.dump(args.__dict__, f, indent=2)

    print('参数保存成功')











