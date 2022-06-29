#include <iostream>
#include <fstream>
#include <string>
#include "bitmap.h"

void build_bmp(std::string _file,
			   int _w,
			   int _h,
			   char *_image)
{
	std::ofstream outTest;
	outTest.open(_file.c_str(), std::ios::binary);

	BITMAPFILEHEADER bmpFileHead;
	BITMAPINFOHEADER bmpInfoHead;

	bmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHead.biWidth = _w;
	bmpInfoHead.biHeight = _h;
	bmpInfoHead.biPlanes = 1;
	bmpInfoHead.biBitCount = 24;
    bmpInfoHead.biCompression = 0;
    bmpInfoHead.biSizeImage = bmpInfoHead.biWidth * bmpInfoHead.biHeight * 3;
    bmpInfoHead.biXPelsPerMeter = 2834;
    bmpInfoHead.biYPelsPerMeter = 2834;
    bmpInfoHead.biClrUsed = 0;
    bmpInfoHead.biClrImportant = 0;

	((char*)(&bmpFileHead.bfType))[0] = 'B';
	((char*)(&bmpFileHead.bfType))[1] = 'M';
	bmpFileHead.bfReserved1 = 0;
	bmpFileHead.bfReserved2 = 0;
	bmpFileHead.bfOffBits = bmpInfoHead.biSize + sizeof(BITMAPFILEHEADER);
	bmpFileHead.bfSize = bmpInfoHead.biSizeImage + bmpFileHead.bfOffBits;

	outTest.write(reinterpret_cast<char*>(&bmpFileHead), sizeof(BITMAPFILEHEADER));
	outTest.write(reinterpret_cast<char*>(&bmpInfoHead), sizeof(BITMAPINFOHEADER));
	outTest.write(_image, bmpInfoHead.biSizeImage);
	outTest.close();
};
