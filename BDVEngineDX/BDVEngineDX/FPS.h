#ifndef FPS_H
#define FPS_H

#include "Utils.h"

class FPS
{
public:
	FPS();
	~FPS();

	void Update(const float time);
	UINT GetFPS();

private:
	// Not needed
	FPS(const FPS & fps) = delete;
	FPS & operator = (const FPS & fps) = delete;

private:
	UINT m_FPS;
	UINT m_Counter;
	float m_PrevTime;
};

#endif