///////////////////////////////////////////////////////////////////////////////////////////
//
//   第四章
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

///////////////////////////////////////////////////////////////////////////////////////////
//
//边缘锐化
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwEdgeShapen( BYTE *pGryImg, int width, int height, 
	                double sharpenFactor, //锐化倍数
	                BYTE *pResImg
                  )
{   
	BYTE *pCur, *pRes;
	int x, y, delta, res,c;
	
	c = (int)(sharpenFactor*4096/8); //放大4096倍,变为乘法+移位
	memcpy(pResImg, pGryImg, width);
	for (y = 1, pCur = pGryImg+width,pRes= pResImg+width; y<height-1; y++)
	{
		*(pRes++) = *(pCur++);
		for (x = 1; x<width-1; x++, pCur++, pRes++)
		{
			delta = (*pCur)*8-*(pCur+1)-*(pCur-1)- //应该除以8的,在c中除过了
				    *(pCur+width+1)-*(pCur+width)-*(pCur+width-1)-
				    *(pCur-width+1)-*(pCur-width)-*(pCur-width-1);
#if 1
			res = (*pCur)+((delta *c)>>12);
			*pRes = min(255, max(0, res));
#else 
			delta /= 8;
			delta *= sharpenFactor;
			res = (*pCur)+delta;
			*pRes = min(255, max(0, res));
#endif
		}
		*(pRes++) = *(pCur++);
	}
	memcpy(pRes, pCur, width);
	return;
}
