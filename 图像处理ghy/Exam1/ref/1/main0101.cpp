#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "bmpFile.h"
#include "RmwALG_InvertImg.h"

void testInvertGryImg()
{
	unsigned char *pGryImg;
	int width, height;

	// step.1-------------读图像文件------------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("..\\RmwCPP0101\\H0101Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	// step.2-------------图像处理--------------------------//
	RmwInvertGryImg7(pGryImg, width, height);
	// step.3-------------保存图像--------------------------//
	Rmw_Write8BitImg2BmpFile(pGryImg, width, height, "..\\RmwCPP0101\\H0101Gry_invert.bmp");
	Rmw_Write8BitImg2BmpFileRed(pGryImg, width, height, "..\\RmwCPP0101\\H0101Gry_invert_PseudoColor.bmp");
	// step.4-------------结束------------------------------//
	delete pGryImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}

void testInvertRgbImg()
{
	unsigned char *pRgbImg;
	int width, height;

	// step.1-------------读图像文件------------------------//
	pRgbImg = Rmw_Read24BitBmpFile2Img("..\\RmwCPP0101\\H0102Rgb.bmp", &width, &height);
	if (!pRgbImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	// step.2-------------图像处理--------------------------//
	RmwInvertRgbImg(pRgbImg, width, height);
	// step.3-------------保存图像--------------------------//
	Rmw_Write24BitImg2BmpFile(pRgbImg, width, height, "..\\RmwCPP0101\\H0102Rgb_invert1.bmp");
	// step.4-------------结束------------------------------//
	delete pRgbImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
 

int main()
{
	testInvertGryImg();
	testInvertRgbImg();
}