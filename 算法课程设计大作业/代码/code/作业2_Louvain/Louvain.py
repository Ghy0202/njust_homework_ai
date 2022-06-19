"""
贪心算法：
    1、求解划分结果
    2、Modularity取值
    2、Accuracy的计算
"""
# 网络
import networkx as nx
import matplotlib.pyplot as plt
# 工具包
from itertools import permutations
from itertools import combinations
from collections import defaultdict
# 计算算法的时间消耗
import time
import numpy as np


'''LOUVAIN算法实现'''
class Louvain(object):
    """
    输入：带权重的图
    输出：划分结果（字典形式输出）
    """
    def __init__(self,MIN_VALUE=0.0000001):
        self.MIN_VALUE=MIN_VALUE# 判断是否终止的阈值:属于可调参数
        self.nodes_weights={}



    def node2Community(self, graph):
        """
        输入图，生成社区
        :param graph:图
        :return: 返回社区（字典）
        """
        community = {}  # 定义社区字典
        ew = defaultdict(lambda: defaultdict(float))  # 边的权重
        for idx, node in enumerate(graph.nodes()):
            community[node] = idx
            for edge in graph[node].items():
                ew[node][edge[0]] = edge[1]["weight"]

        # community--一个节点对应的社区名称
        # ew--一个结点链接的其他结点以及权重
        return community, ew

    def updateWeights(self,ew):
        """
        更新边的权重
        :param ew:
        :return: 每一个结点相连的节点个数
        """
        nodes_weights=defaultdict(float)

        for node in ew.keys():
            nodes_weights[node]=sum([weight for weight in ew[node].values()])# 每一个结点的权重=与他相连的节点的个数

        return nodes_weights


    """获得最好的社区"""
    def getBestPartition(self,graph,param=1.):
        """
        类的主方法
        :param graph:图
        :param param:
        :return:
        """
        # 初始化：将输入的图转化为社区，返回社区以及边的权重
        community,ew=self.node2Community(graph)
        # 首先执行Phase1
        community=self.Phase1(community,ew,param)# 每个结点对应的社区
        best_modularity=self.caculateModularity(community,ew,param)

        partition=community.copy()
        new_community,new_ew=self.Phase2(community,ew)

        while True:
            new_community=self.Phase1(new_community,new_ew,param)
            modularity=self.caculateModularity(new_community,new_ew,param)
            print("模块度：",modularity)
            if abs(best_modularity-modularity)<self.MIN_VALUE:
                # 入股ΔQ小于阈值
                break

            best_modularity=modularity #迭代
            partition=self.updatePartition(new_community,partition)
            new_community,new_ew=self.Phase2(new_community,new_ew)

        return partition

    def Phase1(self,community,ew,param):
        """
        将所有的结点放在唯一的社区中（一个节点，一个社区）

        对于每一个结点i,算法计算如下：
            计算Modularity(ΔQ)，当将一个结点i放入相邻社区j中时
            得到最大的Modularity(ΔQ)
        :param community:
        :param ew:
        :param param:
        :return:
        """

        all_edge_weights=sum([weight for start in ew.keys() for end ,weight in ew[start].items()])/2
        self.nodes_weights=self.updateWeights(ew)

        flag=True
        while flag:
            statuses=[]
            for node in community.keys():
                statuses = []
                cid=community[node]
                #TODO:获取邻居节点
                neighbor_nodes=[edge[0] for edge in self.getNeighborNodes(node,ew)]

                max_delta=0.
                max_cid=cid
                dcommunities={}
                for neighbor_node in neighbor_nodes:
                    community_copy=community.copy()
                    if community_copy[neighbor_node] in dcommunities:
                        continue
                    dcommunities[community_copy[neighbor_node]]=1
                    community_copy[node]=community_copy[neighbor_node]

                    #TODO:ΔQ 少了一个方法
                    delta_q=2*self.getNodeWeightInCluster(node,community_copy,ew)-(self.getTOT(node,community_copy,ew)*self.nodes_weights[node]/all_edge_weights)*param
                    if delta_q>max_delta:
                        max_delta=delta_q
                        max_cid=community_copy[neighbor_node]

                community[node]=max_cid
                statuses.append(cid!=max_cid)

            if sum(statuses)==0:
                break

        return community

    def Phase2(self,community,ew):
        """
        网络按照如下的计算方式得到：
            两个超级结点之间的权重是他们相邻两个社区之间的边的权重之和
            超级节点会被链接起来，如果相邻社区至少有一条边相连
        :param community:
        :param ew:
        :return:
        """
        community2node=defaultdict(list)# {社区编号：节点列表}
        new_node2community={}# 结点转化成社区
        new_ew=defaultdict(lambda:defaultdict(float))

        for node,cid in community.items():# 结点，对应的社区名称
            community2node[cid].append(node)
            if cid not in new_node2community:
                new_node2community[cid]=cid

        nodes=list(community.keys())

        # 储存所有点对
        node_pairs=list(permutations(nodes,2))+[(node,node) for node in nodes]

        for edge in node_pairs:
            new_ew[new_node2community[community[edge[0]]]][new_node2community[community[edge[1]]]] += ew[edge[0]][edge[1]]
        return new_node2community,new_ew

    def getTOT(self,node,community_copy,ew):
        """
        计算q
        :param node:
        :param community_copy:
        :param ew:
        :return:
        """
        nodes=[n for n,cid in community_copy.items() if cid ==community_copy[node] and node!=n]
        weight=0.
        for n in nodes:
            weight+=sum(list(ew[n].values()))
        return weight


    def getNeighborNodes(self,node,ew):
        """
        获取邻近结点
        :param node:
        :param ew:
        :return: 返回与结点node相连的所有结点
        """
        if node not in ew:
            return 0
        return ew[node].items()

    def getNodeWeightInCluster(self,node, node2community, ew):
        """
        获取结点在族类的权重
        :param node: 结点
        :param node2community:以结点编号对应社区编号
        :param ew: 边的权重
        :return: 权重
        """
        neighbor_nodes=self.getNeighborNodes(node,ew)# 获取结点node的所有相邻的结点
        node_com=node2community[node]# 结点node对应的社区编号
        weight=0.
        for neighbor_node in neighbor_nodes:
            # neighbor_node的格式：neighbor_node[0]存放的是结点，neighbor_node[1]存放的是权重
            if node_com==node2community[neighbor_node[0]]:
                weight+=neighbor_node[1]
        return weight



    def caculateModularity(self,community,ew,param):
        """
        计算ΔQ
        :param community:社区
        :param ew:边的权重
        :param param:
        :return:ΔQ
        """
        q=0
        #所有边的权重
        all_edge_weights=sum([weight for start in ew.keys() for end ,weight in ew[start].items()])/2

        nodes=defaultdict(list)# 储存社区中结点的列表----->即最终呈现的答案的样式
        for node,cid in community.items():
            nodes[cid].append(node)

        for cid,nodes in nodes.items():
            node_combinations=list(combinations(nodes,2))+[(node,node) for node in nodes]
            cluster_weight=sum([ew[node_pair[0]][node_pair[1]] for node_pair in node_combinations])
            tot=self.countTOT(nodes,ew)
            q+=(cluster_weight/(2*all_edge_weights))-param*((tot/(2*all_edge_weights))**2)
        return q

    def countTOT(self,nodes,ew):
        # getDegreeOfCluster
        """
        计算族类的度数
        :param nodes: 结点
        :param ew: 边权
        :return: TOT
        """
        weight = sum([sum(list(ew[n].values())) for n in nodes])
        return weight


    def updatePartition(self,new_node2community,partition):
        """
        更新社区
        :param new_node2community:新的结点-社区对
        :param partition:
        :return:
        """
        temp_partition=defaultdict(list)
        for node,cid in partition.items():
            temp_partition[cid].append(node)# 社区编号：结点

        for old_cid,new_cid in new_node2community.items():
            for old_com in temp_partition[old_cid]:
                partition[old_com]=new_cid # 更新社区名？

        return partition

# 如何根据txt文件来获取图
def loadGraph(filename):
    """
    将text文件中的图加载进nx中，并且返回nx的图
    filename='email-Eu-core.txt'或者'small_dolphin.txt'
    :param filename:
    :return:
    """
    G = nx.Graph()

    with open(filename) as file:
        for line in file:
            head, tail = [str(x) for x in line.split()]  # 如果节点使用数组表示的可以将str(x)改为int(x)
            # 去掉自环
            #if head==tail:
            #    continue
            G.add_edge(head, tail,weight=1.)
            G.add_edge(tail,head,weight=1.)

    print(G)
    # 将图数据存储进gml,方便之后作图
    #nx.write_gml(G,'email.gml')
    # nx.write_gml(G,'dolphin.gml')
    return G

'''AC计算'''
def acc(y_true, y_pred):
    """
    最后并没有使用这个函数
    """
    y_true = y_true.astype(np.int64)
    assert y_pred.size == y_true.size
    D = max(y_pred.max(), y_true.max()) + 1
    w = np.zeros((D, D), dtype=np.int64)
    for i in range(y_pred.size):
        w[y_pred[i], y_true[i]] += 1
    from scipy.optimize import linear_sum_assignment as linear_assignment  # 添加as语句不用修改代码中的函数名
    ind = linear_assignment(w.max() - w)
    print("debug:ind=",ind)
    return sum([w[i, j] for i, j in ind]) * 1.0 / y_pred.size

def evaluate(ans):
    """
    输入的是字典数
    :param ans: 以社区号为标签的字典
    :return: 分类精度
    """
    #首先存储分类结果为txt:0-1004
    mypre=[]
    for cid,nodelist in ans.items():
        for node in nodelist:
            mypre.append((node,cid))
    # 按照结点的序号升序排序
    mypre.sort(key=lambda x:x[0])
    # 将答案写入txt文件中:顺序为按照节点编号排序0-1004
    file=open('email_pre.txt','w')
    for epre in mypre:
        file.write(str(epre[1]))
        file.write('\n')
    file.close()
    # 从txt文件中读取分类结果，并用np存储------------预测结果
    file=open('email_pre.txt','r')
    lines=file.readlines()
    ypre=[]
    for line in lines:
        ypre.append(int(line))
    file.close()
    # 从txt中读取标签-----------------实际结果
    file=open('E:/homework/louvain-python-master/email-Eu-core-department-labels.txt','r')
    lines=file.readlines()
    ylabel=[]
    for line in lines:
        trainingSet = line.split(' ')# 按照空格分割
        ylabel.append(int(trainingSet[1]))
    # 数据格式转化
    ylabel=np.array(ylabel)
    ypre=np.array(ypre)

    # 准确率
    from sklearn.metrics import accuracy_score
    print("准确率：",accuracy_score(y_true=ylabel,y_pred=ypre))
    # 随机分配评分
    from sklearn.metrics.cluster import adjusted_rand_score
    print("随机分配准确率：",adjusted_rand_score(ylabel,ypre))
    #ARI
    from sklearn import metrics
    print("ARI=",metrics.adjusted_rand_score(ylabel,ypre) )
    # 互信息
    print("互信息",metrics.adjusted_mutual_info_score(ylabel,ypre))
    # FMI
    print("FMI",metrics.fowlkes_mallows_score(ylabel,ypre) )

'''实验'''
def test_Dolphin():
    """
    实验一：无标签小样本
    :return:
    """
    sample_graph=loadGraph('E:/homework/louvain-python-master/small_dolphin.txt')
    print("简单的图",sample_graph)
    nx.draw(sample_graph, with_labels=True)
    plt.show()
    edges, weights = zip(*nx.get_edge_attributes(sample_graph, 'weight').items())
    print("weights=",weights)
    pos = nx.spring_layout(sample_graph)
    nx.draw(sample_graph, pos, node_color='b', edgelist=edges, edge_color=weights, width=10.0, edge_cmap=plt.cm.Blues)
    # plt.savefig('edges.png')
    plt.show()

    louvain = Louvain()
    partition = louvain.getBestPartition(sample_graph)

    p = defaultdict(list)
    for node, com_id in partition.items():
        p[com_id].append(node)
    print("最终分类的结果")
    for com, nodes in p.items():
        print(com, nodes)  # 打印出社区、结点

    print(type(p))

    # 替代方案
    print("p=", p)
    values = [(search(p, node)) % 100 for node in sample_graph.nodes()]
    print("values=", values)
    nx.draw_spring(sample_graph, cmap=plt.get_cmap('jet'), node_color=values, node_size=30, with_labels=False)
    plt.show()
    pass

def test_Email():
    """
    实验二：有标签大样本
    :return: 返回聚类的精确度
    """
    start_time=time.time()
    sample_graph = loadGraph('E:/homework/louvain-python-master/email-Eu-core.txt')


    nx.draw(sample_graph, with_labels=True)
    plt.show()
    louvain = Louvain()
    partition = louvain.getBestPartition(sample_graph)

    p = defaultdict(list)
    for node, com_id in partition.items():
        p[com_id].append(int(node))
    end_time=time.time()

    print("消耗时间：",end_time-start_time)

    print("最终分类的结果")
    for com, nodes in p.items():
        print(com, nodes)  # 打印出社区、结点

    evaluate(p)

    '''acc计算'''
    # 首先读取每个结点的标签
    file=open('E:/homework/louvain-python-master/email-Eu-core-department-labels.txt')
    count=0.
    right=0
    target=defaultdict(list)# {社区编号：节点列表}# 储存正确的词典
    for line in file:
        count+=1
        node_label=line.split()
        node=str(node_label[0])
        label=node_label[1]
        target[label].append(int(node))
    print("目标分类效果：",target)



    # 替代方案
    print("p=",p)
    values = [search(p,node) for node in sample_graph.nodes()]
    print("values=",values)
    nx.draw_spring(sample_graph, cmap=plt.get_cmap('jet'), node_color=values, node_size=30, with_labels=False)
    plt.show()

    pass

def search(p,node):
    """
    搜索结点的社区编号
    :param p:
    :param node:
    :return:
    """
    ans=0
    for com,nodes in p.items():
        if int(node) in nodes or node in nodes:
            ans=com
            break
    return ans



if __name__=='__main__':

    #test_Email()
    test_Dolphin()







