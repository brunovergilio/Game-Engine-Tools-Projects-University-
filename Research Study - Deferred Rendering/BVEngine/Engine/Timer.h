#ifndef TIMER_H
#define TIMER_H

#include "GraphicsUtils.h"

namespace BVEngine
{
	class Timer
	{
	public:
		Timer();
		Timer(const Timer & other);
		Timer & operator = (const Timer & other);
		virtual ~Timer();

		void Reset();
		void Update();

		float GetDelta();

	private:
		Timer(Timer && other) = delete;
		Timer & operator = (Timer && other) = delete;

	private:
		LARGE_INTEGER m_PrevTime;
		LARGE_INTEGER m_CurrTime;
		float m_OneOverFrequency;
	};
}
#endif