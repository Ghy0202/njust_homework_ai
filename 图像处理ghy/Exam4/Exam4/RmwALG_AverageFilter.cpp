///////////////////////////////////////////////////////////////////////////////////////////
//
//   第三章
//
///////////////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <nmmintrin.h>

///////////////////////////////////////////////////////////////////////////////////////
//
//基于列积分的快速均值滤波
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwAvrFilterBySumCol( BYTE *pGryImg,  //原始灰度图像
	                       int width, int height,//图像的宽度和高度
	                       int M, int N, //滤波邻域：M列N行
	                       BYTE *pResImg //结果图像
                         )
{   //没有对边界上邻域不完整的像素进行处理，可以采用变窗口的策略
	BYTE *pAdd, *pDel, *pRes;
	int halfx, halfy;
	int x, y;
	int sum,c;
	int sumCol[4096]; //约定图像宽度不大于4096

	// step.1------------初始化--------------------------//
	M = M/2*2+1; //奇数化
	N= N/2*2+1; //奇数化
	halfx = M/2; //滤波器的半径x
	halfy = N/2; //滤波器的半径y
	c = (1<<23)/(M*N); //乘法因子
	memset(sumCol, 0, sizeof(int)*width);
	for (y = 0, pAdd = pGryImg; y<N; y++)
	{
		for (x = 0; x<width; x++) sumCol[x] += *(pAdd++);
	}
	// step.2------------滤波----------------------------//
	for (y = halfy, pRes = pResImg+y*width,pDel=pGryImg; y<height-halfy; y++)
	{
		//初值
		for (sum=0,x = 0; x<M; x++) sum += sumCol[x];
		//滤波
		pRes += halfx; //跳过左侧
		for (x = halfx; x<width-halfx; x++)
		{
			//求灰度均值
			//*(pRes++)=sum/(N*M);
			*(pRes++) = (sum*c)>>23; //用整数乘法和移位代替除法
			//换列,更新灰度和
			sum -= sumCol[x-halfx]; //减左边列
			sum += sumCol[x+halfx+1]; //加右边列
		}
		pRes += halfx;//跳过右侧
		//换行,更新sumCol
		for (x = 0; x<width; x++)
		{
			sumCol[x] -= *(pDel++); //减上一行
			sumCol[x] += *(pAdd++); //加下一行
		}
	}
	// step.3------------返回----------------------------//
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//基于列积分的积分图实现
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwDoSumGryImg( BYTE *pGryImg,  //原始灰度图像
	                 int width, //图像的宽度 
	                 int height,//图像的高度
	                 int *pSumImg //计算得到的积分图
                   )
{
	BYTE *pGry;
	int *pRes;
	int x, y;
	int sumCol[4096]; //约定图像宽度不大于4096

	memset(sumCol, 0, sizeof(int)*width);
	for (y = 0, pGry = pGryImg, pRes=pSumImg; y<height; y++)
	{
		//最左侧像素的特别处理
		sumCol[0] += *(pGry++);
		*(pRes++) = sumCol[0];
		//正常处理
		for (x = 1; x<width; x++)
		{
			sumCol[x] += *(pGry++); //更新列积分
			*(pRes++) = *(pRes-1)+sumCol[x];
		}
	}
	return;
}

void RmwDoSumRGBImg( BYTE *pRGBImg,  //原始灰度图像
	                 int width, //图像的宽度 
	                 int height,//图像的高度
	                 int *pSumImg //计算得到的积分图
                   )
{
	BYTE *pRGB;
	int *pRes;
	int x, y;
	int sumCol[4096*3]; //约定图像宽度不大于4096

	memset(sumCol, 0, sizeof(int)*width*3);
	for (y = 0, pRGB = pRGBImg, pRes=pSumImg; y<height; y++)
	{
		//最左侧像素的特别处理
		sumCol[0] += *(pRGB++);//blue
		sumCol[1] += *(pRGB++);
		sumCol[2] += *(pRGB++);
		*(pRes++) = sumCol[0]; //blue
		*(pRes++) = sumCol[1];
		*(pRes++) = sumCol[2];
		//正常处理
		for (x = 3; x<width*3; x++)
		{
			//更新列积分
			sumCol[x] += *(pRGB++);
			//更新积分图
			*(pRes++) = *(pRes-3)+sumCol[x];
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//基于SSE的积分图实现
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwDoSumGryImg_SSE( BYTE *pGryImg,  //原始灰度图像
	                     int width, //图像的宽度,必须是4的倍数
	                     int height,//图像的高度
	                     int *pSumImg //计算得到的积分图
                       )
{   _declspec(align(16)) int sumCol[4096]; //约定图像宽度不大于4096,16字节对齐
	__m128i *pSumSSE,A;
	BYTE *pGry;
	int *pRes;
	int x, y;

	memset(sumCol, 0, sizeof(int)*width);
	for (y = 0, pGry = pGryImg, pRes=pSumImg; y<height; y++)
	{
		//0:需要特别处理
		sumCol[0] += *(pGry++);
		*(pRes++) = sumCol[0];
		//1
		sumCol[1] += *(pGry++);
		*(pRes++) = *(pRes-1)+sumCol[1];
		//2
		sumCol[2] += *(pGry++);
		*(pRes++) = *(pRes-1)+sumCol[2];
		//3
		sumCol[3] += *(pGry++);
		*(pRes++) = *(pRes-1)+sumCol[3];
		//[4...width-1]
		for (x = 4, pSumSSE = (__m128i *)(sumCol+4); x<width; x+=4, pGry+=4)
		{
			//把变量的低32位(有4个8位整数组成)转换成32位的整数
			A = _mm_cvtepu8_epi32(_mm_loadl_epi64((__m128i*)pGry));
			//4个32位的整数相加 
			*(pSumSSE++) = _mm_add_epi32(*pSumSSE, A);
			//递推
			*(pRes++) = *(pRes-1)+sumCol[x+0];
			*(pRes++) = *(pRes-1)+sumCol[x+1];
			*(pRes++) = *(pRes-1)+sumCol[x+2];
			*(pRes++) = *(pRes-1)+sumCol[x+3];
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//基于积分图的快速均值滤波
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwAvrFilterBySumImg( int *pSumImg, //计算得到的积分图
	                       int width, int height,//图像的宽度和高度
	                       int M, int N, //滤波邻域：M列N行
	                       BYTE *pResImg //结果图像
                         )
{   //没有对边界上邻域不完整的像素进行处理，可以采用变窗口的策略
	int *pY1, *pY2;
	BYTE *pRes;
	int halfx, halfy;
	int y, x1, x2;
	int sum, c;

	// step.1------------初始化--------------------------//
	M = M/2*2+1; //奇数化
	N = N/2*2+1; //奇数化
	halfx = M/2; //滤波器的半径x
	halfy = N/2; //滤波器的半径y
	c = (1<<23)/(M*N); //乘法因子
 	// step.2------------滤波----------------------------//
	for ( y = halfy+1, pRes = pResImg+y*width, pY1 = pSumImg,pY2=pSumImg+N*width; 
		  y<height-halfy; 
		  y++,pY1+=width,pY2+=width
		)
	{
 		pRes += halfx+1; //跳过左侧
		//for (x = halfx+1,x1=0,x2=M; x<width-halfx; x++,x1++,x2++)
		for (x1=0,x2=M; x2<width; x1++,x2++) //可以简化如此，但不太容易读
		{
			sum = *(pY2+x2)-*(pY2+x1)-*(pY1+x2)+*(pY1+x1);
			*(pRes++) = (sum*c)>>23; //用整数乘法和移位代替除法
 		}
		pRes += halfx;//跳过右侧
	}
	// step.3------------返回----------------------------//
	return;
}
