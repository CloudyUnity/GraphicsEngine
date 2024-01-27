#include "MathUtilsClass.h"
#include <algorithm>

float MathUtilsClass::LerpCurveWithIndex(int index, float t)
{
	if (index == 0)
		return t;

	if (index == 1)
		return t * t;

	if (index == 2)
		return t * t * t;

	if (index == 3)
		return CosCurve(t);

	if (index == 4)
		return HumpCurve(1, t);

	if (index == 5)
		return FlatCurve(5, t);

	return -1;
}

float MathUtilsClass::CosCurve(float t)
{
	return 0.5f - 0.5f * cos(PI * t);
}

float MathUtilsClass::FlatCurve(float rise, float t)
{
	return std::clamp(rise - rise * cos(2 * PI * t), 0.0, 1.0);
}

float MathUtilsClass::HumpCurve(float peak, float t)
{
	return (-4 * peak) * pow(t - 0.5f, 2) + peak;
}
