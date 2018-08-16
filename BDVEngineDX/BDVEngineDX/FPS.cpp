#include "FPS.h"

FPS::FPS()
	: m_FPS(0), m_Counter(0), m_PrevTime(0.0f)
{
}

FPS::~FPS()
{
}

void FPS::Update(const float time)
{
	m_Counter++;
	if (time - m_PrevTime > 1.0f)
	{
		m_FPS = m_Counter;
		m_Counter = 0;
		m_PrevTime = time;
	}
}

UINT FPS::GetFPS()
{
	return m_FPS;
}