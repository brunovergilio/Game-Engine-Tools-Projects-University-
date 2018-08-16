#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::LookAt(CXMVECTOR position, CXMVECTOR lookAt, CXMVECTOR up)
{
	XMStoreFloat3(&m_Position, position);
	XMStoreFloat3(&m_LookAt, lookAt);
	XMStoreFloat3(&m_Up, up);
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixLookAtRH(position, lookAt, up));
}

void Camera::SetPerspective(const float angle, const float aspect, const float nearZ, const float farZ)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovRH(angle, aspect, nearZ, farZ));
}

void Camera::SetOrthographic(const float width, const float height, const float nearZ, const float farZ)
{
	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixOrthographicOffCenterRH(0.0f, width, height, 0.0f, nearZ, farZ));
}

void Camera::Walk(const float val)
{
	XMVECTOR vDir = XMLoadFloat3(&m_LookAt) - XMLoadFloat3(&m_Position);
	XMVECTOR vX = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR vY = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vZ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	
	XMVECTOR vWalk = XMVectorSet(
		XMVectorGetX(XMVector3Dot(vX, vDir)),
		XMVectorGetX(XMVector3Dot(vY, vDir)),
		XMVectorGetX(XMVector3Dot(vZ, vDir)),
		0.0f
		);

	XMStoreFloat3(&m_Position, XMLoadFloat3(&m_Position) + vWalk * -val);
	XMStoreFloat3(&m_LookAt, XMLoadFloat3(&m_LookAt) + vWalk * -val);

	LookAt(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_LookAt), XMLoadFloat3(&m_Up));
}

void Camera::Strafe(const float val)
{
	UNREFERENCED_PARAMETER(val);
	//Vect vX(1.0f, 0.0f, 0.0f);
	//Vect vY(0.0f, 1.0f, 0.0f);
	//Vect vZ(0.0f, 0.0f, 1.0f);
	//Vect vStrafe(vX.dot(vRight), vY.dot(vRight), vZ.dot(vRight));

	//vPos += vStrafe * -val;
	//vLookAt += vStrafe * -val;

	//setOrientAndPosition(vUp, vLookAt, vPos);
}

void Camera::Fly(const float val)
{
	UNREFERENCED_PARAMETER(val);
	//Vect vX(1.0f, 0.0f, 0.0f);
	//Vect vY(0.0f, 1.0f, 0.0f);
	//Vect vZ(0.0f, 0.0f, 1.0f);
	//Vect vFly(vX.dot(vUp), vY.dot(vUp), vZ.dot(vUp));

	//vPos += vFly * -val;
	//vLookAt += vFly * -val;

	//setOrientAndPosition(vUp, vLookAt, vPos);
}

XMMATRIX Camera::GetViewMatrixXM() const
{
	return XMLoadFloat4x4(&m_ViewMatrix);
}

XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

XMMATRIX Camera::GetProjMatrixXM() const
{
	return XMLoadFloat4x4(&m_ProjMatrix);
}

XMFLOAT4X4 Camera::GetProjMatrix() const
{
	return m_ProjMatrix;
}

XMMATRIX Camera::GetOrthoMatrixXM() const
{
	return XMLoadFloat4x4(&m_OrthoMatrix);
}

XMFLOAT4X4 Camera::GetOrthoMatrix() const
{
	return m_OrthoMatrix;
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&m_Position);
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_Position;
}