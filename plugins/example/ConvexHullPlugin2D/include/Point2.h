#pragma once

//2D凸包点结构
class Point2
{
public:
	double x, y, angle;
	Point2(double x = 0, double y = 0, double angle = 0);
	Point2 operator + (const Point2& other);
	Point2 operator - (const Point2& other);
	double operator * (const Point2& other);
	double norm();
	~Point2();
};
