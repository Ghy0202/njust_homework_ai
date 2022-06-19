####################################
#       数独问题的简单遍历解法：
#       简单遍历求解
#       其中solve_0为最初写的单解、无解的版本
#       solve_00为之后修改的多解版本
####################################
import time


def isSafe(data, row, col, num):
    """

    :param data:
    :param row:
    :param col:
    :param num:
    :return:
    """
    # 检查列重复
    for x in range(9):
        if data[row][x]==num:
            return False
    # 检查行重复
    for x in range(9):
        if data[x][col]==num:
            return False
    # 检查九宫格重复
    sRow = row - row % 3
    sCol = col - col % 3
    for i in range(3):
        for j in range(3):
            if data[i + sRow][j + sCol] == num:
                return False
    return True


'''单解的解法'''
def solve_0(data,row,col):
    """
    输入棋盘，以及行列信息
    :param data: 棋盘
    :param row: 行
    :param col: 列
    :return: 是否有解
    """
    if(row==8 and col==9 ):
        # 到达边界，说明求解成功
        return True

    if col==9:
        row+=1
        col=0

    if data[row][col]>0:
        # 已经有数字
        return solve_0(data,row,col+1)
    for num in range(1,10,1):
        # 1-9遍历
        if isSafe(data,row,col,num):
            data[row][col]=num

            if solve_0(data,row,col+1):
                return True
        # 还原这个位置的的值
        data[row][col]=0

    return False
'''多解的解法'''
count=0
def solve_00(data,row,col):
    """
    输入棋盘，以及行列信息
    :param data: 棋盘
    :param row: 行
    :param col: 列
    :return: 是否有解
    """
    global count
    if(row==8 and col==9 ):
        # 到达边界，说明求解成功
        count+=1
        print("求解结果")
        printData(data)
        return 0

    if col==9:
        row+=1
        col=0

    if data[row][col]>0:
        # 已经有数字
        return solve_00(data,row,col+1)
    for num in range(1,10,1):
        # 1-9遍历
        if isSafe(data,row,col,num):
            data[row][col]=num

            if solve_00(data,row,col+1):
                return 0
        # 还原这个位置的的值
        data[row][col]=0

    return 0

def printData(data):
    """
    打印数独求解结果
    :param data: 数独
    :return:打印的结果
    """
    for i in range(9):
        for j in range(9):
            print(data[i][j],end=" ")
        print()

'''单解比较时间复杂度'''
def test_1():
    data = [[0,0,5,3,0,0,0,0,0],
        [8,0,0,0,0,0,0,2,0],
        [0,7,0,0,1,0,5,0,0],
        [4,0,0,0,0,5,3,0,0],
        [0,1,0,0,7,0,0,0,6],
        [0,0,3,2,0,0,0,8,0],
        [0,6,0,5,0,0,0,0,9],
        [0,0,4,0,0,0,0,3,0],
        [0,0,0,0,0,9,7,0,0]]
    start_time=time.time()
    if (solve_0(data, 0, 0)):
        printData(data)
        end_time=time.time()
        print("耗时：",end_time-start_time)
    else:
        print("无解 ")
        end_time = time.time()
        print("耗时：", end_time - start_time)

'''无解比较时间复杂度'''
def test_2():
    data = [[0,0,0,5,8,7,0,0,0],
        [0,6,5,4,0,3,9,7,0],
        [0,4,0,0,0,0,0,5,0],
        [5,9,0,0,6,0,0,4,2],
        [4,8,0,3,0,2,0,0,9],
        [2,8,0,0,4,0,0,6,5],
        [0,2,0,0,0,0,0,9,0],
        [0,5,8,2,0,6,4,1,0],
        [0,0,0,1,9,5,0,0,0]]
    start_time = time.time()
    if (solve_0(data, 0, 0)):
        printData(data)
        end_time = time.time()
        print("耗时：", end_time - start_time)
    else:
        print("无解 ")
        end_time = time.time()
        print("耗时：", end_time - start_time)

'''多解的情况'''
def test_3():
    """
    多解的测试
    :return:
    """
    data = [[0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 6, 5, 4, 0, 3, 9, 7, 0],
        [0, 4, 0, 0, 0, 0, 0, 5, 0],
        [5, 9, 0, 0, 6, 0, 0, 4, 2],
        [4, 0, 0, 3, 0, 2, 0, 0, 9],
        [2, 8, 0, 0, 4, 0, 0, 6, 5],
        [0, 2, 0, 0, 0, 0, 0, 9, 0],
        [0, 5, 8, 2, 0, 6, 4, 1, 0],
        [0, 0, 0, 1, 9, 5, 0, 0, 0]]
    start_time = time.time()
    if (solve_00(data, 0, 0)):
        printData(data)
        end_time = time.time()
        print("耗时：", end_time - start_time)
        print("解的个数",count)
    else:
        print("~~~~ ")
        end_time = time.time()
        print("耗时：", end_time - start_time)
        print("解的个数", count)

if __name__=='__main__':
    print("单解测试")
    test_1()
    print("无解测试")
    test_2()
    print("多解测试")
    test_3()









