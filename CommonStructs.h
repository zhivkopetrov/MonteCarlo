#ifndef COMMON_STRUCTS_H_
#define COMMON_STRUCTS_H_

struct Point
{
	Point() : x(0.0), y(0.0) {}
	Point(double inputX, double inputY) : x(inputX), y(inputY) {}

	double x;
	double y;
};

using OvalRadius = Point;

struct MonteCarloArgs
{
	Point animationCenter;
	double animationScale;
	OvalRadius ovalRadius;
};

enum TextIndex
{
	TIME,
	ALL_POINTS,
	ERROR,
	TEXTS_COUNT
};

#endif /* COMMON_STRUCTS_H_ */