#include <iostream>
#include "Window.h"
#include "Manderbrot.h"
#include <fstream>
#include "../bmp/bitmap.h"

int main(int argc, char *argv[])
{
	Window win;

	std::ofstream outTest;
	outTest.open("test.bmp", std::ios::binary);

	BITMAPFILEHEADER bmpFileHead;
	BITMAPINFOHEADER bmpInfoHead;

	bmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHead.biWidth = 1920;
	bmpInfoHead.biHeight = 1080;
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

	std::cout << bmpInfoHead.biSizeImage << std::endl;
	std::cout << bmpInfoHead.biHeight << std::endl;
	std::cout << bmpInfoHead.biWidth << std::endl;

	char *dummy = new char [bmpInfoHead.biSizeImage];

	
	double lpp = win.get_lpp();
	double dim = win.get_dimesion();
	int width = win.get_width();
	int height = win.get_height();
	double ox = win.get_ox() - dim, oy = win.get_oy() - dim / width * height;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			double x = ox + lpp * i;
			double y = oy + lpp * j;

			Manderbrot it(std::complex<double>{0.0, 0.0}, 20, std::complex<double>{x, y});
			while (!it.stop_criterion())
			{
				it.forward_step();
				if (it.is_disconvergence())
					break;
			}
			if (it.stop_criterion())
			{
				dummy[(bmpInfoHead.biWidth * j + i) * 3] = 255;
				dummy[(bmpInfoHead.biWidth * j + i) * 3 + 1] = 255;
				dummy[(bmpInfoHead.biWidth * j + i) * 3 + 2] = 255;
			}
			//				std::cout << "white" << std::endl;
			else
			{
				dummy[(bmpInfoHead.biWidth * j + i) * 3] = 0;
				dummy[(bmpInfoHead.biWidth * j + i) * 3 + 1] = 0;
				dummy[(bmpInfoHead.biWidth * j + i) * 3 + 2] = 0;
				//				std::cout << "black" << std::endl;
			}
		}
	}
	outTest.write(reinterpret_cast<char*>(&bmpFileHead), sizeof(BITMAPFILEHEADER));
	outTest.write(reinterpret_cast<char*>(&bmpInfoHead), sizeof(BITMAPINFOHEADER));
	outTest.write(dummy, bmpInfoHead.biSizeImage);
	outTest.close();

	delete [] dummy;

	return 0;
};
