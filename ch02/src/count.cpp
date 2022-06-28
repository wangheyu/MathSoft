#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
	// if (argc < 2)
	// {
	// 	std::cerr << "Too few parameters, stupid!" << std::endl;
	// 	std::exit(-1);
	// }
	int n = 0;
	std::cin >> n;
	std::cout << "Counting:" << std::endl;
	for (int i = 0; i < n; i++)
		std::cout << "count to " << i << std::endl;
	std::cerr << "Are you crazy, play me with such an idiot question!"
			  << std::endl;
	return 0;
};
