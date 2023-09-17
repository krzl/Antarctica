#include "stdafx.h"
#include "Random.h"

#include <random>

static std::random_device rd;
static std::mt19937 generator(rd());

int32_t Random::GetRandomInt32(const int32_t min, const int32_t max)
{
	std::uniform_int_distribution distribution(min, max);
	return distribution(generator);
}

int64_t Random::GetRandomInt64(const int64_t min, const int64_t max)
{
	std::uniform_int_distribution distribution(min, max);
	return distribution(generator);
}

uint32_t Random::GetRandomUInt32(const uint32_t min, const uint32_t max)
{
	std::uniform_int_distribution distribution(min, max);
	return distribution(generator);
}

uint64_t Random::GetRandomUInt64(const uint64_t min, const uint64_t max)
{
	std::uniform_int_distribution distribution(min, max);
	return distribution(generator);
}

float Random::GetRandomFloat(const float min, const float max)
{
	std::uniform_real_distribution distribution(min, max);
	return distribution(generator);
}
