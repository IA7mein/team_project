#include "Mathf.h"
#include <stdlib.h>

//�w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max)
{
	float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float range = min + random * (max - min);
	return range;
}

//���`�⊮
float Mathf::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}