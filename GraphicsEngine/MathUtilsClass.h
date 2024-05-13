#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "Settings.h"
using namespace DirectX;

class MathUtilsClass
{
public:
	static float LerpCurveWithIndex(int, float);

	static float HumpCurve(float, float);
	static float CosCurve(float);
	static float FlatCurve(float rise, float t);
};

#endif