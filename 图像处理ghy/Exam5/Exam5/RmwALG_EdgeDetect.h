#ifndef _RMW_ALG_EDGE_DETECT_H
#define _RMW_ALG_EDGE_DETECT_H

///////////////////////////////////////////////////////////////////////////////////////////
//
//   第四章
//
///////////////////////////////////////////////////////////////////////////////////////////
//Gradient算子
void RmwGradientGryImg(BYTE *pGryImg, int width, int height, BYTE *pGrdImg);
//Robert算子
void RmwRobertsGryImg(BYTE *pGryImg, int width, int height, BYTE *pRbtImg);
//Sobel算子
void RmwSobelGryImg(BYTE *pGryImg, int width, int height, BYTE *pSbImg);
//Prewitt算子
void RmwPrewittGryImg(BYTE *pGryImg, int width, int height, BYTE *pPRTImg);
//沈俊算子
void RmwShenJunGryImg(BYTE *pGryImg, BYTE *pTmpImg, int width, int height, double a0, BYTE *pSJImg);
///////////////////////////////////////////////////////////////////////////////////////////
//
//应用示例：米粒边缘检测
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwExtractRiceEdge( BYTE *pGryImg, //原始灰度图像 
	                     BYTE *pTmpImg, //辅助图像
	                     int width,
	                     int height,
	                     double a0, 
	                     int grdThre,
	                     BYTE *pEdgeImg
                       );
#endif

