#include <iostream>
#include "Manderbrot.h"

int main(int argc, char *argv[])
{
	Manderbrot it(std::complex<double>{2.0, 3.0}, 10, std::complex<double>{4.0, 5.0});
	std::cout << it.get_iteration_point() << std::endl;
	std::cout << it.get_iteration_const() << std::endl;
	Manderbrot it2(2.0, 3.0, 10, 4.0, 5.0);
	std::cout << it2.get_iteration_point() << std::endl;
	std::cout << it2.get_iteration_const() << std::endl;
	
	return 0;
};
