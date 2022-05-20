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
#include "bmpFile.h"

#include "RmwLocateMark.h"
 

void RmwShrinkGryImgByExtract(BYTE *pGryImg, int width, int height, int shrinkX, int shrinkY, BYTE *pResImg)
{   //图像抽样缩小
	int x, y, offy, w, h;
	BYTE *pOrg, *pRes;

	w = width/shrinkX;
	h = height/shrinkY;
	offy = shrinkY*width;
	for (y = 0, pRes = pResImg; y<h; y++)
	{
		pOrg = pGryImg+y*offy;
		for (x = 0; x<w; x++, pRes++)
		{
			*pRes = *(pOrg+x*shrinkX);
		}
	}
	return;
}

void RmwDoSumGryImgByCol( BYTE *pGryImg,  //原始灰度图像
	                      int width, //图像的宽度 
	                      int height,//图像的高度
	                      int *pSumImg //计算得到的积分图
                        )
{   //灰度图像积分图
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
			sumCol[x] += *(pGry++); //更新列和
			*(pRes++) = *(pRes-1)+sumCol[x];
		}
	}
	return;
}

void RmwRobertsGryImg(BYTE *pGryImg, int width, int height, BYTE *pRbtImg)
{   //Robert算子
	BYTE *pGry, *pRbt;
	int dx, dy;
	int x, y;

	for (y = 0, pGry = pGryImg, pRbt = pRbtImg; y<height-1; y++)
	{
		for (x = 0; x<width-1; x++, pGry++)
		{
			dx = *pGry-*(pGry+width+1);
			dy = *(pGry+1)-*(pGry+width);
			*(pRbt++) = max(abs(dx), abs(dy));
		}
		*(pRbt++) = 0; //尾列不做,边缘强度赋0
		pGry++;
	}
	memset(pRbt, 0, width); //尾行不做,边缘强度赋0
	return;
}

void RmwSobelGryImg(BYTE *pGryImg, int width, int height, BYTE *pSbImg)
{   //Sobel算子
	BYTE *pGry, *pSb;
	int dx, dy;
	int x, y;

	memset(pSbImg, 0, width); //首行不做,边缘强度赋0
	for (y = 1, pGry = pGryImg+width, pSb = pSbImg+width; y<height-1; y++)
	{
		*(pSb++) = 0;  //首列不做,边缘强度赋0
		pGry++;
		for (x = 1; x<width-1; x++,pGry++)
		{
			//求dx
			dx = *(pGry-1-width)+(*(pGry-1)*2)+*(pGry-1+width);
			dx -= *(pGry+1-width)+(*(pGry+1)*2)+*(pGry+1+width);
			//求dy
			dy = *(pGry-width-1)+(*(pGry-width)*2)+*(pGry-width+1);
			dy -= *(pGry+width-1)+(*(pGry+width)*2)+*(pGry+width+1);
			//结果
			*(pSb++) = min(255, abs(dx)+abs(dy));
		}
		*(pSb++) = 0; //尾列不做,边缘强度赋0
		pGry++;
	}
	memset(pSb, 0, width); //尾行不做,边缘强度赋0
	return;
}

void RmwSearchMaxRect( int *pSumImg, int width, int height,
	                   int w, int h,
	                   int delL, //左侧比较范围,可以为0
	                   int delR, //右侧比较范围,可以为0
	                   int delU, //上侧比较范围,可以为0
	                   int delD, //下侧比较范围,可以为0
	                   int stepxy, //搜索步长1~4,加速
	                   int *x1, int *x2, int *y1, int *y2
                     )
{   //最大显著能量块搜索
	int *pSumRow, *pSumCol, *pTmp;
	int sumC, sumL, sumR, sumU, sumD;
	int maxV, bstX, bstY;
	int x, y;

	maxV = 0;
	bstX = bstY = 0;
	sumU = sumD = sumL = sumR =0;
	stepxy = max(1, min(4,stepxy));
	for (y = delU+1, pSumRow = pSumImg+y*width; y<height-h-delD-1; y += stepxy, pSumRow += width*stepxy)
	{   
		for (x = delL+1, pSumCol = pSumRow+x; x<width-w-delR-1; x += stepxy, pSumCol += stepxy)
		{
			//中心
			sumC = *(pSumCol+h*width+w)-*(pSumCol+h*width)-*(pSumCol+w)+*pSumCol;
			//上面
			if (delU)
			{
				pTmp = pSumCol-delU*width;
				sumU = *(pTmp+delU*width+w)-*(pTmp+delU*width)-*(pTmp+w)+*pTmp;
			}
			//下面
			if (delD)
			{
				pTmp = pSumCol+h*width;
				sumD = *(pTmp+delD*width+w)-*(pTmp+delD*width)-*(pTmp+w)+*pTmp;
			}
			//左侧
			if (delL)
			{
				pTmp = pSumCol-delL;
				sumL = *(pTmp+h*width+delL)-*(pTmp+h*width)-*(pTmp+delL)+*pTmp;
			}
			//右侧
			if (delR)
			{
				pTmp = pSumCol+w;
				sumR = *(pTmp+h*width+delR)-*(pTmp+h*width)-*(pTmp+delR)+*pTmp;
			}
 			//评价准则
			if (sumC-sumL-sumR-sumU-sumD>maxV)
			{
				maxV = sumC-sumL-sumR-sumU-sumD;
				bstX = x;
				bstY = y;
			}
		}
	}
	*x1 = bstX;
	*x2 = bstX+w-1;
	*y1 = bstY;
	*y2 = bstY+h-1;
	return;
}

void RmwCopyGryRect(BYTE *pGryImg, int width, int height, int x1, int x2, int y1, int y2, BYTE *pRectImg)
{   //图像块复制
	BYTE *pRow, *pRect;
	int y, w;

	w = x2-x1+1;
	pRect = pRectImg;
	for (y = y1, pRow = pGryImg+y*width+x1; y<=y2; y++, pRow += width)
	{
		memcpy(pRect, pRow, w);
		pRect += w;
	}
	return;
}

void RmwDrawGryRect(BYTE *pGryImg, int width, int height, int x1, int x2, int y1, int y2, int color)
{   //在灰度图像中画矩形
	int y, i;
	BYTE *p1, *p2;

	x1 = max(0, min(x1, width-1));
	x2 = max(0, min(x2, width-1));
	y1 = max(0, min(y1, height-1));
	y2 = max(0, min(y2, height-1));
	 
	if (x1>x2) { i = x2; x2 = x1; x1 = i; }
	if (y1>y2) { i = y2; y2 = y1; y1 = i; }

	memset(pGryImg+y1*width+x1, color, x2-x1+1);
	memset(pGryImg+y2*width+x1, color, x2-x1+1);

	for (y = y1, p1 = pGryImg+y1*width+x1, p2 = pGryImg+y1*width+x2; y<=y2; y++, p1 += width, p2 += width)
	{
		*(p1) = color;
		*(p2) = color;
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////
//
// Construction/Destruction
//
///////////////////////////////////////////////////////////////////////////////////
RmwLocateMark::RmwLocateMark()
{
	m_isInitdOK = false;
	//内存
	m_pGryImg=NULL;
	m_pGrdImg=NULL;
	m_pSumImg=NULL;
	m_memSize=0;
}

RmwLocateMark::~RmwLocateMark()
{
	Dump();
}

void RmwLocateMark::Dump()
{
	if (m_pGryImg)
	{
		delete m_pGryImg;
		m_pGryImg = NULL;
	}
	if (m_pGrdImg)
	{
		delete m_pGrdImg;
		m_pGrdImg = NULL;
	}
	if (m_pSumImg)
	{
		delete m_pSumImg;
		m_pSumImg = NULL;
	}
	m_memSize = 0;
}
///////////////////////////////////////////////////////////////////////////////////
//
// 初始化
//
///////////////////////////////////////////////////////////////////////////////////
bool RmwLocateMark::Initialize( int maxWidth, //支持的最大图像宽度
	                            int maxHeight, //支持的最大图像高度
	                            bool invertGry, //灰度反相
	                            int mode12, //计算模式,1-使用灰度,2-使用梯度
	                            int nFast0123, //加速,0-不加速,1-加速4倍,2-加速16倍(目标很大时)
	                            bool doDebug //是否调试
                              )
{
	// step.0----初始化-----------------------------//
	m_isInitdOK = false;
	m_doDebug= doDebug; //是否调试
	// step.1----参数保存---------------------------//
	m_invertGry= invertGry;
	m_mode12=mode12;
	m_nFast0123= min(max(0,nFast0123),3);
	m_nShrink = (int)(pow(2, m_nFast0123)+0.5);
	//缩小后的尺寸
	m_maxWidth = maxWidth/m_nShrink;
	m_maxHeight = maxHeight/m_nShrink;
	// step.2----内存-------------------------------//
	if (m_maxWidth*m_maxHeight>m_memSize)
	{
		//先释放
		Dump();
		//后申请
		m_pGryImg = new BYTE[m_maxWidth*m_maxHeight];
		m_pGrdImg = new BYTE[m_maxWidth*m_maxHeight];
		m_pSumImg = new int[m_maxWidth*m_maxHeight];
		m_memSize = m_maxWidth*m_maxHeight;
	}
	// step.3----返回-------------------------------//
	m_isInitdOK = m_pGryImg && m_pGrdImg && m_pSumImg;
	//返回
	return m_isInitdOK;
}
///////////////////////////////////////////////////////////////////////////////////
//
// 执行
//
///////////////////////////////////////////////////////////////////////////////////
bool RmwLocateMark::DoNext( BYTE *pGryImg, //当前图像
	                        int width, //当前图像的宽度  
	                        int height, //当前图像的高度
	                        int targetW, //目标宽度
	                        int targetH, //目标高度
	                        int delLeftW, //去掉左边的宽度,可以为0
	                        int delRightW, //去掉右边的宽度,可以为0
	                        int delTopH, //去掉上边的的高度,可以为0
	                        int delBottomH, //去掉上边的的高度,可以为0
	                        int frameID //帧号
                         )
{   //返回:是否正确执行

	// step.0----安全与参数-------------------------//
	m_orgWidth = width;
	m_orgHeight = height;
	//当前图像参数
	m_width=width/m_nShrink;
	m_height=height/m_nShrink;
	m_targetW=targetW/m_nShrink;
	m_targetH=targetH/m_nShrink;
	m_delLeftW= delLeftW/m_nShrink; 
	m_delRightW=delRightW/m_nShrink;
	m_delTopH= delTopH/m_nShrink;
	m_delBottomH= delBottomH/m_nShrink;
	m_frameID= frameID; //帧号
	//虚指针
	m_pOrgImg = pGryImg;
	//安全检查
	if (!m_isInitdOK) return false;
	if (m_width>m_maxWidth) return false;
	if (m_height>m_maxHeight) return false;
	// step.1----图像缩小------------------------//
	RmwShrinkGryImgByExtract( pGryImg, width, height,
		                      m_nShrink,
		                      m_nShrink,
		                      m_pGryImg
	                        );
	// step.2----计算积分图像--------------------//
	if (m_mode12==1)
	{
		RmwDoSumGryImgByCol(m_pGryImg, m_width, m_height, m_pSumImg);
	}
	else if (m_mode12==2)
	{
		//RmwSobelGryImg(m_pGryImg, m_width, m_height, m_pGrdImg);
		RmwRobertsGryImg(m_pGryImg, m_width, m_height, m_pGrdImg);
		RmwDoSumGryImgByCol(m_pGrdImg, m_width, m_height, m_pSumImg);
	}
	// step.3----位置搜索------------------------//
	RmwSearchMaxRect( m_pSumImg, m_width, m_height,
		              m_targetW, m_targetH,
		              m_delLeftW, //左侧比较范围,可以为0
		              m_delRightW, //右侧比较范围,可以为0
		              m_delTopH, //上侧比较范围,可以为0
		              m_delBottomH, //下侧比较范围,可以为0
		              2,//1 //搜索步长1~4,加速
		              &m_x1,&m_x2,&m_y1,&m_y2
	                );	
	// step.4----调试与返回----------------------//
	if (m_doDebug)
	{
		Debug();
	}
	return true;
}

void RmwLocateMark::GetOrgPos(int *x1, int *x2, int *y1, int *y2)
{   //取结果
	*x1 = m_x1*m_nShrink;
	*x2 = m_x2*m_nShrink;
	*y1 = m_y1*m_nShrink;
	*y2 = m_y2*m_nShrink;
	return;
}

BYTE *RmwLocateMark::GetTargetImg(int extWH, int *w, int *h, RMWRECT *refPos)
{   //取结果图像
	int x1, x2, y1, y2;
	int j1, j2, i1, i2;

	// step.1----取原始位置----------------------//
	GetOrgPos(&x1, &x2, &y1, &y2);
	j1 = x1;
	j2 = x2;
	i1 = y1;
	i2 = y2;
	// step.2----扩展---------------------------//
	x1 = max(0, x1-extWH);
	x2 = min(m_orgWidth-1, x2+extWH);
	y1 = max(0, y1-extWH);
	y2 = min(m_orgHeight-1, y2+extWH);
	// step.3----复制---------------------------//
	RmwCopyGryRect(m_pOrgImg, m_orgWidth, m_orgHeight, x1, x2, y1, y2, (BYTE *)m_pSumImg);
	// step.4----返回---------------------------//
	*w = x2-x1+1;
	*h = y2-y1+1;
	//最佳位置
	refPos->left = j1-x1;
	refPos->right = refPos->left+(j2-j1);
	refPos->top = i1-y1;
	refPos->bottom = refPos->top+(i2-i1);
	//返回
	return (BYTE *)m_pSumImg;
}
///////////////////////////////////////////////////////////////////////////////////
//
// 调试
//
///////////////////////////////////////////////////////////////////////////////////
void RmwLocateMark::Debug()
{  
	BYTE *pDbgImg;
	int i;

	// step.1----内存申请------------------------//
	pDbgImg = new BYTE[m_width*m_height];
	// step.2----图像复制------------------------//
	if (m_mode12==1)
	{
		memcpy(pDbgImg, m_pGryImg, m_width*m_height);
	}
	else
	{
		memcpy(pDbgImg, m_pGrdImg, m_width*m_height);
	}
	for (i = 0; i<m_width*m_height; i++)
	{
		pDbgImg[i] = min(250, pDbgImg[i]);
	}
	// step.3----画目标位置----------------------//
	RmwDrawGryRect(pDbgImg, m_width, m_height, m_x1, m_x2, m_y1, m_y2, 255);
	// step.3----结果----------------------------//
	sprintf(m_filename, "d:\\tmp\\frameID=%05d_nFast=%d.bmp", m_frameID, m_nFast0123);
	Rmw_Write8BitImg2BmpFileMark(pDbgImg, m_width, m_height, m_filename);

	return;
}
