#########################################################################
#  #         手动实现FNN实验：
#  #             (1)dropout,激活函数，损失函数修改
#  #             (2)网络层数、内部结点个数修改
#########################################################################
from model import BPNN
from utils import DataLoader

import numpy as np
import matplotlib.pyplot as plt
import argparse
import json
import datetime


def example_1(optimizer,epoch,batch_size=32,lr=0.001,dropout=None,num_layers=4,hidden_nodes=1024,activate='tanh'):
    """
    :param epoch:
    :param batch_size: 批次
    :param lr: 学习率
    :param dropout:
    :param num_layers: 层数
    :param hidden_nodes: 隐层结点数
    :return:
    """
    # 加载数据
    #初始化层的结点数
    unit_list=[]
    unit_list.append(32*32)
    for i in range(num_layers-2):
        unit_list.append(hidden_nodes)
    unit_list.append(10)

    train_data=DataLoader(batch_size=batch_size,data_type='train',scale=False)
    test_data=DataLoader(batch_size=batch_size,data_type='test',scale=False)
    # 定义神经网络的时候，需要申明batch_size
    model=BPNN(num_layers=num_layers,unit_list=unit_list,initializer='xavier',optimizer=optimizer,batch_size=batch_size,dropout=dropout,activate=activate)

    # 训练
    loss_dict=model.train(train_data,train_data,epoch=epoch,lr=lr,dropout_prob=dropout)# 这里测试集就是验证集
    plt.plot(loss_dict['train_loss'])
    print("loss:",loss_dict['train_loss'])
    #plt.savefig("1.png")
    # 显示损失曲线
    filename = "./log/" + "BP-scratch-" + datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + '.jpg'
    plt.savefig(filename)
    # 问题
    h,label=model.predict(test_data)

    acc=np.sum(h==label)/len(label)

    print("*"*50)
    print("测试集上的正确率为{:.3f}".format(acc))
    return acc

####################################################################################################################
##  以下是部分比较难跑的实验的记录
##  主要是一开始忘记把作图加上了，所以输出的是list，只好手动加了一下
##  这一版的代码中添加了绘图的部分,下面是冗余哈~~~
##################################################################################################################

if __name__=="__main__":
    #show100_ada()
    # 命令行调用
    parser=argparse.ArgumentParser()
    parser.add_argument("--batch_size",default=64,type=int)
    parser.add_argument("--epoch",default=280,type=int)
    parser.add_argument("--num_layers",default=4,type=int)
    parser.add_argument("--hidden_nodes",default=505,type=int)
    parser.add_argument("--dropout",default=0.3,type=float)
    parser.add_argument("--optimizer",default="rmsprop",type=str)
    parser.add_argument("--lr",default=0.01,type=float)
    parser.add_argument("--loss_func",default="mle",type=str)
    parser.add_argument("--activate",default="sigmoid",type=str)
    parser.add_argument("--acc",default=0.0,type=float)
    args=parser.parse_args()

    #实验模块
    print("*"*50)
    args.acc = example_1(epoch=args.epoch,batch_size=args.batch_size,dropout=args.dropout,lr=args.lr,num_layers=args.num_layers,optimizer=args.optimizer,activate=args.activate)

    #记录实验设置的参数以及结果
    with open('./log/bp_scratch_args.txt', 'a') as f:
        json.dump(args.__dict__, f, indent=2)

    print('参数保存成功')



