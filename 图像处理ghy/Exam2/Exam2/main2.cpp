#include<time.h>
#include<stdio.h>
#include<vector>
#include<Windows.h>
#include<string>
#include<string.h>
#include"RmwALG_Histogram.h"
#include"bmpFile.h"

using namespace std;
/*任务1：彩色图像转化成灰度图像*/
void Color2Gry_Count(unsigned char* pImg, int width, int height) {
	//利用计算的方式
	unsigned char* p = pImg, * pEnd = pImg + height * width * 3;
	for (; p != pEnd; p++) {
		int r = *p;
		int g = *(p + 1);
		int b = *(p + 2);
		int gry= (299 * r + 587 * g + 114 * b) / 1000;
		*p = gry;
		*(++p) = gry;
		*(++p) = gry;
	}

}
int LUT[256][256][256];
void Color2Gry_Table(unsigned char *pImg,int width,int height) {
	//利用查找表的方式
	unsigned char* p = pImg, * pEnd = pImg + height * width * 3;
	for (; p != pEnd; p++) {
		int r = *p;
		int g = *(p + 1);
		int b = *(p + 2);
		
		*p = LUT[r][g][b];
		*(++p) = LUT[r][g][b];
		*(++p) = LUT[r][g][b];
	}
	
}
void Exam2_1() {
	/*----------------------加载图像----------------------------*/
	unsigned char* pRgbImg;
	unsigned char *pRgbImg_copy;
	int width, height;
	pRgbImg = Rmw_Read24BitBmpFile2Img("./data/H0201Rgb.bmp", &width, &height);
	pRgbImg_copy = pRgbImg;
	if (!pRgbImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	/*---------------1基于查找表的方式-----------------------*/
	clock_t start_t = clock();
	//初始化查找表
	for (int r = 0; r < 256; r++) {
		for (int g = 0; g < 256; g++) {
			for (int b = 0; b < 256; b++) {
				LUT[r][g][b] = (299 * r + 587 * g + 114 * b) / 1000;
			}
		}
	}
	for (int i = 0; i < 1000; i++) {
		Color2Gry_Table(pRgbImg,width,height);
	}
	//将数据回写
	Rmw_Write24BitImg2BmpFile(pRgbImg, width, height, "./res_data/H0201Gry_table.bmp");
	clock_t end_t = clock();
	printf("基于查找表的方法耗时:%d\n", end_t - start_t);

	/*----------------2基于计算的方法------------------------------------*/
	start_t = clock();
	unsigned char* pImg;
	for (int i = 0; i < 1000; i++) {
		pImg = pRgbImg_copy;
		Color2Gry_Count(pImg, width, height);
	}
	Rmw_Write24BitImg2BmpFile(pImg, width, height, "./res_data/H0201Gry_count.bmp");
	end_t = clock();
	printf("基于计算的的方法耗时:%d\n", end_t - start_t);
}

/*任务2：均值方差规定化*/
void Mean_variance_specification(unsigned char* pImg, int width, int height,double b1,double c1,double b2,double c2) {
	//b1原始均值 c1原始方差
	unsigned char* p = pImg, * pEnd = pImg + height * width ;

	while (p != pEnd) {
		*p = min(max((*p - b1) * c2 / c1 + b2, 0), 255);
		p++;
	}

}
void Stretch(unsigned char* pImg, int width, int height, int gmin, int gmax, int Gmin, int Gmax) {
	//线性拉伸
	int k = (Gmax - Gmin) / (gmax - gmin);
	int b = Gmin - k * gmin;
	unsigned char* p = pImg, *pEnd = pImg + height * width;
	while (p != pEnd) {
		*p = k * (*p) + b;
		p++;
	}
}
void Exam2_2() {
	/*----------------------加载图像----------------------------*/
	unsigned char* pGryImg;
	int width, height;
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0201Grey.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	else {
		printf("成功加载!\n");
	}
	//获取b1,c1
	double b1, c1;
	int* histogram = new int[256];
	RmwGetHistogram(pGryImg, width, height,histogram);
	GetBrightContrast(histogram, &b1, &c1);
	printf("c=%.f  b=%.f\n", c1, b1);
	//获取原始最大值最小值
	int gmin, gmax;
	GetMinMaxGry(histogram, &gmin, &gmax);
	printf("min=%d max=%d\n", gmin, gmax);
	//接下来均值方差规定化操作
	double b2[] = {16,10,20,21,30};
	double c2[] = {17,19,30,40,40};
	string path = "./res_data/H0201Gry_mvs_stretch";
	string add = ".bmp";
	int Gmin = 0;
	int Gmax = 255;
	for (int i = 0; i < 5; i++) {
		unsigned char* pImg = pGryImg;
		Mean_variance_specification(pImg, width, height, b1, c1,b2[i],c2[i]);
		Stretch(pImg, width, height, gmin, gmax, Gmin, Gmax);
		string fpath = path + to_string(i);
		fpath += add;
		Rmw_Write8BitImg2BmpFile(pImg, width, height, fpath.c_str());
	}
}
/*任务3：均衡化*/
void Exam2_3() {
	/*----------------------加载图像----------------------------*/
	unsigned char* pGryImg;
	int width, height;
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0203Girl.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	else {
		printf("成功加载!\n");
	}
	//均衡化
	RmwHistogramEqualizeDemo(pGryImg, width, height);
	//保存图像
	string fpath = "./res_data/H0203Girl_res.bmp";
	Rmw_Write8BitImg2BmpFile(pGryImg, width, height, fpath.c_str());
}
/*任务4：彩色图片的均衡化*/
void RmwHistogramEqualizeGry(BYTE* pGryImg, int width, int height)
{
	BYTE* pCur, * pEnd = pGryImg + width * height;
	int histogram[256], A[256], LUT[256], g;

	// step.1-------------求直方图--------------------------//
	memset(histogram, 0, sizeof(int) * 256);
	for (pCur = pGryImg; pCur < pEnd;) histogram[*(pCur++)]++;
	// step.2-------------求A[g],N-------------------------//
	for (g = 1, A[0] = histogram[0]; g < 256; g++)
	{
		A[g] = A[g - 1] + histogram[g];
	}
	// step.3-------------求LUT[g]-------------------------//
	for (g = 0; g < 256; g++) LUT[g] = 255 * A[g] / (width * height);
	// step.4-------------查表------------------------------//
	for (pCur = pGryImg; pCur < pEnd;) *(pCur++) = LUT[*pCur];
	// step.5-------------结束------------------------------//
	return;
}
void RmwHistogramEqualizeColor(BYTE* pImg, int width, int height) {
	BYTE* pCur, * pEnd = pImg + width * height*3;
	int histogram[256][3], A[256][3], LUT[256][3], g;
	//RGB三通道
	memset(histogram, sizeof(histogram), 0);
	//1:求直方图
	for (pCur = pImg; pCur < pEnd; pCur++) {
		int r = *pCur; histogram[*(pCur++)][0]++;
		int g = *(++pCur); histogram[*(pCur++)][1]++;
		int b = *(++pCur); histogram[*(pCur++)][2]++;
	}
	//2:求A[g]
	for (g = 1, A[0][0] = histogram[0][0], A[0][1] = histogram[0][1], A[0][2] = histogram[0][2]; g < 256; g++)
	{
		A[g][0] = A[g - 1] [0] + histogram[g][0];//R
		A[g][1] = A[g - 1][1] + histogram[g][1];//G
		A[g][2] = A[g - 1][2] + histogram[g][2];//B
	}
	//3:求LUT[g]
	for (g = 0; g < 256; g++) {
		LUT[g][0] = 255 * A[g][0] / (width * height);
		LUT[g][1] = 255 * A[g][1] / (width * height);
		LUT[g][2] = 255 * A[g][2] / (width * height);
	}
	//4:查表
	for (pCur = pImg; pCur < pEnd;) {
		*(pCur) = LUT[*pCur][0];
		pCur++;
		*(pCur) = LUT[*pCur][1];
		pCur++;
		*(pCur) = LUT[*pCur][2];
		pCur++;
	}
	return;
}
//注意Exam4需要修改，因为原来的函数RmwHistogramEqualizeColor得到的结果和PS有出入，需要改成RmwHistogramEqualizeColor_Re
//第二次的想法
void RmwHistogramEqualizeColor_Re(BYTE* pImg, int width, int height)
{
	BYTE* pCur, * pEnd = pImg + width * height * 3;
	int LUT[256], A[256], g, histogram[256];
	memset(histogram, sizeof(histogram), 0);
	//求直方图
	for (pCur = pImg; pCur < pEnd;) {
		histogram[*(pCur++)]++;
	}
	for (int i = 0; i < 256; i++) {
		histogram[i] /= 3;//计算平均直方图
	}
	//LUT更新
	A[0] = histogram[0];
	LUT[0] = 255 * A[0] / (width * height);
	for (g = 1; g< 256; g++) {
		A[g] = A[g - 1] + histogram[g];
		LUT[g] = 255 * A[g] / (width * height);
	}
	//查表
	for (pCur = pImg; pCur < pEnd;pCur++) {
		*pCur = LUT[*pCur];
	}
	return;
}
void Exam2_4() {
	unsigned char* pRgbImg;
	unsigned char* pRgbImg_copy;
	int width, height;
	pRgbImg = Rmw_Read24BitBmpFile2Img("./data/H0201Rgb.bmp", &width, &height);
	pRgbImg_copy = pRgbImg;
	if (!pRgbImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	//彩色图均衡化
	RmwHistogramEqualizeColor(pRgbImg, width, height);
	//写回结果
	Rmw_Write24BitImg2BmpFile(pRgbImg, width, height, "./res_data/H0201Gry_res.bmp");

}
int main() {

	//Exam2_1();
	//Exam2_2();
	//Exam2_3();
	Exam2_4();


	return 0;
}
