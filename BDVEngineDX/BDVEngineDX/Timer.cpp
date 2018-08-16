/// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
//// PARTICULAR PURPOSE. 
//// 
//// Copyright (c) Microsoft Corporation. All rights reserved 

#include "Timer.h" 

Timer::Timer() : m_Active(false)
{
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
	{
	}
	m_SecondsPerCount = 1.0f / static_cast<float>(frequency.QuadPart);

	Reset();
}

// Returns the total time elapsed since Reset() was called, NOT counting any 
// time when the clock is stopped. 
float Timer::PlayingTime() const
{
	if (m_Active)
	{
		// The distance m_currentTime - m_baseTime includes paused time, 
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from m_currentTime: 
		return static_cast<float>(((m_CurrentTime.QuadPart - m_PausedTime.QuadPart) - m_BaseTime.QuadPart) * m_SecondsPerCount);
	}
	else
	{
		// The clock is currently not running so don't count the time since 
		// the clock was stopped 
		return static_cast<float>(((m_StopTime.QuadPart - m_PausedTime.QuadPart) - m_BaseTime.QuadPart) * m_SecondsPerCount);
	}
}


void Timer::PlayingTime(float time)
{
	// Reset the internal state to reflect a PlayingTime of 'time' 
	// Offset the baseTime by this 'time'. 
	m_Active = false;
	m_StopTime = m_CurrentTime;
	m_PausedTime.QuadPart = 0;

	m_BaseTime.QuadPart = m_StopTime.QuadPart - static_cast<__int64>(time / m_SecondsPerCount);
}


float Timer::DeltaTime() const
{
	return m_DeltaTime;
}

void Timer::Reset()
{
	LARGE_INTEGER currentTime;
	if (!QueryPerformanceCounter(&currentTime))
	{
	}
	m_BaseTime = currentTime;
	m_PreviousTime = currentTime;
	m_StopTime = currentTime;
	m_CurrentTime = currentTime;
	m_PausedTime.QuadPart = 0;
	m_Active = false;
}

void Timer::Start()
{
	LARGE_INTEGER startTime;
	if (!QueryPerformanceCounter(&startTime))
	{
	}
	if (!m_Active)
	{
		// Accumulate the time elapsed between stop and start pairs. 
		m_PausedTime.QuadPart += (startTime.QuadPart - m_StopTime.QuadPart);

		m_PreviousTime = startTime;
		m_StopTime.QuadPart = 0;
		m_Active = true;
	}
}

void Timer::Stop()
{
	if (m_Active)
	{
		// Set the stop time to the time of the last update. 
		m_StopTime = m_CurrentTime;
		m_Active = false;
	}
}

void Timer::Update()
{
	if (!m_Active)
	{
		m_DeltaTime = 0.0;
		return;
	}

	LARGE_INTEGER currentTime;
	if (!QueryPerformanceCounter(&currentTime))
	{
	}
	m_CurrentTime = currentTime;

	m_DeltaTime = (m_CurrentTime.QuadPart - m_PreviousTime.QuadPart) * m_SecondsPerCount;
	m_PreviousTime = m_CurrentTime;

	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}

bool Timer::Active() const
{
	return m_Active;
}