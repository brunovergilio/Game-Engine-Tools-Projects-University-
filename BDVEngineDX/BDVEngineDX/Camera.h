#ifndef CAMERA_H
#define CAMERA_H

#include "Utils.h"

class Camera
{
public:
	Camera();
	~Camera();

	void LookAt(CXMVECTOR position, CXMVECTOR lookAt, CXMVECTOR up);
	void SetPerspective(const float angle, const float aspect, const float nearZ, const float farZ);
	void SetOrthographic(const float width, const float height, const float nearZ, const float farZ);

	void Walk(const float val);
	void Strafe(const float val);
	void Fly(const float val);

	XMMATRIX GetViewMatrixXM() const;
	XMFLOAT4X4 GetViewMatrix() const;

	XMMATRIX GetProjMatrixXM() const;
	XMFLOAT4X4 GetProjMatrix() const;

	XMMATRIX GetOrthoMatrixXM() const;
	XMFLOAT4X4 GetOrthoMatrix() const;

	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;

private:
	// Not needed
	Camera(const Camera & camera) = delete;
	Camera & operator = (const Camera & camera) = delete;

private:
	XMFLOAT4X4 m_ViewMatrix;
	XMFLOAT4X4 m_ProjMatrix;
	XMFLOAT4X4 m_OrthoMatrix;

	XMFLOAT3 m_Position;
	XMFLOAT3 m_LookAt;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Dir;
};

#endif