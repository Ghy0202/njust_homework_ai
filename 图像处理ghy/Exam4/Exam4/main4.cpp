#include<stdio.h>
#include<iostream>
#include<Windows.h>
#include<time.h>
#include<math.h>
#include<vector>
#include <nmmintrin.h>
#include"bmpFile.h"
#include"RmwALG_AverageFilter.h"
#include"RmwALG_MedianFilter.h"
using namespace std;
/*对图像进心不同大小的中值滤波，并比较时间消耗*/
void Exam1() {
	//导入图片
	unsigned char* pGryImg,*pResImg1,*pResImg2,*pResImg3;
	int width, height;
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0302Gry.bmp", &width, &height);
	pResImg1= Rmw_Read8BitBmpFile2Img("./data/H0302Gry.bmp", &width, &height);
	pResImg2 = Rmw_Read8BitBmpFile2Img("./data/H0302Gry.bmp", &width, &height);
	pResImg3 = Rmw_Read8BitBmpFile2Img("./data/H0302Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	//不同大小的中值滤波
	cout<<RmwMedianFilter(pGryImg,width,height,5,5,pResImg1)<<endl;
	cout<<RmwMedianFilter(pGryImg, width, height, 13, 13, pResImg2)<<endl;
	cout<<RmwMedianFilter(pGryImg, width, height, 21, 21, pResImg3)<<endl;
	//保存结果
	Rmw_Write8BitImg2BmpFile(pResImg1, width, height, "./res_data/H0302_5.bmp");
	Rmw_Write8BitImg2BmpFile(pResImg2, width, height, "./res_data/H0302_13.bmp");
	Rmw_Write8BitImg2BmpFile(pResImg3, width, height, "./res_data/H03025_21.bmp");
	printf("Exam1 success!\n");
}

void GauseRow(BYTE* pGryImg, int width, int height, BYTE* pResImg, int* shell, int size, int data) {
	BYTE* pCur = pGryImg;
	BYTE* pRes = pResImg;
	//初始化掩膜
	shell = new int[13];
	float M = 1.0 / (sqrt(2 * 3.14 * data * data));
	float c = 1.0 / (2.0 * data * data);
	int temp;
	for (int p = 0; p < size; p++) {
		temp = (exp(-p * p * c) * M) * (1 << 10);
		shell[p] = shell[2 * size - 2 - p] = temp;
	}
	//高斯过程
	for (int y = 0; y < height; y++) {
		for (pCur = pGryImg + y * width; pCur < pGryImg + (y + 1) * width; pCur++) {
			*pRes = *pCur * shell[0] >> 10;
			for (int i = 1; i < 13; i++) {
				*pRes += *(pCur + i) * shell[i] >> 10;
			}
			pRes++;
		}
	}
}
void Exam2() {
	unsigned char* pGryImg;
	int width, height;
	pGryImg = Rmw_Read8BitBmpFile2Img("./data/H0303Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	BYTE* pResImg = new BYTE[width * height];
	BYTE* pResImg2 = new BYTE[width * height];
	BYTE* pResImg3 = new BYTE[width * height];
	int* shell=new int[3];
	//行做高斯
	GauseRow(pGryImg, width, height, pResImg,shell,7,3);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "./res_data/H0303_guase_row.bmp");
	//图片旋转
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pResImg2[x * height + y] = pResImg[y * width + x];
		}
	}
	//再次高斯
	GauseRow(pResImg2, width, height, pResImg3, shell, 7, 3);
	//把图片旋转回来
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pResImg2[x * height + y] = pResImg3[y * width + x];
		}
	}
	Rmw_Write8BitImg2BmpFile(pResImg2, width, height, "./res_data/H0303_guase.bmp");
}
int main() {
	//Exam1();
	Exam2();



	return 0;
}