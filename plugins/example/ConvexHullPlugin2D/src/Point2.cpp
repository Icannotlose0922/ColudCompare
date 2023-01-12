#include "Point2.h"
#include<math.h>

Point2::Point2(double x, double y, double angle)
	:x(x), y(y), angle(angle)
{
}

Point2 Point2::operator+(const Point2& other)
{
	return Point2(x + other.x, y + other.y);
}

Point2 Point2::operator-(const Point2& other)
{
	return Point2(x - other.x, y - other.y);
}

double Point2::operator*(const Point2 & other)
{
	return x*other.x + y*other.y;
}

double Point2::norm()
{
	return sqrt(x*x + y*y);
}

Point2::~Point2()
{
}
