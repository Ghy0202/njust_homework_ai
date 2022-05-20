// RmwLocateMark.h: interface for the RmwLocateMark class.
//
//////////////////////////////////////////////////////////////////////
#ifndef RMW_LOCATE_MARK_H
#define RMW_LOCATE_MARK_H

//矩形定义
typedef struct tagRMWRECT
{
	int left;
	int right;
	int top;
	int bottom;
}RMWRECT;

class RmwLocateMark
{
public:
	RmwLocateMark();
	virtual ~RmwLocateMark();
	//初始化
    bool Initialize( int maxWidth, //支持的最大图像宽度
		             int maxHeight, //支持的最大图像高度
		             bool invertGry, //灰度反相
		             int mode12, //计算模式,1-使用灰度,2-使用梯度
		             int nFast0123, //加速,0-不加速,1-加速4倍,2-加速16倍(目标很大时)
		             bool doDebug //是否调试
		           );
	//执行
	bool DoNext( BYTE *pGryImg, //当前图像
		         int width, //当前图像的宽度  
		         int height, //当前图像的高度
		         int targetW, //目标宽度
		         int targetH, //目标高度
		         int delLeftW, //去掉左边的宽度,可以为0
		         int delRightW, //去掉右边的宽度,可以为0
		         int delTopH, //去掉上边的的高度,可以为0
		         int delBottomH, //去掉上边的的高度,可以为0
		         int frameID //帧号
	           );
	//取在原始图像中的位置
	void GetOrgPos(int *x1,int *x2,int *y1,int *y2); 
	//取结果图像
	BYTE *GetTargetImg(int extWH,int *w, int *h,RMWRECT *refPos);
private:
	void Dump();
	//调试
	void Debug();
private:
	bool m_isInitdOK;
	//参数
	int m_maxWidth; //支持的最大图像宽度
	int m_maxHeight; //支持的最大图像高度
	bool m_invertGry; //灰度反相
	int m_mode12; //计算模式,1-使用灰度,2-使用梯度,3-使用灰度+梯度
	int m_nFast0123; //加速,0-不加速,1-加速4倍,2-加速16倍(目标很大时)
	int m_nShrink;
	bool m_doDebug; //是否调试
	//内存
	BYTE *m_pOrgImg; //虚指针
	BYTE *m_pGryImg;
	BYTE *m_pGrdImg;
	int *m_pSumImg;
	int m_memSize;
	//当前图像参数
	int m_orgWidth;
	int m_orgHeight;
	int m_width; //当前图像的宽度  
	int m_height; //当前图像的高度
	int m_targetW; //目标宽度
	int m_targetH; //目标高度
	int m_delLeftW; //去掉左边的宽度,可以为0
	int m_delRightW; //去掉右边的宽度,可以为0
	int m_delTopH; //去掉上边的的高度,可以为0
	int m_delBottomH; //去掉上边的的高度,可以为0
	int m_frameID; //帧号
	//结果
	int m_x1;
	int m_x2;
	int m_y1;
	int m_y2;
	//调试
	char m_filename[255];
};
 
#endif
