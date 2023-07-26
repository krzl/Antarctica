#pragma once

class Timer
{
public:

	void Start();
	void Stop();
	void Restart();

	float GetTime() const;

private:

	std::chrono::steady_clock::time_point m_startTime;

	float m_accumulator = 0.0f;
	bool m_isStopped    = true;
};
