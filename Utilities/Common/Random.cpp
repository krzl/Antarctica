#include "stdafx.h"
#include "Random.h"


#include <ctime>
#include <random>

int32_t Random::GetRandomInt32()
{
	std::default_random_engine generator(time(nullptr));
	const std::uniform_int_distribution<int32_t> distribution(INT32_MIN, INT32_MAX);
	return distribution(generator);
}

int64_t Random::GetRandomInt64()
{
	std::default_random_engine generator(time(nullptr));
	const std::uniform_int_distribution<int64_t> distribution(INT64_MIN, INT64_MAX);
	return distribution(generator);
}

uint32_t Random::GetRandomUInt32()
{
	std::default_random_engine generator(time(nullptr));
	const std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);
	return distribution(generator);
}

uint64_t Random::GetRandomUInt64()
{
	std::default_random_engine generator(time(nullptr));
	const std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
	return distribution(generator);
}

float Random::GetRandomFloat()
{
	union
	{
		float f;
		uint32_t n;
	} caster;
	caster.n = GetRandomUInt32();
	
	return caster.f;
}
