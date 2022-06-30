#include <complex>

class Iteration
{
protected:
	std::complex<double> iteration_point{0.0, 0.0};
	int iteration_times = 0;
	int max_iteration = 20;
	std::complex<double> iteration_const{0.0, 0.0};
	bool flag_stop = false;
	bool flag_disconvergence = false;
public:
	Iteration(){};
	Iteration(std::complex<double> _ip,
			  int _max_it,
			  std::complex<double> _ic) :
		iteration_point(_ip),
		max_iteration(_max_it),
		iteration_const(_ic) {};
	Iteration(double _ix,
			  double _iy,
			  int _max_it,
			  double _cx,
			  double _cy) :
		iteration_point(std::complex<double>{_ix, _iy}),
		max_iteration(_max_it),
		iteration_const(std::complex<double>{_cx, _cy}) {};
	std::complex<double> get_iteration_point()
	{
		return iteration_point;
	};
	int get_iteration_times()
	{
		return iteration_times;
	};
	std::complex<double> get_iteration_const()
	{
		return iteration_const;
	};
	int get_max_iteration()
	{
		return max_iteration;
	};
	virtual void forward_step() = 0;
	virtual bool stop_criterion() = 0;
	virtual bool is_disconvergence() = 0;
};

class Manderbrot : public Iteration
{
public:
	Manderbrot() : Iteration() {};
	Manderbrot(std::complex<double> _ip,
			   int _max_it,
			   std::complex<double> _ic) : Iteration(_ip, _max_it, _ic) {};
	Manderbrot(double _ix,
			   double _iy,
			   int _max_it,
			   double _cx,
			   double _cy) : Iteration(_ix, _iy, _max_it, _cx, _cy) {};
	virtual void forward_step();
	virtual bool stop_criterion();
	virtual bool is_disconvergence();
};

void Manderbrot::forward_step()
{
	iteration_point = iteration_point * iteration_point + iteration_const;
	iteration_times++;
	if (iteration_times == max_iteration)
		flag_disconvergence = true;
	if (std::abs(iteration_point) > 2.0)
		flag_stop = true;
};

bool Manderbrot::stop_criterion()
{
	return flag_stop;
};
bool Manderbrot::is_disconvergence()
{
	return flag_disconvergence;
};
