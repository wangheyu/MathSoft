#include <iostream>
#include "Manderbrot.h"

int main(int argc, char *argv[])
{
	Manderbrot it(std::complex<double>{0.0, 0.0}, 20, std::complex<double>{0.25, 0.25});
	while (!it.stop_criterion())
	{
		it.forward_step();
		if (it.is_disconvergence())
			break;
	}
	if (it.stop_criterion())
		std::cout << "white" << std::endl;
	else
		std::cout << "black" << std::endl;
	std::cout << it.get_iteration_point() << std::endl;
	std::cout << it.get_iteration_const() << std::endl;
	Manderbrot it2(2.0, 3.0, 10, 4.0, 5.0);
	std::cout << it2.get_iteration_point() << std::endl;
	std::cout << it2.get_iteration_const() << std::endl;
	
	return 0;
};
