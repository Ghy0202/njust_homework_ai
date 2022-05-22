###########################################################
#       基于torch的FNN
#       修改层数，结点
#       修改优化器，dropout
############################################################
import numpy as np
import torch
import torchvision
import torchvision.transforms as transforms
import torch.nn.functional as F
import torch.optim as optim
import torch.nn as nn
import datetime
# 可视化与进度条
import json
from tqdm import tqdm
import matplotlib.pyplot as plt
import argparse
plt.rcParams['font.sans-serif'] = ['SimHei'] # 指定默认字体
plt.rcParams['axes.unicode_minus']=False     # 正常显示负号

# 定义BP神经网络
class FNN(nn.Module):
    """初始化参数"""
    def __init__(self,dropout=0.1,hidden_nodes=2048,numlayers=2,activate='relu'):
        '''三层网络,四层网络'''
        super(FNN,self).__init__()
        self.numlayers=numlayers
        self.activate=activate
        # 定义不同的层数
        if numlayers==3:
            self.fc1 = nn.Linear(32 * 32 * 3, hidden_nodes)
            self.fc2 = nn.Linear(hidden_nodes,hidden_nodes)
            self.fc3 = nn.Linear(hidden_nodes, 10)
        elif numlayers==2:
            self.fc1 = nn.Linear(32 * 32 * 3, hidden_nodes)
            self.fc2 = nn.Linear(hidden_nodes, 10)
        else:
            RuntimeError("numlayers must choose from 2 or 3")

        if dropout !=0:
            # 设置dropout
            self.dropout=nn.Dropout(dropout)
        else:
            self.dropout=None

    def forward(self,x):
        x=x.view(-1,32*32*3)# 展平处理
        if self.numlayers==3:
            x=self.fc1(x)
            if self.dropout is not None:
                x=self.dropout(x)
            x = F.relu(x)
            x=self.fc2(x)
            if self.dropout is not None:
                x=self.dropout(x)
            x = F.relu(x)
            x = self.fc3(x)
        else:
            x=self.fc1(x)
            if self.dropout is not None:
                x=self.dropout(x)
            x = F.relu(x)
            x = self.fc2(x)
        return x


def test_FNN(lr=0.001,EPOCH=1,BATCH_SIZE=4,hidden_nodes=2048,num_layers=2,dropout=0.1,optimizer='sgd'):

    '''Step 1: 加载本地的cifar数据集&数据预处理'''

    transform = transforms.Compose([transforms.Grayscale(num_output_channels=3),#去掉的话是彩色数据
                                    transforms.ToTensor(),
                                    transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])]
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
    net=FNN(hidden_nodes=hidden_nodes,numlayers=num_layers,dropout=dropout)
    if optimizer=='sgd':
        optimizer=optim.SGD(net.parameters(),lr=lr,momentum=0.9)
    if optimizer=='adam':
        optimizer=optim.Adam(net.parameters(),lr=lr)
    if optimizer=='resprop':
        optimizer=optim.rmsprop.RMSprop(net.parameters())
    else:
        optimizer=optim.Adagrad(net.parameters())
    loss_func=torch.nn.CrossEntropyLoss()#采用的是交叉熵损失

    all_loss=[]

    '''Step2:训练'''
    for epoch in range(EPOCH):
        cur_loss=0.0
        print("\n第",epoch+1,"次训练")
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
    filename = "./log/" + "BPNN-" + datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + '.jpg'
    plt.savefig(filename)
    '''Step3:预测'''
    dataiter=iter(trainloader)
    images,labels=dataiter.next()
    predicts=net.forward(images)
    correct=0
    total=0
    with torch.no_grad():
        for (images,labels) in  testloader:
            #print("images",images)
            #print("labels",labels)
            outputs=net(images)

            numbers,predicted=torch.max(outputs.data,1)
            total+=labels.size(0)
            correct+=(predicted==labels).sum().item()
    print("*"*50)
    print('FNN测试集的正确率为:{:.2f}%'.format(100*correct/total))
    return correct*1.0/total


#测试
if __name__ == '__main__':
    # 命令行调用
    parser = argparse.ArgumentParser()
    parser.add_argument("--batch_size", default=32, type=int)
    parser.add_argument("--epoch", default=1, type=int)
    parser.add_argument("--num_layers", default=2,help="2 or 3", type=int)
    parser.add_argument("--hidden_nodes", default=1024, type=int)
    parser.add_argument("--dropout", default=0.1, type=float)
    parser.add_argument("--optimizer", default="adam", type=str)
    parser.add_argument("--lr", default=0.001, type=float)
    parser.add_argument("--loss_func", default="cross_entropy", type=str)
    parser.add_argument("--acc", default=0.0, type=float)
    args = parser.parse_args()

    # 实验模块
    print("*" * 50)
    args.acc =test_FNN(EPOCH=args.epoch,BATCH_SIZE=args.batch_size,lr=args.lr,hidden_nodes=args.hidden_nodes,dropout=args.dropout,num_layers=args.num_layers,optimizer=args.optimizer)# 可以手动设置迭代次数以及学习率
    # 记录实验设置的参数以及结果
    with open('./log/bp_nn_args.txt', 'a') as f:
        json.dump(args.__dict__, f, indent=2)

    print('参数保存成功')




