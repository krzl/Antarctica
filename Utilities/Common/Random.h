#pragma once

class Random
{
public:

	static int32_t  GetRandomInt32(int32_t min = INT32_MIN, int32_t max = INT32_MAX);
	static int64_t  GetRandomInt64(int64_t min = INT64_MIN, int64_t max = INT64_MAX);
	static uint32_t GetRandomUInt32(uint32_t min = 0, uint32_t max = UINT32_MAX);
	static uint64_t GetRandomUInt64(uint64_t min = 0, uint64_t max = UINT64_MAX);
	static float    GetRandomFloat(float min = 0.0f, float max = 1.0f);
};
