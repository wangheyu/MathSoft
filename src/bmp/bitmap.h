#ifndef __CRAZYFISH_BITMAP__
#define __CRAZYFISH_BITMAP__

#include <string>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;

typedef struct tagBITMAPFILEHEADER
{
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD        biSize;
  LONG         biWidth;
  LONG         biHeight;
  WORD         biPlanes;
  WORD         biBitCount;
  DWORD        biCompression;
  DWORD        biSizeImage;
  LONG         biXPelsPerMeter;
  LONG         biYPelsPerMeter;
  DWORD        biClrUsed;
  DWORD        biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct {
  DWORD        bV5Size;
  LONG         bV5Width;
  LONG         bV5Height;
  WORD         bV5Planes;
  WORD         bV5BitCount;
  DWORD        bV5Compression;
  DWORD        bV5SizeImage;
  LONG         bV5XPelsPerMeter;
  LONG         bV5YPelsPerMeter;
  DWORD        bV5ClrUsed;
  DWORD        bV5ClrImportant;
  DWORD        bV5RedMask;
  DWORD        bV5GreenMask;
  DWORD        bV5BlueMask;
  DWORD        bV5AlphaMask;
  DWORD        bV5CSType;
	//  CIEXYZTRIPLE bV5Endpoints;
  DWORD        bV5GammaRed;
  DWORD        bV5GammaGreen;
  DWORD        bV5GammaBlue;
  DWORD        bV5Intent;
  DWORD        bV5ProfileData;
  DWORD        bV5ProfileSize;
  DWORD        bV5Reserved;
} __attribute__((packed)) BITMAPV5HEADER;

void build_bmp(std::string _file,
			   int _w,
			   int _h,
			   char *_image);

#else

/// Do nothing.

#endif
