#pragma once

#include <random>
#include <vector>

#include "LittleEngine/Utils/logger.h"



namespace LittleEngine::Math
{
	inline std::mt19937& Generator()
	{
		static std::mt19937 generator(std::random_device{}());
		return generator;
	}

#pragma region Vector Choose
	// choose element from vector
	template<typename T>
	inline const T& Choose(const std::vector<T>& vec)
	{
		if (vec.empty())
		{
			Utils::Logger::Warning("Random::Choose: Cannot choose from an empty vector.");
			return T(); // return default value of T
		}
		std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
		return vec[dist(Generator())];
	}

	template<typename T>
	inline T& Choose(std::vector<T>& vec)	
	{
		if (vec.empty())
		{
			ThrowError("Random::Choose: Cannot choose from an empty vector.");
			return T(); // return default value of T
		}
		std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
		return vec[dist(Generator())];
	}

#pragma endregion

#pragma region Range
	
	// choose random float in range [0, 1]
	float Random();


	// choose random float in range [min, max)
	float RandomFloat(float min, float max);

	// choose random int in range [min, max)
	int RandomInt(int min, int max);

}