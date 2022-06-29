#include <iostream>
#include <fstream>
#include <thread>
#include "bitmap.h"

int main(int argc, char *argv[])
{
	std::ifstream bmpFile;
	std::ofstream outTest;
	bmpFile.open("../images/matrix.bmp", std::ios::binary);
	outTest.open("test.bmp", std::ios::binary);
	if (!bmpFile.is_open())
	{
		std::cout << "Can not open the file." << std::endl;
		std::exit(-1);
	}
	BITMAPFILEHEADER bmpFileHead;
	BITMAPINFOHEADER bmpInfoHead;

	bmpFile.read(reinterpret_cast<char*>(&bmpFileHead), sizeof(BITMAPFILEHEADER));
	bmpFile.read(reinterpret_cast<char*>(&bmpInfoHead), sizeof(BITMAPINFOHEADER));
	std::cout << "file type: "
			  << ((char*)(&bmpFileHead.bfType))[0]
			  << ((char*)(&bmpFileHead.bfType))[1] << std::endl;
	std::cout << "file size: "
			  << bmpFileHead.bfSize << std::endl;
 
	std::cout << "reserved01: "
			  << bmpFileHead.bfReserved1 << std::endl;
	std::cout << "reserved02: "
			  << bmpFileHead.bfReserved2 << std::endl;
	
	std::cout << "data offset: "
			  << bmpFileHead.bfOffBits << std::endl;
	std::cout << "structure size: "
			  << bmpInfoHead.biSize << std::endl;
	std::cout << "picture width: "
			  << bmpInfoHead.biWidth << std::endl;
	std::cout << "picture height: "
			  << bmpInfoHead.biHeight << std::endl;
	std::cout << "planes: "
			  << bmpInfoHead.biPlanes << std::endl;
	std::cout << "bit count: "
			  << bmpInfoHead.biBitCount << std::endl;
	std::cout << "compression: "
			  << bmpInfoHead.biCompression << std::endl;
	std::cout << "size of the image: "
			  << bmpInfoHead.biSizeImage << std::endl;
	std::cout << "x ppm: "
			  << bmpInfoHead.biXPelsPerMeter << std::endl;
	std::cout << "y ppm: "
			  << bmpInfoHead.biYPelsPerMeter << std::endl;
	std::cout << "color used: "
			  << bmpInfoHead.biClrUsed << std::endl;
	std::cout << "color important: "
			  << bmpInfoHead.biClrImportant << std::endl;
	std::cout << sizeof(bmpInfoHead) << std::endl;
	std::cout << sizeof(bmpFileHead) << std::endl;

	
	int offbits = sizeof(bmpFileHead) + sizeof(bmpFileHead);
	int rest = bmpFileHead.bfOffBits - offbits;
	char *dummy = new char [rest];
	bmpFile.read(dummy, rest);
	delete [] dummy;
	dummy = new char [bmpInfoHead.biSizeImage];
	bmpFile.read(dummy, bmpInfoHead.biSizeImage);
	bmpFile.close();

	bmpInfoHead.biSize = sizeof(bmpInfoHead);
	bmpFileHead.bfOffBits = offbits;
	bmpFileHead.bfSize = bmpInfoHead.biSizeImage + bmpFileHead.bfOffBits;
	outTest.write(reinterpret_cast<char*>(&bmpFileHead), sizeof(BITMAPFILEHEADER));
	outTest.write(reinterpret_cast<char*>(&bmpInfoHead), sizeof(BITMAPINFOHEADER));
	outTest.write(dummy, bmpInfoHead.biSizeImage);
	
	outTest.close();

	delete [] dummy;
	return 0;
};
