struct Point2d
{
	double x;
	double y;
};

class Window
{
private:
	Point2d origin = {0.0, 0.0};
	double dimension = 5.0;
	int height = 1080;
	int width = 1920;
public:
	Window(){};
	Window(double _ox, double _oy, double _d)
	{
		origin.x = _ox;
		origin.y = _oy;
		dimension = _d;
	};

	double get_dimesion();
	int get_height();
	int get_width();
	double get_lpp();
	double get_ox();
	double get_oy();
};

double Window::get_dimesion()
{
	return dimension;
};

int Window::get_height()
{
	return height;
};

int Window::get_width()
{
	return width;
};

double Window::get_lpp()
{
	return (dimension * 2.0 /  width);
};

double Window::get_ox()
{
	return origin.x;
};

double Window::get_oy()
{
	return origin.y;
};
