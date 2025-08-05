#include "LittleEngine/Utils/random.h"


namespace
{
	static std::uniform_real_distribution<float> u_0_1(0.f, 1.f);
}


namespace LittleEngine::Random
{

	// choose random float in range [0, 1]
	float Random()
	{
		return u_0_1(Generator());
	}

	// choose random float in range [min, max]
	float Float(float min, float max)
	{
		if (min >= max) ThrowError("Random::Float: min must be less than max.");
		return min + (max - min) * u_0_1(Generator());
	} 

	int Int(int min, int max)
	{
		if (min >= max) ThrowError("Random::Int: min must be less than max.");
		std::uniform_int_distribution<int> dist(min, max - 1);
		return dist(Generator());
	}
}