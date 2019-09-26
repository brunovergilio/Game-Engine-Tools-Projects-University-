#ifndef INPUT_H
#define INPUT_H

namespace BVEngine
{
	class Input
	{
	public:
		Input();
		~Input();

		void Update();

		bool IsKeyPressed();

	private:
		Input(const Input & other) = delete;
		Input(Input && other) = delete;
		Input & operator = (const Input & other) = delete;
		Input & operator = (Input && other) = delete;

	private:
		char m_PrevState[256];
		char m_CurrState[256];
		float m_MouseCurrX;
		float m_MousePrevX;
		float m_MouseCurrY;
		float m_MousePrevY;
	};
}
#endif