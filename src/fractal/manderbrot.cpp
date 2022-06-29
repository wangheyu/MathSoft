#include <iostream>
#include "Window.h"
#include "Manderbrot.h"
#include <fstream>
#include "../bmp/bitmap.h"

int main(int argc, char *argv[])
{
	Window win;
	double lpp = win.get_lpp();
	double dim = win.get_dimesion();
	int width = win.get_width();
	int height = win.get_height();
	double ox = win.get_ox() - dim, oy = win.get_oy() - dim / width * height;

	char *dummy = new char [width * height * 3];

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
				dummy[(width * j + i) * 3] = 255;
				dummy[(width * j + i) * 3 + 1] = 255;
				dummy[(width * j + i) * 3 + 2] = 255;
			}
			else
			{
				dummy[(width * j + i) * 3] = 0;
				dummy[(width * j + i) * 3 + 1] = 0;
				dummy[(width * j + i) * 3 + 2] = 0;
			}
		}
	}

	build_bmp(argv[1], width, height, dummy);
	delete [] dummy;

	return 0;
};
