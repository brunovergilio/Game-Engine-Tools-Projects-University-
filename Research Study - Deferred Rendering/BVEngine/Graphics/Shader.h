#ifndef SHADER_H
#define SHADER_H

#include "GraphicsUtils.h"
#include "InputLayout.h"

namespace BVGraphics
{
	// Base Shader class
	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

	protected:
		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint, const char * const pShaderAndVersion,
			ComPtr<ID3DBlob> & pCompiledShader, ComPtr<ID3DBlob>  & pShaderError);

	private:
		Shader(const Shader & other) = delete;
		Shader(Shader && other) = delete;
		Shader & operator = (const Shader & other) = delete;
		Shader & operator = (Shader && other) = delete;
	};

	// Vertex Shader class
	class VertexShader : public Shader
	{
	public:
		VertexShader();
		~VertexShader();

		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint,
			InputLayout * const pInputLayout = nullptr, const D3D11_INPUT_ELEMENT_DESC * const pInputElem = nullptr, const unsigned int elemCount = 0);

		void Release();

		inline ID3D11VertexShader *GetShader() const { return m_pVertexShader.Get(); }

	private:
		VertexShader(const VertexShader & other) = delete;
		VertexShader(VertexShader && other) = delete;
		VertexShader & operator = (const VertexShader & other) = delete;
		VertexShader & operator = (VertexShader && other) = delete;

	private:
		ComPtr<ID3D11VertexShader> m_pVertexShader;
	};

	// Hull Shader class
	class HullShader : public Shader
	{
	public:
		HullShader();
		~HullShader();

		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint);

		void Release();

		inline ID3D11HullShader *GetShader() const { return m_pHullShader.Get(); }

	private:
		HullShader(const HullShader & other) = delete;
		HullShader(HullShader && other) = delete;
		HullShader & operator = (const HullShader & other) = delete;
		HullShader & operator = (HullShader && other) = delete;

	private:
		ComPtr<ID3D11HullShader> m_pHullShader;
	};

	// Domain Shader class
	class DomainShader : public Shader
	{
	public:
		DomainShader();
		~DomainShader();

		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint);

		void Release();

		inline ID3D11DomainShader *GetShader() const { return m_pDomainShader.Get(); }

	private:
		DomainShader(const DomainShader & other) = delete;
		DomainShader(DomainShader && other) = delete;
		DomainShader & operator = (const DomainShader & other) = delete;
		DomainShader & operator = (DomainShader && other) = delete;

	private:
		ComPtr<ID3D11DomainShader> m_pDomainShader;
	};

	// Geometry Shader class
	class GeometryShader : public Shader
	{
	public:
		GeometryShader();
		~GeometryShader();

		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint);

		void Release();

		inline ID3D11GeometryShader *GetShader() const { return m_pGeometryShader.Get(); }

	private:
		GeometryShader(const GeometryShader & other) = delete;
		GeometryShader(GeometryShader && other) = delete;
		GeometryShader & operator = (const GeometryShader & other) = delete;
		GeometryShader & operator = (GeometryShader && other) = delete;

	private:
		ComPtr<ID3D11GeometryShader> m_pGeometryShader;
	};

	// Pixel Shader class
	class PixelShader : public Shader
	{
	public:
		PixelShader();
		~PixelShader();

		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint);

		void Release();

		inline ID3D11PixelShader *GetShader() const { return m_pPixelShader.Get(); }

	private:
		PixelShader(const PixelShader & other) = delete;
		PixelShader(PixelShader && other) = delete;
		PixelShader & operator = (const PixelShader & other) = delete;
		PixelShader & operator = (PixelShader && other) = delete;

	private:
		ComPtr<ID3D11PixelShader> m_pPixelShader;
	};

	// Compute Shader class
	class ComputeShader : public Shader
	{
	public:
		ComputeShader();
		~ComputeShader();

		GraphicsResult CompileFromFile(const char * const pFilename, const char * const pEntryPoint);

		void Release();

		inline ID3D11ComputeShader *GetShader() const { return m_pComputeShader.Get(); }

	private:
		ComputeShader(const ComputeShader & other) = delete;
		ComputeShader(PixelShader && other) = delete;
		ComputeShader & operator = (const ComputeShader & other) = delete;
		ComputeShader & operator = (ComputeShader && other) = delete;

	private:
		ComPtr<ID3D11ComputeShader> m_pComputeShader;
	};
}

#endif