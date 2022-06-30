#include <iostream>
#include <fstream>
#include "bitmap.h"

int main(int argc, char* argv[])
{
	int width = 1920;
	int height = 1080;

	char *cache = new char[width * height * 3];

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			int pos = j * width + i;
			cache[pos * 3] = 0;
			cache[pos * 3 + 1] = 0;
			cache[pos * 3 + 2] = 255;
		}
	build_bmp("test.bmp", width, height, cache);
	delete [] cache;
};

