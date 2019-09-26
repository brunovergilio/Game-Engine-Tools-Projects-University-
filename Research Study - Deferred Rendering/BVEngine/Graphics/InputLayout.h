#ifndef INPUTLAYOUT_H
#define INPUTLAYOUT_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class InputLayout
	{
	public:
		friend class VertexShader;

		InputLayout();
		~InputLayout();

		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint,
			const D3D11_INPUT_ELEMENT_DESC * const pInputElem, const size_t elemCount);

		void Release();

		inline ID3D11InputLayout * Get() const { return m_pInputLayout.Get(); }

	private:
		InputLayout(const InputLayout & other) = delete;
		InputLayout(InputLayout && other) = delete;
		InputLayout & operator = (const InputLayout & other) = delete;
		InputLayout & operator = (InputLayout && other) = delete;

	private:
		ComPtr<ID3D11InputLayout> m_pInputLayout;
	};
}

#endif