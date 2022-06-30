#include <iostream>
#include "window.h"
#include "mandelbrot.h"

int main(int argc, char *argv[])
{
	Window win;
	Window win2(2.0, 3.0, 0.1);
	std::cout << "width: " << win2.get_width() << std::endl;
	std::cout << "height: " << win2.get_height() << std::endl;
	std::cout << "dimension: " << win2.get_dimension() << std::endl;
	std::cout << "lpp: " << win2.get_lpp() << std::endl;
	Manderbrot man;
	Manderbrot man1(2, 3, 10, 1, 7);
	std::cout << "ip: " << man1.get_iteration_point() << std::endl; 
	std::cout << "max_it: " << man1.get_max_iteration() << std::endl;
	std::cout << "ic:" << man1.get_iteration_const() << std::endl;
	return 0;
};
