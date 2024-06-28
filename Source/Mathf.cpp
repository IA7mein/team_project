#include "Mathf.h"
#include <stdlib.h>

//指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max)
{
	float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float range = min + random * (max - min);
	return range;
}

//線形補完
float Mathf::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}