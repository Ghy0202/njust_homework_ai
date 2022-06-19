###############################################
# 回溯法：
#   (1)解决单解的问题
#   (2)解决多解的问题
#   (3)解决判断是否有解的问题
#
################################################
import copy
import numpy as np

# 检验算法效率
import threading
import time



'''定义空缺位置的结构体'''
class VAC(object):
    def __init__(self,row,col,area,rangelist):
        """
        初始化
        :param row: 行
        :param col: 列
        :param area: 3*3九宫格
        :param rangelist: 取值范围
        """
        self.row=row
        self.col=col
        self.area=area
        self.rangelist=rangelist

'''根据行和列判断属于哪一个九宫格'''
def countIndex(i,j):
    """
    返回九宫格的编号
    :param i:
    :param j:
    :return:
    """
    if i<3:
        if j<3:
            return 0
        elif j<6:
            return 1
        else:
            return 2
    elif i<6:
        if j<3:
            return 3
        elif j<6:
            return 4
        else:
            return 5
    else:
        if j<3:
            return 6
        elif j<6:
            return 7
        else:
            return 8

'''初始化行、列以及九宫格'''
def _init(data):
    """
    根据输入的数独初始化
    :param data:
    :return:返回数独题的行、列、九宫格的状态
    """
    row=[[],[],[],[],[],[],[],[],[]]
    column=[[],[],[],[],[],[],[],[],[]]
    area=[[],[],[],[],[],[],[],[],[]]
    for i in range(9):
        for j in range(9):
            if data[i,j]!=0:
                row[i].append(data[i,j])
                column[j].append(data[i,j])
                index=countIndex(i,j)
                area[index].append(data[i,j])
    return row,column,area

'''空缺位置的初始化'''
def _initVac(data,row,col,area):
    """
    初始化空缺位置的信息：包含所在位置以及可以填的数字的范围
    :param data:
    :param row:
    :param col:
    :param area:
    :return:
    """
    vac=list()
    for i in range(9):
        for j in range(9):
            if data[i,j]==0:
                index=countIndex(i,j)
                values=[1,2,3,4,5,6,7,8,9]
                for r in row[i]:
                    if r in values:
                        values.remove(r)
                for c in col[j]:
                    if c in values:
                        values.remove(c)
                for a in area[index]:
                    if a in values:
                        values.remove(a)
                vac0=VAC(i,j,index,values)# 初始化空缺位置
                vac.append(vac0)
    vac.sort(key=lambda l:len(l.rangelist))
    return vac



'''检查填写是否合法'''
def isOk(row,column,area,vac):
    """
    检查空缺位置填入的数字是否合法
    :param row:行
    :param column:列
    :param area:九宫格
    :param vac:空缺位置
    :return:是否合法的填充
    """
    for r in row:
        if len(set(r))!=len(r):
            return False
    for c in column:
        if len(set(c))!=len(c):
            return False
    for a in area:
        if len(set(a))!=len(a):
            return False
    for v in vac:
        if len(v.rangelist)==0:
            return False
    return True
'''回溯法求解'''
c_count=0
def solve_1(data):
    """
    回溯法主函数
    :param data:
    :param count:
    :return:
    """
    global c_count
    row,column,area=_init(data)# 初始化行、列以及九宫格的状态
    vac=_initVac(data,row,column,area)# 初始化空位置的状态

    # 如果不通过
    if isOk(row,column,area,vac)==False:
        return 0

    # 如果空缺位置已经全部填满
    if vac==[]:
        # TODO:之后记得注释回来
        print(20*"*","求解结果",20*"*")
        print(data)
        c_count+=1
        return 0
    # 否则继续搜索
    else:
        cur_vac=vac[0] #当前的空缺位置
        if len(cur_vac.rangelist)==1:
            data[cur_vac.row,cur_vac.col]=cur_vac.rangelist[0]
            #print("debug2", data,"在",cur_vac.row,cur_vac.col,"插入",cur_vac.rangelist[0])
            return solve_1(data)
        elif len(cur_vac.rangelist)>1:
            data1=copy.deepcopy(data)
            for i in cur_vac.rangelist:
                data[cur_vac.row,cur_vac.col]=i
                #print("debug1",data,"在",cur_vac.row,cur_vac.col,"插入",i)
                if solve_1(data)==0:
                    data=data1 # 回溯
            return 0
#########################################################################################################################限时运行函数
'''限时计算输出多解的情况'''
class Limit(threading.Thread):
    def __init__(self,target,args=()):
        """
        这里利用thread来限时
        :param target:目标函数
        :param args:
        """
        super(Limit,self).__init__()
        self.func=target
        self.args=args

    def run(self):
        """
        接受返回值
        :return:
        """
        self.result=self.func(*self.args)

    def get_extra(self):
        """
        线程不结束，这里需要显示一下延迟的问题
        :return:
        """
        try:
            return self.result
        except Exception:
            return None

# 限时的使用
def limit_time(timeout,granularity):
    """
    对于上面封装的Limit的调用
    :param timeout: 最大允许的运行时长
    :param granularity: 轮询间隔（轮询时间越短越精确，同时CPU负荷越大）
    :return:
    """
    def functions(func):
        def run(*args):
            thre_func=Limit(target=func,args=args)
            thre_func.setDaemon(True)
            thre_func.start()
            sleep_num=int(timeout//granularity)
            for i in range(0,sleep_num):
                infor=thre_func.get_extra()
                if infor:
                    return infor
                else:
                    time.sleep(granularity)
            return None
        return run
    return functions




######################################################################################################################测试点
'''单解测试1'''
def test_1():
    """
    测试单解的情况
    :return:
    """
    # 输入需要测试的数独
    mp=np.array([
        [0,0,5,3,0,0,0,0,0],
        [8,0,0,0,0,0,0,2,0],
        [0,7,0,0,1,0,5,0,0],
        [4,0,0,0,0,5,3,0,0],
        [0,1,0,0,7,0,0,0,6],
        [0,0,3,2,0,0,0,8,0],
        [0,6,0,5,0,0,0,0,9],
        [0,0,4,0,0,0,0,3,0],
        [0,0,0,0,0,9,7,0,0]
    ])
    print(20 * "*", "原始数独", 20 * "*")
    print(mp)
    row,column,area=_init(mp)
    vac=_initVac(mp,row,column,area)
    # 求解
    start_time=time.time()
    if isOk(row,column,area,vac):
        solve_1(mp)
    else:
        print("~~~~~无解~~~~~~")
    end_time = time.time()
    print("耗时：", end_time - start_time, "s")
    if(c_count==0):
        print("~~~~~无解~~~~~~")
    else:
        print("~~~~~~~~~~~~~~~~~~有",c_count,"个解~~~~~~~~~~~~~~~~~~")

'''单解测试2'''
def test_2():
    """
    测试单解的情况
    :return:
    """
    # 输入需要测试的数独

    mp=np.array([
        [0,0,0,5,8,7,0,0,0],
        [0,6,5,4,0,3,9,7,0],
        [0,4,0,0,0,0,0,5,0],
        [5,9,0,0,6,0,0,4,2],
        [4,0,0,3,0,2,0,0,9],
        [2,8,0,0,4,0,0,6,5],
        [0,2,0,0,0,0,0,9,0],
        [0,5,8,2,0,6,4,1,0],
        [0,0,0,1,9,5,0,0,0]
    ])
    print(20 * "*", "原始数独", 20 * "*")
    print(mp)
    row,column,area=_init(mp)
    vac=_initVac(mp,row,column,area)
    # 求解
    start_time=time.time()
    if isOk(row,column,area,vac):
        solve_1(mp)
    else:
        print("~~~~~无解~~~~~~")
    end_time = time.time()
    print("耗时：", end_time - start_time, "s")
    if(c_count==0):
        print("~~~~~无解~~~~~~")
    else:
        print("~~~~~~~~~~~~~~~~~~有",c_count,"个解~~~~~~~~~~~~~~~~~~")

'''无解测试1'''
def test_3():
    """
    测试单解的情况
    :return:
    """
    # 输入需要测试的数独

    mp=np.array([
        [0,0,0,5,8,7,0,0,0],
        [0,6,5,4,0,3,9,7,0],
        [0,4,0,0,0,0,0,5,0],
        [5,9,0,0,6,0,0,4,2],
        [4,8,0,3,0,2,0,0,9],
        [2,8,0,0,4,0,0,6,5],
        [0,2,0,0,0,0,0,9,0],
        [0,5,8,2,0,6,4,1,0],
        [0,0,0,1,9,5,0,0,0]
    ])
    print(20 * "*", "原始数独", 20 * "*")
    print(mp)
    row,column,area=_init(mp)
    vac=_initVac(mp,row,column,area)
    # 求解
    start_time=time.time()
    if isOk(row,column,area,vac):
        solve_1(mp)
    else:
        print("~~~~~无解~~~~~~")
    end_time = time.time()
    print("耗时：", end_time - start_time, "s")
    if(c_count==0):
        print("~~~~~无解~~~~~~")
    else:
        print("~~~~~~~~~~~~~~~~~~有",c_count,"个解~~~~~~~~~~~~~~~~~~")
'''无解测试2'''
def test_4():
    """
    测试单解的情况
    :return:
    """
    # 输入需要测试的数独

    mp=np.array([
        [0, 0, 0, 5, 8, 7, 0, 0, 0],
        [0, 6, 5, 4, 0, 3, 9, 7, 0],
        [0, 4, 0, 0, 0, 0, 0, 5, 0],
        [5, 9, 0, 0, 6, 0, 0, 4, 2],
        [4, 0, 0, 3, 0, 2, 0, 0, 9],
        [2, 8, 0, 0, 4, 0, 0, 6, 5],
        [6, 2, 0, 0, 0, 0, 0, 9, 0],
        [0, 5, 8, 2, 0, 6, 4, 1, 0],
        [0, 0, 0, 1, 9, 5, 0, 0, 0]
    ])
    print(20 * "*", "原始数独", 20 * "*")
    print(mp)
    row,column,area=_init(mp)
    vac=_initVac(mp,row,column,area)
    # 求解
    start_time=time.time()
    if isOk(row,column,area,vac):
        solve_1(mp)
    else:
        print("~~~~~无解~~~~~~")
    end_time = time.time()
    print("耗时：", end_time - start_time, "s")
    if(c_count==0):
        print("~~~~~无解~~~~~~")
    else:
        print("~~~~~~~~~~~~~~~~~~有",c_count,"个解~~~~~~~~~~~~~~~~~~")

'''多解测试1'''
def test_5():
    """
    测试单解的情况
    :return:
    """
    # 输入需要测试的数独

    mp=np.array([
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 6, 5, 4, 0, 3, 9, 7, 0],
        [0, 4, 0, 0, 0, 0, 0, 5, 0],
        [5, 9, 0, 0, 6, 0, 0, 4, 2],
        [4, 0, 0, 3, 0, 2, 0, 0, 9],
        [2, 8, 0, 0, 4, 0, 0, 6, 5],
        [0, 2, 0, 0, 0, 0, 0, 9, 0],
        [0, 5, 8, 2, 0, 6, 4, 1, 0],
        [0, 0, 0, 1, 9, 5, 0, 0, 0]
    ])
    print(20 * "*", "原始数独", 20 * "*")
    print(mp)
    row,column,area=_init(mp)
    vac=_initVac(mp,row,column,area)
    # 求解
    start_time=time.time()
    if isOk(row,column,area,vac):
        solve_1(mp)
    else:
        print("~~~~~无解~~~~~~")
    end_time = time.time()
    print("耗时：", end_time - start_time, "s")
    if(c_count==0):
        print("~~~~~无解~~~~~~")
    else:
        print("~~~~~~~~~~~~~~~~~~有",c_count,"个解~~~~~~~~~~~~~~~~~~")

'''多解测试2'''
def test_7():
    """
    测试单解的情况
    :return:
    """
    # 输入需要测试的数独

    mp=np.array([
        [0, 0, 5, 3, 0, 0, 0, 0, 0],
        [8, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 7, 0, 0, 1, 0, 0, 0, 0],
        [4, 0, 0, 0, 0, 5, 0, 0, 0],
        [0, 1, 0, 0, 7, 0, 0, 0, 6],
        [0, 0, 3, 2, 0, 0, 0, 8, 0],
        [0, 6, 0, 5, 0, 0, 0, 0, 9],
        [0, 0, 4, 0, 0, 0, 0, 3, 0],
        [0, 0, 0, 0, 0, 9, 7, 0, 0]
    ])
    print(20 * "*", "原始数独", 20 * "*")
    print(mp)
    row,column,area=_init(mp)
    vac=_initVac(mp,row,column,area)
    # 求解
    start_time=time.time()
    if isOk(row,column,area,vac):
        solve_1(mp)
    else:
        print("~~~~~无解~~~~~~")
    end_time = time.time()
    print("耗时：", end_time - start_time, "s")
    if(c_count==0):
        print("~~~~~无解~~~~~~")
    else:
        print("~~~~~~~~~~~~~~~~~~有",c_count,"个解~~~~~~~~~~~~~~~~~~")
'''多解测试3'''
@limit_time(2,0.02)
def test_6():
    """
    测试单解的情况
    :return:
    """
    # 输入需要测试的数独

    mp=np.array([
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0]
    ])
    print(20 * "*", "原始数独", 20 * "*")
    print(mp)
    row,column,area=_init(mp)
    vac=_initVac(mp,row,column,area)
    # 求解
    start_time=time.time()
    if isOk(row,column,area,vac):
        solve_1(mp)
    else:
        print("~~~~~无解~~~~~~")
    end_time = time.time()
    print("耗时：", end_time - start_time, "s")
    if(c_count==0):
        print("~~~~~无解~~~~~~")
    else:
        print("~~~~~~~~~~~~~~~~~~有",c_count,"个解~~~~~~~~~~~~~~~~~~")



if __name__=='__main__':
    # 单解情况
    #test_1()
    #test_2()
    # 多解情况
    #test_5()

    #test_7()
    # 无解情况
    #test_3()
    #test_4()

    # 这里测试对于全0数独，1秒钟可以跑多少组解
    # 对于多解问题输出的时候，只需要输出c_count就好，中间过程答案不输出（因为输出也算时间）
    test_6()
    print("最终2s的求解结果",c_count)
