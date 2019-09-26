#ifndef CAMERA_H
#define CAMERA_H

#include "BVMath.h"
#include "BVVector_SIMD.h"
#include "BVMatrix_SIMD.h"

using namespace BVMath;
using namespace BVMath::SIMD;

class Camera
{
public:
	Camera();
	~Camera();

	Camera(const Camera & other);
	Camera(Camera && other);
	Camera & operator = (const Camera & other);
	Camera & operator = (Camera && other);

	void LookAt(CBVVector eyePos, CBVVector eyeDir, CBVVector up);
	void SetPerspective(const float nearZ, const float farZ, const float aspectRatio, const float fovY);

	void UpdateViewMatrix();
	void UpdateProjMatrix();

	void Walk(const float val);
	void Strafe(const float val);
	void Fly(const float val);

	void RotateX(const float angle);
	void RotateY(const float angle);
	void RotateZ(const float angle);

	void Pitch(const float angle);
	void Yaw(const float angle);
	void Roll(const float angle);

	inline const BVFloat3 GetPosition() const { return m_Position; }
	inline const BVFloat3 GetRight() const { return m_Right; }
	inline const BVFloat3 GetUp() const { return m_Up; }
	inline const BVFloat3 GetLook() const { return m_Look; }

	inline const BVVector GetPositionV() const { return VectorFromFloat3(m_Position); }
	inline const BVVector GetRightV() const { return VectorFromFloat3(m_Right); }
	inline const BVVector GetUpV() const { return VectorFromFloat3(m_Up); }
	inline const BVVector GetLookV() const { return VectorFromFloat3(m_Look); }

	inline void SetPosition(CBVVector pos) { VectorToFloat3(pos, m_Position); }

	inline const float GetNearZ() const { return m_NearZ; }
	inline const float GetFarZ() const { return m_FarZ; }
	inline const float GetAspectRatio() const { return m_AspectRatio; }
	inline const float GetFovY() const { return m_FovY; }

	inline BVFloat44 GetView() const { return m_View; }
	inline BVFloat44 GetProj() const { return m_Proj; }
	inline BVFloat44 GetInvView() const { BVFloat44 invView; MatrixToFloat4x4(MatrixInverse(GetViewM()), invView); return invView; }
	BVFloat44 GetViewProj() const { BVFloat44 viewProj; MatrixToFloat4x4(GetViewM() * GetProjM(), viewProj); return viewProj; }

	inline BVMatrix GetViewM() const { return MatrixFromFloat4x4(m_View); }
	inline BVMatrix GetProjM() const { return MatrixFromFloat4x4(m_Proj); }
	inline BVMatrix GetInvViewM() const { return MatrixInverse(MatrixFromFloat4x4(m_View)); }
	BVMatrix GetViewProjM() const { return MatrixMul(GetViewM(), GetProjM()); }

private:
	void RotateBasis(CBVVector vBasis, const float angle);

private:
	BVFloat44 m_View;
	BVFloat44 m_Proj;

	BVFloat3 m_Right;
	float m_NearZ;
	BVFloat3 m_Up;
	float m_FarZ;
	BVFloat3 m_Look;
	float m_AspectRatio;
	BVFloat3 m_Position;
	float m_FovY;
};

#endif