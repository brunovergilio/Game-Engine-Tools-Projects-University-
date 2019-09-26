#include "Camera.h"
#include "GraphicsUtils.h"
#include "BVQuaternion_SIMD.h"

using namespace BVMath::SIMD;

Camera::Camera()
{
}


Camera::~Camera()
{
}


Camera::Camera(const Camera & other)
	: m_Position(other.m_Position), m_Right(other.m_Right), m_Up(other.m_Up), m_Look(other.m_Look),
	m_NearZ(other.m_NearZ), m_FarZ(other.m_FarZ), m_AspectRatio(other.m_AspectRatio), m_FovY(other.m_FovY),
	m_View(other.m_View), m_Proj(other.m_Proj)
{
}


Camera::Camera(Camera && other)
	: m_Position(other.m_Position), m_Right(other.m_Right), m_Up(other.m_Up), m_Look(other.m_Look),
	m_NearZ(other.m_NearZ), m_FarZ(other.m_FarZ), m_AspectRatio(other.m_AspectRatio), m_FovY(other.m_FovY),
	m_View(other.m_View), m_Proj(other.m_Proj)
{
	ZeroMemory(&other, sizeof(Camera));
}


Camera & Camera::operator = (const Camera & other)
{
	if (this != &other)
	{
		m_Position = other.m_Position;
		m_Right = other.m_Right;
		m_Up = other.m_Up;
		m_Look = other.m_Look;
		m_NearZ = other.m_NearZ;
		m_FarZ = other.m_FarZ;
		m_AspectRatio = other.m_AspectRatio;
		m_FovY = other.m_FovY;
		m_View = other.m_View;
		m_Proj = other.m_Proj;
	}

	return *this;
}


Camera & Camera::operator = (Camera && other)
{
	if (this != &other)
	{
		m_Position = other.m_Position;
		m_Right = other.m_Right;
		m_Up = other.m_Up;
		m_Look = other.m_Look;
		m_NearZ = other.m_NearZ;
		m_FarZ = other.m_FarZ;
		m_AspectRatio = other.m_AspectRatio;
		m_FovY = other.m_FovY;
		m_View = other.m_View;
		m_Proj = other.m_Proj;

		ZeroMemory(&other, sizeof(Camera));
	}

	return *this;
}

void Camera::LookAt(CBVVector eyePos, CBVVector eyeDir, CBVVector up)
{
	BVVector z = VectorNormalize(eyeDir - eyePos);
	BVVector x = VectorNormalize(VectorCross(up, z));
	BVVector y = VectorCross(z, x);

	VectorToFloat3(x, m_Right);
	VectorToFloat3(y, m_Up);
	VectorToFloat3(z, m_Look);
	VectorToFloat3(eyePos, m_Position);
}

void Camera::SetPerspective(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
{
	m_NearZ = nearZ;
	m_FarZ = farZ;
	m_AspectRatio = aspectRatio;
	m_FovY = fovY;
}

void Camera::UpdateViewMatrix()
{
	BVVector z = VectorNormalize(VectorFromFloat3(m_Look));
	BVVector x = VectorNormalize(VectorCross(VectorFromFloat3(m_Up), z));
	BVVector y = VectorCross(z, x);
	BVVector p = VectorFromFloat3(m_Position);

	VectorToFloat3(x, m_Right);
	VectorToFloat3(y, m_Up);
	VectorToFloat3(z, m_Look);

	BVVector id4 = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	BVMatrix view(x, y, z, id4);
	view = MatrixTranspose(view);
	view.r[3] = VectorSet(-VectorDot(x, p), -VectorDot(y, p), -VectorDot(z, p), 1.0f);

	MatrixToFloat4x4(view, m_View);
}

void Camera::UpdateProjMatrix()
{
	MatrixToFloat4x4(MatrixPerspectiveLH_DX(m_NearZ, m_FarZ, m_AspectRatio, m_FovY), m_Proj);
}

void Camera::Walk(const float val)
{
	BVVector vAmount = VectorReplicate(val);
	BVVector vDir = VectorMul(vAmount, VectorFromFloat3(m_Look));
	
	VectorToFloat3(VectorAdd(vDir, VectorFromFloat3(m_Position)), m_Position);
}

void Camera::Strafe(const float val)
{
	BVVector vAmount = VectorReplicate(val);
	BVVector vDir = VectorMul(vAmount, VectorFromFloat3(m_Right));

	VectorToFloat3(VectorAdd(vDir, VectorFromFloat3(m_Position)), m_Position);
}

void Camera::Fly(const float val)
{
	BVVector vAmount = VectorReplicate(val);
	BVVector vDir = VectorMul(vAmount, VectorFromFloat3(m_Up));

	VectorToFloat3(VectorAdd(vDir, VectorFromFloat3(m_Position)), m_Position);
}

void Camera::RotateX(const float angle)
{
	RotateBasis(VectorSet(1.0f, 0.0f, 0.0f), angle);
}

void Camera::RotateY(const float angle)
{
	RotateBasis(VectorSet(0.0f, 1.0f, 0.0f), angle);
}

void Camera::RotateZ(const float angle)
{
	RotateBasis(VectorSet(0.0f, 0.0f, 1.0f), angle);
}

void Camera::Pitch(const float angle)
{
	BVVector vRight = VectorFromFloat3(m_Right);
	BVQuat qRot = QuaternionRotationAxis(vRight, angle);

	BVVector vUp = VectorFromFloat3(m_Up);
	BVVector vLook = VectorFromFloat3(m_Look);

	vUp = QuaternionQVQC(qRot, vUp);
	vLook = QuaternionQVQC(qRot, vLook);

	VectorToFloat3(vUp, m_Up);
	VectorToFloat3(vLook, m_Look);
}

void Camera::Yaw(const float angle)
{
	BVVector vUp = VectorFromFloat3(m_Up);
	BVQuat qRot = QuaternionRotationAxis(vUp, angle);

	BVVector vLook = VectorFromFloat3(m_Look);
	BVVector vRight = VectorFromFloat3(m_Right);

	vLook = QuaternionQVQC(qRot, vLook);
	vRight = QuaternionQVQC(qRot, vRight);

	VectorToFloat3(vLook, m_Look);
	VectorToFloat3(vRight, m_Right);
}

void Camera::Roll(const float angle)
{
	BVVector vLook = VectorFromFloat3(m_Look);
	BVQuat qRot = QuaternionRotationAxis(vLook, angle);

	BVVector vRight = VectorFromFloat3(m_Right);
	BVVector vUp = VectorFromFloat3(m_Up);

	vRight = QuaternionQVQC(qRot, vRight);
	vUp = QuaternionQVQC(qRot, vUp);

	VectorToFloat3(vRight, m_Right);
	VectorToFloat3(vUp, m_Up);
}

void Camera::RotateBasis(CBVVector vBasis, const float angle)
{
	BVQuat qRot = QuaternionRotationAxis(vBasis, angle);

	BVVector vRight = VectorFromFloat3(m_Right);
	BVVector vUp = VectorFromFloat3(m_Up);
	BVVector vLook = VectorFromFloat3(m_Look);

	vLook = QuaternionQVQC(qRot, vLook);
	vRight = QuaternionQVQC(qRot, vRight);
	vUp = QuaternionQVQC(qRot, vUp);

	VectorToFloat3(vLook, m_Look);
	VectorToFloat3(vRight, m_Right);
	VectorToFloat3(vUp, m_Up);
}
