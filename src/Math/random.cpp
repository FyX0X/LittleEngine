#include "LittleEngine/Math/random.h"


namespace
{
	static std::uniform_real_distribution<float> u_0_1(0.f, 1.f);
}


namespace LittleEngine::Math
{

	// choose random float in range [0, 1]
	float Random()
	{
		return u_0_1(Generator());
	}

	// choose random float in range [min, max]
	float RandomFloat(float min, float max)
	{
		if (min >= max)
		{
			Utils::Logger::Warning("Random::Float: min must be less than max.");
			return min; // or throw an exception
		}
		return min + (max - min) * u_0_1(Generator());
	}

	int RandomInt(int min, int max)
	{
		if (min >= max)
		{
			Utils::Logger::Warning("Random::Int: min must be less than max.");
			return min; // or throw an exception
		}
		std::uniform_int_distribution<int> dist(min, max - 1);
		return dist(Generator());
	}
}