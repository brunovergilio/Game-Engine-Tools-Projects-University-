#ifndef _TIMER_H_
#define _TIMER_H_

#include "Utils.h"

/// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
//// PARTICULAR PURPOSE. 
//// 
//// Copyright (c) Microsoft Corporation. All rights reserved 

class Timer
{
	public:
		Timer();

		float PlayingTime() const;            // Return the Elapsed time the Game has been active in seconds since Reset 
		void PlayingTime(float time);   // Set the Elapsed playing time -- used for restarting in the middle of a game 
		float DeltaTime() const;              // Return the Delta time between the last two updates 

		void Reset();
		void Start();
		void Stop();
		void Update();
		bool Active() const;

	private:
		float m_SecondsPerCount;  // 1.0 / Frequency 
		float m_DeltaTime;

		LARGE_INTEGER m_BaseTime;
		LARGE_INTEGER m_PausedTime;
		LARGE_INTEGER m_StopTime;
		LARGE_INTEGER m_PreviousTime;
		LARGE_INTEGER m_CurrentTime;

		bool m_Active;
};

#endif // !_TIMER_H_