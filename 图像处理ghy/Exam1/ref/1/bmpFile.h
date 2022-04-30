#ifndef BMP_FILE_H
#define BMP_FILE_H
///////////////////////////////////////////////////////////////////////////////////
//
//  8bit
//
///////////////////////////////////////////////////////////////////////////////////
//Read
BYTE *Rmw_Read8BitBmpFile2Img(const char *filename,int *width,int *height);
bool Rmw_Read8BitBmpFile2Img(const char *filename, int *width, int *height, BYTE *pGryImg);
//Write
bool Rmw_Write8BitImg2BmpFile(BYTE *pGryImg,int width,int height,const char *filename);
bool Rmw_Write8BitImg2BmpFileRed(BYTE *pGryImg, int width, int height, const char *filename);
bool Rmw_Write8BitImg2BmpFileMark(BYTE *pGryImg, int width, int height, const char *filename);
///////////////////////////////////////////////////////////////////////////////////
//
//  24bit
//
///////////////////////////////////////////////////////////////////////////////////
//Read
BYTE *Rmw_Read24BitBmpFile2Img(const char *filename,int *width,int *height);
bool Rmw_Read24BitBmpFile2Img(const char *filename, int *width, int *height, BYTE *pRGBImg);
//Write
bool Rmw_Write24BitImg2BmpFile(BYTE *pRGBImg,int width,int height,const char *filename);
///////////////////////////////////////////////////////////////////////////////////
//
//  8/24bit
//
///////////////////////////////////////////////////////////////////////////////////
BYTE *Rmw_ReadBmpFile2Img(const char *filename, int *width, int *height, bool *isRGB);

#endif
