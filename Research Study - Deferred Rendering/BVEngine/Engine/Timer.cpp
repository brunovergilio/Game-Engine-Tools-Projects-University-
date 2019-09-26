#include "Timer.h"

namespace BVEngine
{
	Timer::Timer()
		: m_OneOverFrequency(0.0f), m_PrevTime({ { 0 } }), m_CurrTime({ { 0 } })
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		m_OneOverFrequency = 1.0f / (float)frequency.QuadPart;
	}


	Timer::Timer(const Timer & other)
		: m_OneOverFrequency(other.m_OneOverFrequency), m_PrevTime(other.m_PrevTime), m_CurrTime(other.m_CurrTime)
	{
	}


	Timer & Timer::operator = (const Timer & other)
	{
		if (this != &other)
		{
			m_OneOverFrequency = other.m_OneOverFrequency;
			m_PrevTime = other.m_PrevTime;
			m_CurrTime = other.m_CurrTime;
		}
		
		return *this;
	}


	Timer::~Timer()
	{
		m_OneOverFrequency = 0.0f;
		m_PrevTime.QuadPart = 0;
		m_CurrTime.QuadPart = 0;
	}


	void Timer::Reset()
	{
		m_PrevTime.QuadPart = 0;
		m_CurrTime.QuadPart = 0;
	}


	void Timer::Update()
	{
		m_PrevTime = m_CurrTime;
		QueryPerformanceCounter(&m_CurrTime);
	}


	float Timer::GetDelta()
	{
		return (float)(m_CurrTime.QuadPart - m_PrevTime.QuadPart) * m_OneOverFrequency;
	}
}