#pragma once
#ifndef CONVEXHULL2D_H
#define CONVEXHULL2D_H
#define _USE_MATH_DEFINES

#include<vector>
#include"Point2.h"
#include<math.h>
#include<ccLog.h>
#include<algorithm>

class ConvexHull2D
{
	std::vector<Point2> point;
public:
	ConvexHull2D(std::vector<Point2>& _point);
	~ConvexHull2D() {};
	std::vector<Point2> getConvexPoints();
	//计算两点连线与水平轴的极角
	double PolerAngle(const Point2& p0, Point2& p1);
	//判断点p3是否在线p1p2的逆时针方向
	double CrossProduct(Point2& p1, Point2& p2, Point2& p3);
};

#endif // !CONVEXHULL2D_H
