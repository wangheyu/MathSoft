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

	double get_dimension() {return dimension;};
	double get_height() {return height;};
	double get_width() {return width;};
	double get_ox() {return origin.x;};
	double get_oy() {return origin.y;};
	double get_lpp()
	{
		return (dimension * 2.0 / width);
	};
};
