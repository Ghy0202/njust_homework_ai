#include<stdio.h>
#include<string>
#include<string.h>
#include<math.h>
#include<windows.h>
#include<time.h>
#include<vector>
#include<iostream>
#include<algorithm>

#include "bmpFile.h"
#include "RmwALG_HoughLine.h"
#include "RmwALG_HoughCircle.h"
#include"RmwDraw1Data2Img.h"
#include "RmwALG_ContourTrace.h"
#include "RmwALG_ContourFill.h"

using namespace std;

//全局变量
#define  MAX_POINT_NUM   (512*512)
#define MAX_CONOUR_LENGTH  (512*512)
#define PI 3.1415926
BYTE gChainCode[MAX_CONOUR_LENGTH];

static int gN;
static int gX[MAX_POINT_NUM];
static int gY[MAX_POINT_NUM];
/*最开始：求解图像空间*/
void Test(int t) {
	int width = 256;
	int height = 256;
	int x[] = { 128,0,255,255 };
	int y[] = { 128,255,0,255 };
	int cosV, sinV, maxThro;
	int  thro;
	int theta;
	int i=t;
	int maxCount ,bstTheta , bstThro ;
	int* pCenter, * pCur;
	maxThro = int(sqrt(1.0 * width + width + height * height + 0.5) + 1);
	int* pCount = new int[(maxThro *2* 180)];
	memset(pCount, 0, sizeof(int) * (maxThro * 2 * 180));
	//步长1
	for (theta = 0; theta < 180; theta++) {
		cosV = int(cos(theta * PI / 180) * 8912);
		sinV = int(sin(theta * PI / 180) * 8912);
		pCenter = pCount + int((maxThro * 2) * theta) + maxThro;
		thro = ((x[i] * cosV + y[i] * sinV) >> 13);
		*(pCenter + thro) += 1;
		//std::cout << "!" << *(pCenter + thro) << endl;
	}
	maxCount = bstTheta = bstThro = 0;
	for (theta = 0, pCur = pCount; theta < 180; theta++)
	{
		for (thro = 0; thro < maxThro * 2; thro++, pCur++)
		{
			if (*pCur > maxCount)
			{
				maxCount = *pCur;
				bstTheta = int(theta);
				bstThro = thro;
			}
		}
	}
	bstThro -= maxThro; //去掉偏移maxThro
	printf("theta=%d,thro=%d\n", bstTheta, bstThro);
	unsigned char* pCountImg = new unsigned char[maxThro * 2 * 180];
	for (int k = 0; k < maxThro * 2 * 180; k++) {
		if (pCount[k] != 0) {
			pCountImg[k] = 255;
		}
		else {
			pCountImg[k] = 0;
		}
			
	}
	char tname[90];
	sprintf_s(tname, "./res_data/%d.bmp", i);
	Rmw_Write8BitImg2BmpFile(pCountImg, maxThro, 2 * 180,tname);

}
/*任务一：求边，显示计数器*/
struct Ans{
	int theta;
	int thro;
	int count;
};
vector<Ans>ans;
bool cmp(Ans a, Ans b) {
	return a.count > b.count;
}
int HoughLine(int width, int height,
	int* x, int* y, int N,
	double* A, double* B, double* C,int* pCount
)
{
	int maxCount, bstTheta, bstThro;
	int* pCenter, * pCur;
	int maxThro, cosV, sinV;
	int theta, thro, i;
	// step.1-------------申请计数器空间---------------------//
	maxThro = (int)sqrt(1.0 * width * width + height * height + 0.5) + 1;
	//pCount = new int[(maxThro*2)*180]; //(-maxThro,maxThro)
	if (!pCount) return 0; //建议pCount在该函数外申请
	// step.2-------------霍夫变换---------------------------//
	memset(pCount, 0, sizeof(int) * (maxThro * 2) * 180);
	for (theta = 0; theta < 180; theta++) //步长为1度
	{
		cosV = (int)(cos(theta * 3.1415926 / 180) * 8192); //放大8192倍
		sinV = (int)(sin(theta * 3.1415926 / 180) * 8192);
		pCenter = pCount + (maxThro * 2) * theta + maxThro; //加上偏移maxThro
		for (i = 0; i < N; i++)
		{
			thro = ((x[i] * cosV + y[i] * sinV) >> 13); //缩小8192倍,thro的步长为1
			*(pCenter + thro) += 1;
		}
	}
	// step.3-------------最大值搜索-------------------------//
	maxCount = bstTheta = bstThro = 0;
	for (theta = 0, pCur = pCount; theta < 180; theta++)
	{
		for (thro = 0; thro < maxThro * 2; thro++, pCur++)
		{
			if (*pCur > 0) {
				Ans temp;
				temp.theta = theta;
				temp.thro = thro;
				temp.count = *pCur;
				ans.push_back(temp);
			}
			if (*pCur > maxCount)
			{
				maxCount = *pCur;
				bstTheta = theta;
				bstThro = thro;
			}
		}
	}
	sort(ans.begin(), ans.end(), cmp);
	for (int k = 0; k < 4; k++) {
		std::cout << " theta:" << ans[k].theta << ",thro: " << ans[k].thro << endl;
	}
	bstThro -= maxThro; //去掉偏移maxThro
	// step.4-------------求直线Ax+By+C的值-----------------//
	//x*cos(bstTheta)+y*sin(bstTheta)=bstThro => Ax+By+C=0
	*A = cos(bstTheta * 3.1415926 / 180);
	*B = sin(bstTheta * 3.1415926 / 180);
	*C = -bstThro;
	//printf("%lfX+%lfY+%lf\n", *A, *B, *C);
	//printf("theta=%d,thro=%d\n", bstTheta, bstThro);
	unsigned char* pCountImg = new unsigned char[maxThro * 2 * 180];
	for (int k = 0; k < maxThro * 2 * 180; k++) {
		if (pCount[k] != 0) {
			pCountImg[k] = 255;
		}
		else {
			pCountImg[k] = 0;
		}

	}
	char tname[90];

	Rmw_Write8BitImg2BmpFile(pCountImg, maxThro, 2 * 180, "ccb.bmp");
	// step.5-------------返回经过的点数---------------------//
	//delete pCount; //释放自己申请的内存,该函数内动态申请和释放会产生内存碎片
	return maxCount;
}

void Exam1(string filename,string targetname) {
	unsigned char* pGryImg,*pResImg,* gDbgImg;
	int width, height;
	int* pCount = new int[MAX_POINT_NUM];
	double A, B, C;
	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img(filename.c_str(), &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	//TODO:这边是需要完全自己写的
	pResImg = new unsigned char[width * height];
	/*---------搜集边缘点---------*/
	int x, y;
	gN = 0;
	for (y = 0; y < height; y++)
	{
		//从左到右
		for (x = 0; x < width; x++) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
	}
	int h=RmwHoughLine(width, height, gX, gY, gN, &A, &B, &C,pCount);
	RmwDrawABCLine(pResImg, width, height, A, B, C, 254);
	gN = 0;
	for (y = 0; y < height; y++) {
		//从右到左
		for (x = width - 1; x >= 0; x--) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
	}
	h = RmwHoughLine(width, height, gX, gY, gN, &A, &B, &C, pCount);
	RmwDrawABCLine(pResImg, width, height, A, B, C, 254);
	gN = 0;
	for (x = 0; x < width; x++)
	{
		//从上到下
		for (y = 0; y < height; y++) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
	}
	h = RmwHoughLine(width, height, gX, gY, gN, &A, &B, &C, pCount);
	RmwDrawABCLine(pResImg, width, height, A, B, C, 254);
	gN = 0;
	for (x = 0; x < width; x++) {
		//从下到上
		for (y = height - 1; y >= 0; y--) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
	}
	h = RmwHoughLine(width, height, gX, gY, gN, &A, &B, &C, pCount);
	RmwDrawABCLine(pResImg, width, height, A, B, C, 254);
	/*单独一个一个地检测*/
	
	printf("%d ", gN);
	//Rmw_Write8BitImg2BmpFile(pResImg, width, height, "debug.bmp");
	//Rmw_Write8BitImg2BmpFileMark(pResImg, width, height, (targetname + "_one2one.bmp").c_str());

	/*直接测一个框*/

	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
void Exam1_1(string filename,string targetname) {
	//矩形检测
	unsigned char* pGryImg, * gDbgImg,*pResImg;
	int width, height;
	int* pCount = new int[MAX_POINT_NUM];
	double A, B, C;
	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img(filename.c_str(), &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	pResImg = new unsigned char[width * height];
	//矩形检测：预测的是图像中间的点
	//检测四个峰值
	int x, y;
	gN = 0;
	for (y = 0; y < height; y++)
	{
		//从左到右
		for (x = 0; x < width; x++) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
		//从右到左
		for (x = width - 1; x >= 0; x--) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
	}
	for (x = 0; x < width; x++)
	{
		//从上到下
		for (y = 0; y < height; y++) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
		//从下到上
		for (y = height - 1; y >= 0; y--) {
			if (*(pGryImg + y * width + x) == 0) {
				gX[gN] = x;
				gY[gN] = y;
				gN++;
				pResImg[y * width + x] = 0;
				break;
			}
		}
	}
	int maxThro = (int)sqrt(1.0 * width * width + height * height + 0.5) + 1;
	int h = HoughLine(width, height, gX, gY, gN, &A, &B, &C, pCount);
	for (int i = 0; i < 4; i++) {
		A = cos(ans[i].theta * 3.1415926 / 180);
		B = sin(ans[i].theta * 3.1415926 / 180);
		C = -(ans[i].thro-maxThro);
		cout << A << " " << B << " " << C << endl;
		RmwDrawABCLine(pResImg, width, height, A, B, C, 254);
	}
	Rmw_Write8BitImg2BmpFileMark(pResImg, width, height, (targetname + "_four.bmp").c_str());
}
/*任务二：分治法求外圆*/
void Exam2(string filename,string targetfile) {
	unsigned char* pGryImg,* gDbgImg;
	int width, height;
	int i=0;
	int R = 0;
	int x0=-1, y0=-1;
	int maxCountx = 0;
	int maxCounty = 0;
	int maxCountr = 0;
	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img(filename.c_str(), &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	else {
		printf("success open file\n");
		gDbgImg = new unsigned char[width * height];
	}
	// step.2-----------分治法求外轮廓圆形---------------------//
	int cnt = 0;
	int* pCountX=new int[width + 1];
	int* pCountY = new int[height + 1];
	int* pCountR = new int[int(sqrt(width * width + height * height * 1.0)) + 1];
	memset(pCountX, 0, sizeof(int)*(width+1));
	memset(pCountY, 0, sizeof(int)*(height+1));
	memset(pCountR, 0, sizeof(int)*(int(sqrt(width * width + height * height * 1.0)) + 1));
	vector<int>posx;
	vector<int>posy;
	//对横坐标，边缘二分检测
	int xl, xr,xc;
	for (int y = 0; y < height; y++) {
		//从左到右，最左边的边缘点
		for (xl = 0; xl < width; xl++) {
			if (!pGryImg[y * height + xl]) {
				posx.push_back(xl);
				posy.push_back(y);
				cnt++;
				break;
			}
		}
		//从右到左，最右边的边缘点
		for (xr = width - 1; xr >= 0; xr--) {
			if (!pGryImg[y * height + xr]) {
				posx.push_back(xr);
				posy.push_back(y);
				cnt++;
				break;
			}
		}
		//中点计数
		if (xr - xl > 0)
		{
			xc = (xr + xl + 1) >> 1;
			pCountX[xc]++;
			if (pCountX[xc] > maxCountx) {
				maxCountx = pCountX[xc];
				x0 = xc;
			}
		}

	}
	//对纵坐标，边缘二分检测
	int yl, yr,yc;
	for (int x = 0; x < width; x++) {
		//从上到下，最上边的边缘点
		for (yl = 0; yl < height; yl++) {
			if (!pGryImg[yl * height + x]) {
				posx.push_back(x);
				posy.push_back(yl);
				cnt++;
				break;
			}
		}
		//从下到上，最下边的边缘点
		for (yr = height-1; yr >=0; yr--) {
			if (!pGryImg[yr * height + x]) {
				posx.push_back(x);
				posy.push_back(yr);
				cnt++;
				break;
			}
		}
		//中点计数
		if (yr-yl > 0)
		{
			yc = (yr + yl + 1) >> 1;
			pCountY[yc]++;
			if (pCountY[yc] > maxCounty) {
				maxCounty = pCountY[yc];
				y0 = yc;
			}
		}
	}
	//接下来，计算R
	int r;
	for (int i = 0; i < cnt; i++) {
		r = int(sqrt((posx[i]-x0)* (posx[i] - x0)+(posy[i]-y0)* (posy[i] - y0)));
		pCountR[r]++;
		if (pCountR[r] > maxCountr) {
			maxCountr = pCountR[r];
			R = r;
		}
	}

	printf("计算得到的坐标（%d,%d）,半径%d\n", x0, y0, R);
	for (int i = 0; i < width * height; i++) {
		if (pGryImg[i] == 255) {
			pGryImg[i] = 250;
		}
	}
	//画圆
	RmwDrawCircle(pGryImg, width, height, x0,y0, R, 255);//用255做标记
	Rmw_Write8BitImg2BmpFileMark(pGryImg, width, height, (targetfile+"_circle.bmp").c_str());
	//画直方图X
	memcpy(gDbgImg, pGryImg, width* height);
	RmwDrawData2ImgCol(pCountX, height, 1.0, gDbgImg, width, height, 254);
	Rmw_Write8BitImg2BmpFileMark(gDbgImg, width, height, (targetfile + "_circle_col.bmp").c_str());
	//画直方图Y
	memcpy(gDbgImg, pGryImg, width * height);
	RmwDrawData2ImgRow(pCountY, width, 1.0, gDbgImg, width, height, 254);
	Rmw_Write8BitImg2BmpFileMark(gDbgImg, width, height, (targetfile + "_circle_row.bmp").c_str());
	//半径的直方图
	int w, h;
	w = int(sqrt(width * width + height * height * 1.0)) + 1;
	h = R + 20;
	gDbgImg = new unsigned char[w * h];
	RmwDrawData2ImgCol(pCountR, width, 1.0, gDbgImg, w, h, 253);
	Rmw_Write8BitImg2BmpFileMark(gDbgImg, w,h, (targetfile + "_circle_r.bmp").c_str());
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete gDbgImg;
	delete pCountR;
	delete pCountX;
	delete pCountY;
	printf("Press any key to exit!\n");
	getchar();
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//	这边是5-6题调用的函数
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//将边框设置为color值
void setBoundary(BYTE*pBinImg,int width,int height,BYTE color) {
	BYTE* pRow;
	int y;

	memset(pBinImg, color, width);//第一行
	memset(pBinImg + (height - 1) * width, color, width);
	for (y = 0, pRow = pBinImg; y < height-1; y++, pRow += width)
	{
		*pRow = 0;
		*(pRow + width - 1) = 0;
	}
	memset(pBinImg, color, width);//最后一行
	return;
}
static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
//跟踪算法
int TraceContour(BYTE* pBinImg, int width, int height,//二值图像
	int x0, int y0, //轮廓起点
	bool isOuter, //是否是外轮廓
	BYTE* pChainCode, //外部申请好的一个内存空间，用来存放链码
	int maxChainCodeNum //可以存放的最大链码个数
)
{
	static int initCode[8] = { 7, 7, 1, 1, 3, 3, 5, 5 };
	int dADD[8]; //地址偏移量
	BYTE* pBegin, * pCur, * pTst; //轮廓起点,当前点,检查点
	int code, beginCode, returnCode, i;
	int N;

	// step.1-----初始化----------------------------------------//
	//不同链码对应的地址偏移量
	for (code = 0; code < 8; code++) dADD[code] = dy[code] * width + dx[code];
	pBegin = pBinImg + y0 * width + x0; //轮廓起点的地址
	pCur = pBegin; //当前点设置到轮廓起点
	if (isOuter) //外轮廓时的初始化
	{
		*(pCur - 1) = 1;  //左侧是背景点,标记为灰度值1
		code = 7; //初始化为7
	}
	else //内轮廓时的初始化
	{
		*(pCur + 1) = 1; //右侧是背景点,标记为灰度值1
		code = 3; //初始化为3
	}
	beginCode = initCode[code]; //从起点检查的第一个链码
	// step.2-----轮廓跟踪--------------------------------------//
	N = 0; //链码个数初始化为0
	do {
		*pCur = 254; //是轮廓点,标记为灰度值254
		for (i = 0, code = initCode[code]; i < 7; i++, code = (code + 1) % 8)
		{
			pTst = pCur + dADD[code]; //得到要检查的轮廓点的地址
			if (*pTst < 2) *pTst = 1; //是背景点,标记为灰度值1
			else //是轮廓点
			{
				if (N < maxChainCodeNum) pChainCode[N++] = code; //保存链码
				if (pTst == pBegin) //回到起点的处理
				{
					//找出剩余链码的起始序号
					returnCode = (code + 4) % 8; //转换为于从起点出发已经检查过的链码
					for (i = 0, code = beginCode; i < 7; i++, code = (code + 1) % 8)
					{
						if (code == returnCode)
						{
							i++; //剩余链码的起始序号
							code = (code + 1) % 8; //剩余链码的起始值
							break;
						}
					}
					//检查剩余链码
					for (; i < 7; i++, code = (code + 1) % 8)
					{
						pTst = pBegin + dADD[code];
						if (*pTst < 2) *pTst = 1; //是背景点,标记为灰度值1
						else
						{   //保存链码
							if (N < maxChainCodeNum) pChainCode[N++] = code;
							break; //从起点开始,找到了新的轮廓点pTst
						}
					}
				}
				break; //找到了新的轮廓点pTst
			}
		}
		pCur = pTst; //当前点移动到新的轮廓点pTst
	} while (i < 7); //找到轮廓点时一定有i<7
	// step.3-----结束-----------------------------------------//
	return N; //返回链码个数
}
//外接矩形
void ContourRect(int x0, int y0, BYTE* pCode, int N, int* x1, int* x2, int* y1, int* y2)
{
	int i;

	*y1 = *y2 = y0;
	*x1 = *x2 = x0;
	for (i = 0; i < N - 1; i++)
	{
		x0 = x0 + dx[pCode[i]];
		y0 = y0 + dy[pCode[i]];

		if (x0 > *x2) *x2 = x0;
		else if (x0 < *x1) *x1 = x0;

		if (y0 > *y2) *y2 = y0;
		else if (y0 < *y1) *y1 = y0;
	}
	return;
}
//轮廓线填充
void FillContour(BYTE* pGryImg, int width, int height,//待填充图像
	int x0, int y0, //轮廓起点
	bool isOuter, //是否是外轮廓
	BYTE* pCode, //链码
	int N, //链码个数
	int regionC, //区域内部的颜色
	int contourC, //轮廓点颜色
	int nonC //不存在的颜色
)
{
	
	int dADD[8]; //地址偏移量
	BYTE* pBegin, * pCur, * pTst; //轮廓起点,当前点,填充点
	int inCode, outCode, i;
	int sumC, sumDy, direction;

	// step.1-----初始化----------------------------------------//
	pBegin = pGryImg + y0 * width + x0; //轮廓起点的地址
	//不同链码对应的地址偏移量
	for (i = 0; i < 8; i++) dADD[i] = dy[i] * width + dx[i];
	// step.2-----轮廓点的所有链码纵坐标变化量之和赋初值----------//
	for (sumC = 2; sumC <= 253; sumC++) //求sumC
	{
		if ((regionC >= sumC - 2) && (regionC <= sumC + 2)) continue;
		if ((nonC >= sumC - 2) && (nonC <= sumC + 2)) continue;
		break;
	}
	for (i = 0, pCur = pBegin; i < N; pCur += dADD[pCode[i]], i++) *pCur = sumC;
	// step.3-----计算轮廓点的所有链码纵坐标变化量之和------------//
	inCode = pCode[N - 1]; //进入轮廓起点的链码
	for (i = 0, pCur = pBegin; i < N; i++)
	{
		outCode = pCode[i]; //从该轮廓点出发的链码
		*pCur += dy[inCode] + dy[outCode]; //像素的灰度值就是变化量之和 
		inCode = outCode; //当前轮廓点的出发码就是下一个轮廓点的进入码
		pCur += dADD[outCode]; //指向下一个轮廓点的地址
	}
	// step.4-----对填充起点和填充终点进行标记--------------------//
	for (i = 0, pCur = pBegin; i < N; pCur += dADD[pCode[i]], i++)
	{
		sumDy = *pCur;
		if ((sumDy == sumC + 1) || (sumDy == sumC + 2)) *pCur = regionC; //标记为填充起点
		else if ((sumDy == sumC - 1) || (sumDy == sumC - 2)) *pCur = nonC; //标记为填充终点
	}
	// step.5-----按行在填充起点和填充终点之间进行填充------------//
	direction = isOuter ? 1 : -1; //外轮廓是从左向右,所以是+1;内轮廓反之
	for (i = 0, pCur = pBegin; i < N; pCur += dADD[pCode[i]], i++)
	{
		if (*pCur == regionC) //找到一个填充起点
		{
			pTst = pCur;
			while (*pTst != nonC) //一直填充到终点
			{
				*pTst = regionC;
				pTst += direction;
			}
			*pCur = nonC; //该水平段已经填充过了,避免重复填充
		}
	}
	// step.6-----对轮廓点的颜色进行赋值-------------------------//
	for (i = 0, pCur = pBegin; i < N; pCur += dADD[pCode[i]], i++) *pCur = contourC;
	// step.7-----结束-----------------------------------------//
	return;
}
//绘制轮廓线
void DrawContour(BYTE* pGryImg, int width, int x, int y, BYTE* pCode, int N, BYTE color)
{
	int i, dADD[8];
	BYTE* pCur;

	*(pGryImg + y * width + x) = color; // 为了能够处理不闭合的轮廓
	for (i = 0; i < 8; i++) dADD[i] = dy[i] * width + dx[i];
	for (pCur = pGryImg + y * width + x, i = 0; i < N; i++)
	{
		pCur += dADD[pCode[i]];
		*pCur = color;
	}
	return;
}
/*任务三：跟踪和填充*/
void Exam3() {
	unsigned char* pGryImg,*pCur;
	int width, height;
	int x, y;
	int N, S;
	int x1, x2, y1, y2;
	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0605Bin.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pDbgImg = new BYTE[width * height];//存放答案图像
	memset(pDbgImg, 0, width * height);
	// step.2 -------------链码跟踪----------------------//
	//首先将边框设置成0
	setBoundary(pGryImg, width, height, 0);
	//接着是遍历，从上到下，从左往右扫描图像，寻找新的轮廓
	for (y = 1, pCur = pGryImg + y * width; y < height - 1; y++) {
		pCur++;//跳过最左边的轮廓
		for (x = 1; x < width - 1; x++, pCur++) {
			if ((*pCur == 255) && (*(pCur - 1) <= 1)) {
				//发现了一条外轮廓
				N = TraceContour(pGryImg, width, height,
					x, y, //轮廓起点
					true, //是外轮廓
					gChainCode, //用来存放链码的数组
					MAX_CONOUR_LENGTH //数组的大小
				);
				//计算面积
				S = RmwContourPixels(gChainCode, N);
				//外接矩形
				ContourRect(x, y, gChainCode, N, &x1, &x2, &y1, &y2);
				if ((x1 > 1) && (y1 > 1) && (x2 < width - 2) && (y2 < height - 2) && //去掉贴边者
					(S > 200) && //去掉残缺者 
					(S < 400) //去掉粘连者
					)
				{
					FillContour(pDbgImg, width, height,//待填充图像
						x, y, //轮廓起点
						true, //是否是外轮廓
						gChainCode, //链码
						N, //链码个数
						255, //区域内部的颜色
						255, //轮廓点颜色
						250 //不存在的颜色
					);
					DrawContour(pDbgImg, width, x, y, gChainCode, N, 254);
				}
				else {
					DrawContour(pDbgImg, width, x, y, gChainCode, N, 253);
				}
			}
			else if ((*pCur == 0) && (*(pCur - 1) >= 254)) {
				//发现了一条内轮廓
				N = RmwTraceContour(pGryImg, width, height,
					x - 1, y,  //轮廓起点,注意是x-1
					false, //是内轮廓
					gChainCode, //用来存放链码的数组
					MAX_CONOUR_LENGTH //数组的大小
				);
				//调试
				DrawContour(pDbgImg, width, x - 1, y, gChainCode, N, 253);

			}
		}
		pCur++;//跳过最右侧的点
	}
	// step.3  -------------保存结果----------------//
	Rmw_Write8BitImg2BmpFileMark(pDbgImg, width, height, "./res_data/H0605Bin_trace_fill.bmp");

	//再加上膨胀和腐蚀的效果
	//膨胀
	for (int i = 0; i < width * height; i++) {
		pDbgImg[i] = (pGryImg[i] > 200) * 255;//轮廓被标记成为253，254，设置成255即可实现膨胀
	}
	Rmw_Write8BitImg2BmpFile(pDbgImg, width, height, "./res_data/H0605Bin_expand.bmp");
	//收缩
	for (int i = 0; i < width * height; i++) {
		pDbgImg[i] = (pGryImg[i] ==255) * 255;//轮廓被标记成为253，254，设置成0即可实现收缩
	}
	Rmw_Write8BitImg2BmpFile(pDbgImg, width, height, "./res_data/H0605Bin_shrink.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
/*任务四：判断顺时针还是逆时针*/

void reverseArr(int arr[],int len) {
	//逆转数组
	int t;
	for (int i = 0; i < len / 2; i++)
	{
		t = arr[i];
		arr[i] = arr[len - 1 - i];
		arr[len - 1 - i] = t;
	}
}
void Exam4(int chainCodes[],int len) {
	//输入链码，输出是否为顺时
	//假设起始位置为(0,0)
	int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	int sum = 0;
	int x=0, y=0;
	int nextx, nexty;
	printf("输入链码：{ ");
	for (int i = 0; i < len; i++) {
		//展示链码
		printf("%d", chainCodes[i]);
		if (i != len - 1) {
			printf(",");
		}
		nextx = x + dx[chainCodes[i]];
		nexty = y + dy[chainCodes[i]];
		sum += x * nexty - y * nextx;
		x = nextx;
		y = nexty;
	}
	printf(" }");
	if (sum > 0) {
		printf("\n顺时针\n");
	}
	else {
		printf("\n逆时针\n");
	}
}

int main() {
	string filename1="./data/H0602Bin.bmp";
	string targetname1 = "./res_data/H0602Bin";
	//Test(0);//(0,139);(0,0);(1,-237);(1,-237)
	//Exam1(filename1,targetname1);//可以参考Test1写绘图的部分
	Exam1_1(filename1, targetname1);
	//string filename = "./data/H0604Bin.bmp";
	//string targetname = "./res_data/H0604Bin";
	//Exam2(filename,targetname);
	//Exam3();
	//int arr[]{ 5,5,5,5,4,4,4,6,6,6,1,1,1,1,0,0,0,2,2,2 };
	//int len = sizeof(arr) / sizeof(arr[0]);
	//Exam4(arr,len);
	//reverseArr(arr,len);
	//Exam4(arr,len);
	return 0;
}