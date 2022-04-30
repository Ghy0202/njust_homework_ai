/*
用C/C++编程实现
一幅灰度图像bmp文件H0101Gry.bmp和彩色图像bmp文件H0102Rgb.bmp
读取、反相以及灰度图像H0101Gry.bmp的伪彩色
将处理结果图像保存成bmp文件.
*/
#include<bits/stdc++.h>
#include <windows.h>
using namespace std;
/**文件读取的函数**/
BYTE *Rmw_Read8BitBmpFile2Img(const char *filename,int *width,int *height)
{   //本函数*width返回的是真实图像宽度(去掉了4对齐的扩展)
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER dataHeader;
	BYTE *pGryImg;
	FILE *fp;
	int w, h, ext, y;
	int begin, end;
	BYTE tmp[4];

	//step.0-----------------初始化-------------------------//
	*width = *height = 0;
	//step.1-----------------打开文件-----------------------//
	if(!(fp=fopen(filename,"rb"))) return NULL;
	//step.2-----------------读取文件描述信息---------------//
	if (fread((void *)&fileHeader, sizeof(BITMAPFILEHEADER),1,fp)!=1)
	{
		fclose(fp);
		return NULL;
	}
	//检查是否是bmp文件
	if (fileHeader.bfType!=((WORD)('M'<<8)|'B'))
	{
		fclose(fp);
		return NULL;
	}
	//step.3-----------------读数据描述信息---------------//
	if (fread((void *)&dataHeader, sizeof(BITMAPINFOHEADER), 1,fp)!=1)
	{
		fclose(fp);
		return NULL;
	}
	//检查是否8Bit的灰度图像
	if (dataHeader.biBitCount!=8)
	{
		fclose(fp);
		return NULL;
	}
	//step.4-----------------读取图像数据-------------------//
 	w=dataHeader.biWidth;
	h=dataHeader.biHeight;
    ext=(w+3)/4*4-w; //因为bmp文件保存的每行字节个数是4的倍数
	//数据尾的位置
	fseek(fp, 0L, SEEK_END);
    end = ftell(fp);
	//定位到数据起始位置
	fseek(fp, fileHeader.bfOffBits, SEEK_SET);
	begin = ftell(fp);
	//验证文件长度
	if (end-begin<(w+ext)*h)
	{
		fclose(fp);
		return NULL;
	}
	//读数据
	if ( (pGryImg=new BYTE[w*h])!=NULL)
	{
		for(y=0;y<h;y++)
		{
			//倒读w个字节
			fread(pGryImg+(h-1-y)*w, sizeof(BYTE), w, fp);
 			//读掉扩充的数据
			fread(tmp, sizeof(BYTE), ext, fp);
		}
	}
	//step.5-----------------返回图像数据-------------------//
	fclose(fp);
	*width = w;
	*height = h;
	return pGryImg;
}
bool Rmw_Write24BitImg2BmpFile(BYTE *pRGBImg, int width, int height, const char *filename)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER dataHeader;
	FILE *fp;
	int ext;
	BYTE p[4], *pCur;

	//step.0-----------------创建文件-----------------------//
	if ((fp = fopen(filename, "w+b"))==NULL) return false;
	//step.1-----------------写文件描述信息-----------------//
	fileHeader.bfType = ((WORD)('M'<<8)|'B');
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); //灰度值起始位置
	fileHeader.bfSize = fileHeader.bfOffBits+width*height+0L; //彩色图像没有调色板
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fwrite((void *)&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	//step.2-----------------写数据描述信息-----------------//
	memset((void *)&dataHeader, 0, sizeof(BITMAPINFOHEADER));
	dataHeader.biSize = 40;
	dataHeader.biWidth = width;
	dataHeader.biHeight = height;
	dataHeader.biPlanes = 1;
	dataHeader.biBitCount = 8*3; //RGB是3个分量
 	fwrite((void *)&dataHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//step.4-----------------写图像数据---------------------//
	ext = ((width*3+3)/4*4)-width*3; //因为bmp文件保存的每行字节个数是4的倍数
	if (ext==0)
	{
		for (pCur = pRGBImg+(height-1)*width*3; pCur>=pRGBImg; pCur -= width*3) //倒写
		{
			fwrite((void *)pCur, sizeof(BYTE), width*3, fp);
		}
	}
	else
	{
		p[0] = p[1] = p[2] = p[3] = 0; //扩充的数据设为0
		for (pCur = pRGBImg+(height-1)*width*3; pCur>=pRGBImg; pCur -= width*3) //倒写
		{
			fwrite((void *)pCur, sizeof(BYTE), width*3, fp); //图像数据
			fwrite((void *)p, sizeof(BYTE), ext, fp); //扩充数据
		}
	}
	//step.5-----------------退出--------------------------//
	fclose(fp); //关闭文件
	return true;
}
BYTE *Rmw_Read24BitBmpFile2Img(const char *filename, int *width, int *height)
{   //本函数*width返回的是真实图像宽度(去掉了4对齐的扩展)
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER dataHeader;
	BYTE *pRGBImg;
	FILE *fp;
	int w, h, ext, y;
	int begin, end;
	BYTE tmp[4];

	//step.0-----------------初始化-------------------------//
	*width = *height = 0;
	//step.1-----------------打开文件-----------------------//
	if (!(fp = fopen(filename, "rb"))) return NULL;
	//step.2-----------------读取文件描述信息---------------//
	if (fread((void *)&fileHeader, sizeof(BITMAPFILEHEADER),1,fp)!=1)
	{
		fclose(fp);
		return NULL;
	}
	//检查是否是bmp文件
	if (fileHeader.bfType!=((WORD)('M'<<8)|'B'))
	{
		fclose(fp);
		return NULL;
	}
	//step.3-----------------读数据描述信息---------------//
	if (fread((void *)&dataHeader, sizeof(BITMAPINFOHEADER), 1,fp)!=1)
	{
		fclose(fp);
		return NULL;
	}
	//检查是否24Bit的灰度图像
	if (dataHeader.biBitCount!=24)
	{
		fclose(fp);
		return NULL;
	}
	//step.4-----------------读取图像数据-------------------//
	w = dataHeader.biWidth;
	h = dataHeader.biHeight;
	ext = ((w*3+3)/4*4)-w*3; //因为bmp文件保存的每行字节个数是4的倍数
	//数据尾的位置
	fseek(fp, 0L, SEEK_END);
	end = ftell(fp);
	//定位到数据起始位置
	fseek(fp, fileHeader.bfOffBits, SEEK_SET);
	begin = ftell(fp);
	//验证文件长度
	if (end-begin<w*h*3+ext)
	{
		fclose(fp);
		return NULL;
	}
	//读数据
	if ((pRGBImg = new BYTE[w*h*3])!=NULL)
	{
		for (y = 0; y<h; y++)
		{
			//倒读w个字节
			fread(pRGBImg+(h-1-y)*w*3, sizeof(BYTE), w*3, fp);
			//读掉扩充的数据
			fread(tmp, sizeof(BYTE), ext, fp);
		}
	}
	//step.5-----------------返回图像数据-------------------//
	fclose(fp);
	*width = w;
	*height = h;
	return pRGBImg;
}
bool Rmw_Write8BitImg2BmpFile(BYTE *pGryImg,int width,int height,const char *filename)
{   
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER dataHeader;
	FILE *fp;
	int i, ext;
	BYTE p[4],*pCur;

	//step.0-----------------创建文件-----------------------//
    if( (fp=fopen(filename,"w+b"))==NULL ) return false; 
	//step.1-----------------写文件描述信息-----------------//
	fileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	fileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4L; //灰度值起始位置,调色板大小为256*4
    fileHeader.bfSize=fileHeader.bfOffBits+width*height ;
    fileHeader.bfReserved1=0;
    fileHeader.bfReserved2=0;
	fwrite((void *)&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	//step.2-----------------写数据描述信息-----------------//
	memset((void *)&dataHeader, 0, sizeof(BITMAPINFOHEADER));
	dataHeader.biSize = 40;
	dataHeader.biWidth = width;
	dataHeader.biHeight = height;
	dataHeader.biPlanes = 1;
	dataHeader.biBitCount = 8;
	fwrite((void *)&dataHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//step.3-----------------写调色板-----------------------//
	p[3] = 0;
	for (i=0;i<256;i++)
    {  
		p[0] = p[1] = p[2] = i; //灰度调色板,blue=green=red
		fwrite((void *)p, sizeof(BYTE), 4, fp);
 	}
	//step.4-----------------写图像数据---------------------//
	ext=(width+3)/4*4-width; //每行字节个数需要按4的倍数对齐
	if (ext==0)
	{   
		for(pCur=pGryImg+(height-1)*width;pCur>=pGryImg;pCur-=width) //倒写
		{   
			fwrite((void *)pCur, sizeof(BYTE), width, fp);
		}
	}
	else
	{
		p[0] = p[1] = p[2] = p[3] = 0; //扩充的数据设为0
		for(pCur=pGryImg+(height-1)*width;pCur>=pGryImg;pCur-=width) //倒写
		{   
			fwrite((void *)pCur, sizeof(BYTE), width, fp); //图像数据
			fwrite((void *)p, sizeof(BYTE), ext, fp); //扩充数据
		}
	}
	//step.5-----------------退出--------------------------//
	fclose(fp); //关闭文件
	return true;
}

//灰度图反相
void InvertGry(unsigned char *pGryImg,int width,int height){
    unsigned char *p,*pEnd=pGryImg+width*height;//指针，图片文件末尾
    for(p=pGryImg;p<pEnd;p++){
        *p=255-*p;
    }
    return;
}
//灰度图的伪彩色
bool fakeGry2Color(unsigned char *pGryImg,int width,int height,const char *filename){
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER dataHeader;
	FILE *fp;
	int i, ext;
	BYTE p[4],*pCur;

	//step.0-----------------创建文件-----------------------//
    if( (fp=fopen(filename,"w+b"))==NULL ) return false; 
	//step.1-----------------写文件描述信息-----------------//
	fileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	fileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4L; //灰度值起始位置,调色板大小为256*4
    fileHeader.bfSize=fileHeader.bfOffBits+width*height ;
    fileHeader.bfReserved1=0;
    fileHeader.bfReserved2=0;
	fwrite((void *)&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	//step.2-----------------写数据描述信息-----------------//
	memset((void *)&dataHeader, 0, sizeof(BITMAPINFOHEADER));
	dataHeader.biSize = 40;
	dataHeader.biWidth = width;
	dataHeader.biHeight = height;
	dataHeader.biPlanes = 1;
	dataHeader.biBitCount = 8;
	fwrite((void *)&dataHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    //TODO:下面是自己改写的板块,调色板规则
    p[0]=p[1]=p[3]=p[2]=0;//2-r 0-b 1-g  3-黑色
    for(int i=0;i<256;i++){
        //样例只是修改了红色的值：p[2]=i;
        //新的参考这篇博客给的颜色转换函数：https://blog.csdn.net/pigbossa/article/details/120295316
        if(i<64){
            p[0]=255;
            p[1]=i;
            p[2]=0;
        }else if(i<128){
            p[0]=255-i;
            p[1]=255;
            p[2]=0;
        }else if(i<196){
            p[0]=0;
            p[1]=255;
            p[2]=i;
        }else{
            p[0]=0;
            p[1]=255-i;
            p[2]=255;
        }
        
        fwrite((void *)p,sizeof(BYTE),4,fp);
    }

    //step.4-----------------写图像数据---------------------//
	ext=(width+3)/4*4-width; //每行字节个数需要按4的倍数对齐
	if (ext==0)
	{   
		for(pCur=pGryImg+(height-1)*width;pCur>=pGryImg;pCur-=width) //倒写
		{   
			fwrite((void *)pCur, sizeof(BYTE), width, fp);
		}
	}
	else
	{
		p[0] = p[1] = p[2] = p[3] = 0; //扩充的数据设为0
		for(pCur=pGryImg+(height-1)*width;pCur>=pGryImg;pCur-=width) //倒写
		{   
			fwrite((void *)pCur, sizeof(BYTE), width, fp); //图像数据
			fwrite((void *)p, sizeof(BYTE), ext, fp); //扩充数据
		}
	}
	//step.5-----------------退出--------------------------//
	fclose(fp); //关闭文件
	return true;

}

//彩色图反相
void invertColor(unsigned char * pRgbImg,int width,int height){
    unsigned char *p,*pEnd=pRgbImg+width*height*3;//三通道的数据
    for(p=pRgbImg;p<pEnd;p++){
        *p=255-*p;
    }
    return;
}
void Exam_Gry(){
    //首先读取灰度图像
    unsigned char * pGryImg;
    int width,height;
    pGryImg=Rmw_Read8BitBmpFile2Img("H0101Gry.bmp",&width,&height);
    if(!pGryImg){
        printf("***load Gry Image ERROR!**\n");
        return;
    }
    /*********灰度图的反相**********/
    //灰度图像的反相
    InvertGry(pGryImg,width,height);
    //保存反相
    Rmw_Write8BitImg2BmpFile(pGryImg,width,height,"./res_data/H101Gry_invert.bmp");
    /*********灰度图的伪彩色*******/
    fakeGry2Color(pGryImg,width,height,"./res_data/H101_Gry2Color_all.bmp");

    delete pGryImg;
    return ;
}
void Exam_Color(){
    unsigned char *pRgbImg;
    int width,height;
    pRgbImg=Rmw_Read24BitBmpFile2Img("H0102Rgb.bmp",&width,&height);
    if(!pRgbImg){
        printf("***load Rgb Image ERROR!**\n");
        return;
    }
    //彩色图像的反相
    invertColor(pRgbImg,width,height);
    //保存图像
    Rmw_Write24BitImg2BmpFile(pRgbImg,width,height,"./res_data/H102Rgb_invert.bmp");


    delete pRgbImg;
    return ;
}
int main(){
    Exam_Gry();
    Exam_Color();
    
	printf("success!");
	return 0;
}