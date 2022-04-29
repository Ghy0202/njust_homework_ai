#ifndef _RMW_ALG_HISTOGRAM_H
#define _RMW_ALG_HISTOGRAM_H

///////////////////////////////////////////////////////////////////////////////////////
//
//直方图及其计算
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwGetHistogram(BYTE *pGryImg, int width, int height, int *histogram);
void GetMinMaxGry(int *histogram, int *minGry, int *maxGry);
void GetBrightContrast(int *histogram, double *bright, double *contrast);
///////////////////////////////////////////////////////////////////////////////////////
//
//直方图均衡化
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwHistogramEqualizeDemo(BYTE *pGryImg, int width, int height);
void RmwHistogramEqualize(BYTE *pGryImg, int width, int height);
///////////////////////////////////////////////////////////////////////////////////////
//
//对数变换
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwLogTransform(BYTE *pGryImg, int width, int height);
///////////////////////////////////////////////////////////////////////////////////////
//
//逐点直方图均衡化
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwHistogramEqualizePixel(BYTE *pGryImg, int width, int height, int N, int M, BYTE *pResImg);
///////////////////////////////////////////////////////////////////////////////////////
//
//14Bit直方图均衡化
//
///////////////////////////////////////////////////////////////////////////////////////
int RmwRead14BitImgFile(short int *p14Img, int width, int height, char *fileName);
void RmwHistogramEqualize(short int *pGry14BitImg, int width, int height, BYTE *pResImg);
 
#endif

