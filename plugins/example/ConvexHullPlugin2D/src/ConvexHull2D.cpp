#include "ConvexHull2D.h"

ConvexHull2D::ConvexHull2D(std::vector<Point2>& _point)
	:point(_point)
{
}

std::vector<Point2> ConvexHull2D::getConvexPoints()
{
	if (point.size() < 3)
	{
		ccLog::Error("切片中点的数量小于3");
		return{};
	}

	std::vector<Point2> convexPoint;
	//凸包算法的实现
	size_t minIndex = min_element(point.begin(), point.end(),
		[](const Point2& m, const Point2& n) { return m.y < n.y; }) - point.begin();	//获取y集合汇总最小值得索引
	Point2 minPoint = point[minIndex];		//获取y值最小的点

	//获取每个点与水平轴的夹角
	for (int i = 0; i < point.size(); i++)
	{
		point[i].angle = PolerAngle(minPoint, point[i]);
	}

	//按照点集中的角度值来进行排序
	sort(point.begin(), point.end(),
		[](const Point2& m, const Point2& n) {return m.angle < n.angle; });

	convexPoint.push_back(point[0]);
	convexPoint.push_back(point[1]);
	for (int i = 2; i < point.size(); i++)
	{
		while (CrossProduct(convexPoint[convexPoint.size() - 2],
			convexPoint[convexPoint.size() - 1], point[i]) >= 0)
		{
			convexPoint.pop_back();
		}
		convexPoint.push_back(point[i]);
	}


	return convexPoint;
}

double ConvexHull2D::PolerAngle(const Point2& p0, Point2& p1)
{
	double angle;
	Point2 vec1(50, 0), vec2;		//构建一个与x轴平行的向量vec1
	vec2 = p1 - p0;
	angle = acos((vec1 * vec2) / (vec1.norm() * vec2.norm())) * 180 / M_PI;
	if (isnan(angle))
	{
		angle = 0;
	}

	return angle;
}

double ConvexHull2D::CrossProduct(Point2& p1, Point2& p2, Point2& p3)
{
	Point2 vec1, vec2;
	vec1 = p2 - p1;
	vec2 = p3 - p1;
	return vec1.y * vec2.x - vec1.x * vec2.y;
}
