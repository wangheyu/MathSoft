#include <iostream>
#include "Window.h"

int main(int argc, char *argv[])
{
	Window win;
	double lpp = win.get_lpp();
	double ox = win.get_ox(), oy = win.get_oy();
	for (int i = 0; i < win.get_width(); i++)
	{
		for (int j = 0; j < win.get_height(); j++)
		{
			double x = ox + lpp * i;
			double y = oy + lpp * j;
			std::cout << "(" << x << ", " << y << ")";
		}
		std::cout << std::endl;
	}
	return 0;
};
