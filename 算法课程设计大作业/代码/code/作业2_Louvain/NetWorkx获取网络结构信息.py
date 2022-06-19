
from communities.algorithms import louvain_method
import matplotlib.pyplot as plt
import networkx as nx

from communities.visualization import louvain_animation
from communities.visualization import draw_communities
path1="E:/homework/louvain-python-master/MyLouvain/dolphin.gml"
path2="E:/homework/louvain-python-master/MyLouvain/email.gml"

Graph1=nx.read_gml(path1)
Graph2=nx.read_gml(path2)

def test(G):
    """
    测试：输入图信息，输出划分结果
    :param G: 图
    :return:
    """
    # 输出节点信息
    #print("结点信息：",G.nodes(data=True))
    # 输出边信息
    #print("边信息：",G.edges())
    # 计算网络的传递性
    print("网络传递性：",nx.transitivity(G))
    # 结点个数
    print("结点个数：",G.number_of_nodes())
    # 边数
    print("边的个数：",G.number_of_edges())

    # 结点的邻居个数
    #print("结点邻居个数：",G.neighbors())

    #数据格式转换
    #
    A = nx.to_numpy_matrix(G)
    #A = nx.adjacency_matrix(G).todense()
    print(type(A.A))
def loadtxtG(path):
    G = nx.Graph()


    edge_list = []
    node_set = set()  # 集合的特性就是元素不会重复，互异的
    with open(path, 'r') as f:
        for line in f:
            cols = line.strip().split(' ')
            y1 = int(cols[0])
            y2 = int(cols[1])
            node_set.add(y1)
            node_set.add(y2)
            edge = (y1, y2)  # 元组代表一条边
            edge_list.append(edge)

    G.add_nodes_from(range(1, len(node_set) + 1))  # 节点序号从1开始编号
    G.add_edges_from(edge_list)
    #A = nx.to_numpy_matrix(G)
    A = nx.adjacency_matrix(G).todense()
    print(type(A.A))
    return A.A

def countIndex(num,L):
    ans=0
    for i in range(len(L)):
        if num in L[i]:
            ans=i
            break
    return ans

if __name__=="__main__":


    test(Graph1)
    print("*"*90)
    test(Graph2)

    # 失败了
    # test(Graph1)
    path1="E:/homework/louvain-python-master/small_dolphin.txt"
    #path2="E:/homework/louvain-python-master/email-Eu-core.txt"
    A=loadtxtG(path1)
    com ,frame= louvain_method(A)
    for i in range(len(com)):
        print(com[i])
    #print(frame)
    print(type(A),type(com))
    # 替代方案
    values=[  countIndex(int(node),com) for node in Graph1.nodes()]
    print(values)
    nx.draw_spring(Graph1, cmap=plt.get_cmap('jet'), node_color=values, node_size=30, with_labels=False)
    plt.show()


    # 绘制动图
    #louvain_animation(A, frame,filename="dolphin.gif")

    # 绘制静态图
    #draw_communities(adj_matrix=A,communities=com)





