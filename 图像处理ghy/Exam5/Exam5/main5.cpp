#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include<string>
#include<iostream>
#include<string.h>
#include <intrin.h> 
#include "bmpFile.h"
#include "RmwALG_EdgeSharpen.h"
//#include "RmwALG_EdgeDetect.h"
#include "RmwALG_2DHistogram.h"
#include "RmwLocateMark.h"


using namespace std;
/*运行测试4.6*/
void testEdgeSharpen()
{
	unsigned char* pGryImg;
	int width, height;

	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0401Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	// step.2-------------处理图像--------------------------//
	//0.5
	RmwEdgeShapen(pGryImg, width, height, 0.5, pResImg);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/shapen_0_5_H0401Gry.bmp");
	//2
	RmwEdgeShapen(pGryImg, width, height, 2, pResImg);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/shapen_2_H0401Gry.bmp");
	//4
	RmwEdgeShapen(pGryImg, width, height, 4, pResImg);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/shapen_4_H0401Gry.bmp");
	//8
	RmwEdgeShapen(pGryImg, width, height, 8, pResImg);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/shapen_8_H0401Gry.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete pResImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
/*Soble：一阶*/
/*
void testSobelRice()
{
	unsigned char* pGryImg;
	int width, height;

	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0401Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	// step.2-------------处理图像--------------------------//
	RmwSobelGryImg(pGryImg, width, height, pResImg);
	// step.3-------------保存图像--------------------------//
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/Fig0431Gry_1_Sobel.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete pResImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
*/
/*沈算子：二阶*/
/*
void testShenJunRice()
{
	unsigned char* pGryImg;
	int width, height;

	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0401Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	BYTE* pTmpImg = new BYTE[width * height];
	// step.2-------------处理图像--------------------------//
	RmwShenJunGryImg(pGryImg, pTmpImg, width, height, 0.5, pResImg);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/H0401_shen.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete pResImg; //释放自己申请的内存
	delete pTmpImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
*/
/*米粒的边缘检测算法：一阶、二阶以及提高运算速度*/
/*
* void testExtractRiceEdge()
{
	unsigned char* pGryImg;
	int width, height;

	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0401Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	BYTE* pTmpImg = new BYTE[width * height];
	// step.2-------------处理图像--------------------------//
	RmwExtractRiceEdge(pGryImg, pTmpImg, width, height, 0.5, 32 * 2, pResImg);
	// step.3-------------保存图像--------------------------//
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/H0401Gry_3.bmp");
	//填充上颜色
	for (int i = 0; i < width * height; i++)
	{
		*(pGryImg + i) = max(pGryImg[i], pResImg[i]);
	}
	Rmw_Write8BitImg2BmpFileMark(pGryImg, width, height, "./res_data/H0401Gry_4_Overlay.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete pResImg; //释放自己申请的内存
	delete pTmpImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
*/
/*4.7 */

/*5-2 直方图、Otus阈值、二值化*/
void Exam5_2() {
	unsigned char* pGryImg;
	unsigned char* pImg;//图像指针
	int width, height;
	int thre, num=0;
	int histogram[256];
	memset(histogram,0, sizeof(histogram));
	
	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0501Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	//直方图参数更新:就是单纯用原图
	pImg = pGryImg;
	int gmin=255, gmax=0;
	for (; pImg < pGryImg + height * width; pImg++) {
		histogram[*pImg]++;
		if (*pImg > gmax) {
			gmax = *pImg;
		}
		if (*pImg < gmin) {
			gmin = *pImg;
		}
		num++;
	}
	//num = RmwHistogramBy2D(pGryImg, pAvrImg, width, height, 4 * 2, histogram);
	//绘制直方图
	RmwDrawHistogram2Img(histogram, 256, 1.0, pResImg, width, height);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/histogram.bmp");
	// step.3-------------二值化----------------------------//
	//得到阈值
	thre = my_RmwGetOtsuThreshold(histogram, 256, gmin, gmax);
	printf("\nthre1=%d\n",  thre);
	//二值图像
	RmwThreshold(pGryImg, width, height, thre);
	Rmw_Write8BitImg2BmpFile(pGryImg, width, height, "./res_data/H0401Gry_Bin.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete pResImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;

}
/*5-3滤波再切割*/
void Exam5_3() {
	unsigned char* pGryImg;
	unsigned char* pImg;//图像指针
	int width, height;
	int thre1,thre2, num = 0;
	int histogram[256];
	int reshistogram[256];
	memset(histogram, 0, sizeof(histogram));
	memset(reshistogram, 0, sizeof(reshistogram));
	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0501Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	//直方图参数更新:原图的直方图
	pImg = pGryImg;
	int gmin = 255, gmax = 0;
	for (; pImg < pGryImg + height * width; pImg++) {
		histogram[*pImg]++;
		if (*pImg > gmax) {
			gmax = *pImg;
		}
		if (*pImg < gmin) {
			gmin = *pImg;
		}
		num++;
	}
	//平滑后得到新的直方图
	int sum = histogram[0] + histogram[1] + histogram[2] + histogram[3] + histogram[4];
	for (int i = 2; i < 254; i++) {
		reshistogram[i] = sum / 5;
		sum = sum - histogram[i - 2] + histogram[i + 2];
	}
	//绘制平滑后的直方图
	RmwDrawHistogram2Img(reshistogram, 256, 1.0, pResImg, width, height);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/histogram_5.bmp");
	// step.3-------------二值化----------------------------//
	//得到阈值
	thre1 = my_RmwGetOtsuThreshold(histogram, 256, gmin, gmax);
	thre2 =RmwGetOtsuThreshold(reshistogram, 256);
	printf("\nthre1=%d\nthre2=%d\n",  thre1,thre2);
	//二值图像
	RmwThreshold(pGryImg, width, height, thre2);
	Rmw_Write8BitImg2BmpFile(pGryImg, width, height, "./res_data/H0401Gry_Bin_5.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete pResImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
/*5-4除法再切割*/
void Exam5_4() {
	unsigned char* pGryImg;
	unsigned char* pImg;//图像指针
	int width, height;
	int thre, num = 0;
	int histogram[256];
	memset(histogram, 0, sizeof(histogram));

	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0501Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	//直方图参数更新:就是单纯用原图
	pImg = pGryImg;
	for (; pImg < pGryImg + height * width; pImg++) {
		*pImg = (*pImg) / 4;
		histogram[*pImg]++;
	}
	//绘制直方图
	RmwDrawHistogram2Img(histogram, 256, 1.0, pResImg, width, height);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/histogram4.bmp");
	// step.3-------------二值化----------------------------//
	//得到阈值
	thre = RmwGetOtsuThreshold(histogram, 256);
	printf("\nthre3=%d\n",  thre);
	//二值图像
	RmwThreshold(pGryImg, width, height, thre);
	Rmw_Write8BitImg2BmpFile(pGryImg, width, height, "./res_data/H0401Gry_Bin4.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	delete pResImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
/*文本定位——老师给的方法*/
void Exam4() {
	clock_t start_t, end_t;
	start_t = clock();
	/*读取文件，并且切割文字*/
	class RmwLocateMark gRmwPos;
	BYTE* pGryImg;
	int width, height;
	char filename[255];
	//处理
	clock_t t1, t2;
	bool isFirst = true;
	bool ret;
	//结果
	BYTE* pResImg;
	int w, h;
	RMWRECT refPos;
	for (int i = 2; i <= 4; i++)
	{
		
		//读图像
		sprintf_s(filename, "./data/H040%dGry.bmp", i);
		//cout << "正在执行文件：" << filename << "\n";
		pGryImg = Rmw_Read8BitBmpFile2Img(filename, &width, &height);
		if (!pGryImg)
		{
			printf("*file open err!\n");
			getchar();
			continue;
		}
		else {
			//printf("%s,width=%d,height=%d\n", filename, width, height);
		}
		//初始化
		if (isFirst)
		{
			isFirst = false;
			ret = gRmwPos.Initialize(1024, //支持的最大图像宽度
				1024, //支持的最大图像高度
				false, //灰度反相
				2, //计算模式,1-使用灰度,2-使用梯度
				3, //加速,0-不加速,1-加速4倍,2-加速16倍(目标很大时)
				false //是否调试
			);
			if (!ret)
			{
				printf("********* gRmwPos.Initialize() Err! ************* \n");
				getchar();
				getchar();
				delete pGryImg;
				return;
			}
		}
		//定位
		t1 = clock();
		for (int k = 0; k < 100; k++)
			gRmwPos.DoNext(pGryImg, //当前图像
				width, //当前图像的宽度  
				height, //当前图像的高度
				100, //目标宽度
				100, //目标高度
				50, //去掉左边的宽度,可以为0
				50, //去掉右边的宽度,可以为0
				15, //去掉上边的的高度,可以为0
				15, //去掉上边的的高度,可以为0
				i //帧号
			);
		t2 = clock();
		//结果
		pResImg = gRmwPos.GetTargetImg(16, &w, &h, &refPos);
		//sprintf_s(filename, "./res_data/H040%dGry_detect%d.bmp", i, t2 - t1);
		//Rmw_Write8BitImg2BmpFileMark(pResImg, w, h, filename);
		//下一幅
		delete pGryImg;
	}
	end_t = clock();
	cout << "执行结束消耗"<<(end_t - start_t)/1000.0<<"毫秒";
	getchar();
	getchar();
	return;
}
/*重新改写*/
void searchMaxPos(
	int* pSumImg, int width, int height,
	int w, int h,
	int delLRUk,
	int delLRUz,
	int* x1, int* x2, int* y1,
	int* y2
) {
	int* pLine, * pCur, * pL, * pR;
	int x, y, k, z;
	int maxV, bstX, bstY, sumC, sumL, sumR, sumU = 0;
	k = delLRUk;
	z = delLRUz;
	maxV = -INT_MAX;
	bstX = bstY = 0;
	for (y = z, pLine = pSumImg + z * width; y < height - h; y++, pLine += width) {
		for (x = 1 + k, pCur = pLine + x; x < width - w - k; x++, pCur++) {
			sumC = *(pCur + h * width + w) - *(pCur + h * width) - *(pCur + w) + *pCur;
			//左边
			pL = pCur - k;
			sumL = *(pL + h * width + k) - *(pL + h * width - 1) - *(pL + k) + *pL;
			//右边
			pR = pCur + w;
			sumR = *(pR + h * width + k) - *(pR + h * width - 1) - *(pR + k) + *pR;
			//上边
			sumU = *(pCur + w - width) - *(pCur - width) - *(pCur + w - (z - 1) * width) + *(pCur - (z - 1) * width);
			if (sumC - sumL-sumU -sumR> maxV) {
				maxV = sumC  -sumL-sumU-sumR;
				bstX = x;
				bstY = y;
			}
		}
	}
	*x1 = bstX; 
	*x2 = *x1 + w;
	*y1 = bstY;
	*y2 = *y1 + h;
}
//压缩图像
void toSmall(BYTE* pGryImg, int width, int height, BYTE* pResImg, int* pSumImg, int nSize) {
	//将图像压缩:nSize取2的幂次
	int* pImg;
	BYTE* pRes;
	int* pLine;
	int x, y;
	int sq = sqrt(nSize);
	
	int temp;
	/*
	for (y = 1, pLine = pSumImg + width, pRes = pResImg; y < height - sq; y += sq, pLine += sq * width) {
		for (x = 1, pImg = pLine + x; x < width - sq; x += sq, pImg += sq) {
			temp = *(pImg + sq * width + sq) + *(pImg - width) - *(pImg + sq * width) - *(pImg - width + sq);
			*(pRes++) = temp / nSize;
		}
		pRes++;
	}
	*/
	for (y = 1, pLine = pSumImg + width, pRes = pResImg; y < height - sq; y += sq, pLine += sq * width) {
		for (x = 1, pImg = pLine + x; x < width - sq; x += sq, pImg += sq) {
			temp = *(pImg + (width << 2) + sq) + *(pImg - width) - *(pImg + (width << 2) )- *(pImg - width + sq);
			*(pRes++) = temp / nSize;
		}
		pRes++;
	}

}
//边缘检测
void Sobel(BYTE* pGryImg, int width, int height, BYTE* pResImg) {
	BYTE* pImg, * pRes;//指针：图像，结果
	int dx, dy, x, y;
	memset(pResImg, 0, sizeof(pResImg));
	for (y = 1, pImg = pGryImg + width, pRes = pResImg + width; y < height - 1; y++) {
		*(pRes++) = 0;//边边不做，赋值0
		pImg++;
		for (x = 1; x < width - 1; x++, pImg++) {
			//求dx
			dx = *(pImg - 1 - width) + (*(pImg - 1) << 1) + *(pImg - 1 + width);
			dx -= *(pImg + 1 - width) + (*(pImg + 1) << 1) + *(pImg + 1 + width);
			//求dy
			dy = *(pImg - 1 - width) + (*(pImg - width) << 1) + *(pImg + 1 - width);
			dy -= *(pImg - 1 + width) + (*(pImg + width) << 1) + *(pImg + 1 + width);
			//结果
			*(pRes++) = min(255, abs(dx) + abs(dy));
		}
		*(pRes++) = 0;//边边不做，赋值0
		pImg++;
	}
}
//计算积分图
void doSumImg_SSE(BYTE* pGryImg, int width, int height, int* pSumImg) {
	//利用SSE加速得到积图
	_declspec(align(16)) int sumCol[4096];//约定图像宽度不大于4096，16字节对齐
	__m128i* pSumSSE, A;
	BYTE* pImg;
	int* pRes;
	int x, y;
	memset(sumCol, 0, sizeof(sumCol));
	for (y = 0, pImg = pGryImg, pRes = pSumImg; y < height; y++) {
		//0
		sumCol[0] += *(pImg++);
		*(pRes++) = sumCol[0];
		//1
		sumCol[1] += *(pImg++);
		*(pRes++) = *(pRes - 1) + sumCol[1];
		//2
		sumCol[2] += *(pImg++);
		*(pRes++) = *(pRes - 1) + sumCol[2];
		//3
		sumCol[3] += *(pImg++);
		*(pRes++) = *(pRes - 1) + sumCol[3];
		//下面就是正常的加速
		for (x = 4, pSumSSE = (__m128i*)(sumCol + 4); x < width; x += 4, pImg += 4) {
			A = _mm_cvtepu8_epi32(_mm_loadl_epi64((__m128i*)pImg));
			//4个32位的整数相加
			*(pSumSSE++) = _mm_add_epi32(*pSumSSE, A);
			for (int i = 0; i < 4; i++) {
				*(pRes++) = *(pRes - 1) + sumCol[x + i];
			}
		}
	}

}
void Draw(BYTE* pGryImg, int width, int w, int h, int* left, int* right, int* up, int* down) {
	//将结果绘制在原图上
	BYTE* pImg, * pCur;
	int x, y;

	//上方
	y = *up;
	x = *left;
	pCur = pGryImg + y * width + *left;
	x = 0;
	while (x < w) {
		*(pCur++) = 255;
		x++;
	}
	//下方
	y = *down;
	x = *left;
	pCur = pGryImg + y * width + x;
	x = 0;
	while (x < w) {
		*(pCur++) = 255;
		x++;
	}
	//左方
	y = *up, x = *left;
	pCur = pGryImg + y * width + x;
	y = 0;
	while (y < h) {
		*pCur = 255;
		pCur += width;
		y++;
	}
	//右方
	y = *up, x = *right;
	pCur = pGryImg + y * width + x;
	y = 0;
	while (y < h) {
		*pCur = 255;
		pCur += width;
		y++;
	}
	return;
}
//图像复制
void myCopyGryRect(BYTE* pGryImg, int width, int height, int x1, int x2, int y1, int y2, BYTE* pRectImg)
{   //图像块复制
	BYTE* pRow, * pRect;
	int y, w;
	pRectImg = new unsigned char[(y2 - y1) * (x2 - x1)];
	w = x2 - x1 + 1;
	pRect = pRectImg;
	for (y = y1, pRow = pGryImg + y * width + x1; y <= y2; y++, pRow += width)
	{
		memcpy(pRect, pRow, w);
		pRect += w;
	}
	return;
}
void myExam4() {
	clock_t start_t, end_t;
	
	char filename[255];
	unsigned char* pGryImg;
	int height, width;

	int w = 22, h = 26;//目标大小
	int shrink_a = 16;//缩放比例
	int shrink_h, shrink_w;//缩放后的目标大小
	int sq = sqrt(shrink_a);
	int* x1 = new int;
	int* x2 = new int;
	int* y1 = new int;
	int* y2 = new int;
	unsigned char* pResImg=NULL ;
	start_t = clock();
	for (int i = 2; i <= 4; i++) {
		//读图像
		sprintf_s(filename, "./data/H040%dGry.bmp", i);
		//std::cout << "正在执行文件：" << filename << "\n";
		pGryImg = Rmw_Read8BitBmpFile2Img(filename, &width, &height);
		//这边如果图片大小固定的话，可以写死缩放大小和图片大小
		shrink_h = height / sq - (height / sq) % sq;
		shrink_w = width / sq - (width / sq) % sq;
		unsigned char* pShrinkImg = new unsigned char[shrink_h * shrink_w];
		int* pSumImg = new int[width * height];//积分图
		int* pSumShrinkImg = new int[shrink_h * shrink_w];//压缩后的积分图
		unsigned char* pSbImg = new unsigned char[shrink_h * shrink_w];
		if (!pGryImg)
		{
			printf("*file open err!\n");
			getchar();
			continue;
		}
		else {
			//printf("%s,width=%d,height=%d\n", filename, width, height);
		}
		//对原始图像进行处理
		
		//1、计算原始图像的积分图
		doSumImg_SSE(pGryImg, width, height, pSumImg);
		//2、压缩原始图像
		toSmall(pGryImg, width, height, pShrinkImg, pSumImg, shrink_a);//压缩图像
		//sprintf_s(filename, "./res_data/my_dect_H040%dGryShrink.bmp", i);
		//Rmw_Write8BitImg2BmpFile(pShrinkImg, shrink_w, shrink_h, filename);
		//3、边缘强度检测压缩后的图像
		Sobel(pShrinkImg, shrink_w, shrink_h, pSbImg);
		//sprintf_s(filename, "./res_data/my_dect_H040%dGrySb.bmp", i);
		//Rmw_Write8BitImg2BmpFile(pSbImg, shrink_w, shrink_h, filename);
		//4、计算压缩后的图像的积分图
		doSumImg_SSE(pShrinkImg, shrink_w, shrink_h, pSumShrinkImg);
		//5、搜索块
		//对x的限制 对y的限制
		searchMaxPos(pSumShrinkImg,shrink_w,shrink_h,w,h,20,25,x1,x2,y1,y2);
		*x1 = (*x1) * sq;
		*x2 = (*x1) +w*sq;
		*y1 = (*y1) * sq;
		*y2 = (*y1) + h * sq;
		//std::cout << *x1 << " " << *x2 << " " << *y1 << " " << *y2 << endl;
		//6、返回绘制的结果
		//Draw(pGryImg,width,w*sq,h*sq,x1,x2,y1,y2);
		//sprintf_s(filename, "./res_data/my_dect_H040%dGry.bmp", i);
		//Rmw_Write8BitImg2BmpFile(pGryImg, width, height,filename);
		
	}
	end_t = clock();
	std::cout << "执行结束消耗" << (end_t - start_t) / 1000.0 << "毫秒";
}
int main() {
	//testEdgeSharpen();
	//testExtractRiceEdge();/*4.6是这个*/
	//Exam5_2();
	//Exam5_3();
	//Exam5_4();
	


	//声明：因为部分函数名重叠所以运行完上述实验可以重开一个项目再运行Exam4(),注意头文件的添加和删除，不然一堆bug~
	//这个是老师的方法
	Exam4();
	//我的方法
	//myExam4();
	return 0;
}