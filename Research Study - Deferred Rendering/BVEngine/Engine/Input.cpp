#include "Input.h"
#include <memory>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace BVEngine
{
	Input::Input()
	{
		memset(m_PrevState, 0, 256);
		memset(m_CurrState, 0, 256);
	}


	Input::~Input()
	{
	}


	void Input::Update()
	{
	}


	bool Input::IsKeyPressed()
	{
		return false;
	}
}