#include "AnimatedGameObject.h"

AnimatedGameObject::AnimatedGameObject()
	: m_pToParent(nullptr), m_pToRoot(nullptr), m_pToWorld(nullptr), m_Time(0.0f), m_AnimationIndex(0U)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_pShaderParameters = new AnimatedShaderParameters();
	m_pShaderParameters->pInterpolationData = new ShaderInterpolationData();
}

AnimatedGameObject::AnimatedGameObject(BDVModel * const pModel, AnimatedShader * const pShader)
	: GameObject(pModel, pShader), m_Time(0.0f), m_AnimationIndex(0U)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_pShaderParameters = new AnimatedShaderParameters();
	m_pShaderParameters->pInterpolationData = new ShaderInterpolationData();

	m_pToParent = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
	m_pToRoot = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
	m_pToWorld = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
}

AnimatedGameObject::AnimatedGameObject(BDVModel * const pModel, AnimatedShader * const pShader, const XMFLOAT4 &color, Texture * const pTexture, Sampler * const pSampler, const Material &material)
	: GameObject(pModel, pShader), m_Time(0.0f), m_AnimationIndex(0U)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_pShaderParameters = new AnimatedShaderParameters();
	m_pShaderParameters->pInterpolationData = new ShaderInterpolationData();
	m_pShaderParameters->material = material;
	m_pShaderParameters->material.m_Color = color;
	m_pShaderParameters->pTexture = pTexture;
	m_pShaderParameters->pSampler = pSampler;

	m_pToParent = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
	m_pToRoot = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
	m_pToWorld = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
}

AnimatedGameObject::~AnimatedGameObject()
{
	delete m_pShaderParameters->pInterpolationData;
	delete m_pShaderParameters;

	if (m_pToParent)
	{
		_aligned_free(m_pToParent);
	}

	if (m_pToRoot)
	{
		_aligned_free(m_pToRoot);
	}

	if (m_pToWorld)
	{
		_aligned_free(m_pToWorld);
	}
}

void AnimatedGameObject::Update(const float dt)
{
	BDVModel *pBDVModel = (BDVModel *)m_pModel;
	if (m_Time > pBDVModel->GetAnimation(m_AnimationIndex)->pAnimSamples[pBDVModel->GetAnimation(m_AnimationIndex)->numAnimSamples - 1].time)
	{
		m_Time = 0.0f;
	}

	if (m_pShaderParameters->transformOnHardware)
	{
		InterpolateHardware();
	}
	else
	{
		Interpolate();
	}

	m_Time += dt;
}

void AnimatedGameObject::Render(const Graphics * const pGraphics, Camera * const pCamera)
{
	if (!(m_pModel && m_pShader))
	{
		return;
	}

	m_pShaderParameters->pFinalMatrices = m_pToWorld;
	m_pShaderParameters->numJoints = ((BDVModel *)m_pModel)->GetNumJoints();

	XMMATRIX bindPose = XMLoadFloat4x4(&((BDVModel *)m_pModel)->GetJoints()[0].invBindPose);
	bindPose.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	bindPose = XMMatrixInverse(nullptr, bindPose);

	XMFLOAT4X4 finalWorldMatrix;
	XMStoreFloat4x4(&finalWorldMatrix, bindPose * XMLoadFloat4x4(&m_WorldMatrix));

	XMStoreFloat4x4(&m_pShaderParameters->wvp, XMLoadFloat4x4(&finalWorldMatrix) * pCamera->GetViewMatrixXM() * pCamera->GetProjMatrixXM());
	m_pShaderParameters->world = finalWorldMatrix;
	XMMATRIX inv = XMLoadFloat4x4(&finalWorldMatrix);
	inv.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&m_pShaderParameters->invWorld, XMMatrixTranspose(XMMatrixInverse(nullptr, inv)));

	m_pShader->MapData(pGraphics, m_pShaderParameters);

	m_pModel->Draw(pGraphics);
}

void AnimatedGameObject::SetAnimation(unsigned int animationIndex)
{
	m_AnimationIndex = animationIndex;
	m_Time = 0;
}

void AnimatedGameObject::SetColor(const XMFLOAT4 &color)
{
	m_pShaderParameters->material.m_Color = color;
}

void AnimatedGameObject::SetTexture(Texture * const pTexture)
{
	m_pShaderParameters->pTexture = pTexture;
}

void AnimatedGameObject::SetSampler(Sampler * const pSampler)
{
	m_pShaderParameters->pSampler = pSampler;
}

void AnimatedGameObject::SetMaterial(const Material &material)
{
	m_pShaderParameters->material = material;
}

void AnimatedGameObject::UseColor(const bool value)
{
	m_pShaderParameters->useColor = value;
}

void AnimatedGameObject::UseTexture(const bool value)
{
	m_pShaderParameters->useTexture = value;
}

void AnimatedGameObject::UseLight(const bool value)
{
	m_pShaderParameters->useLight = value;
}

bool AnimatedGameObject::UseColor() const
{
	return m_pShaderParameters->useColor;
}

bool AnimatedGameObject::UseTexture() const
{
	return m_pShaderParameters->useTexture;
}

bool AnimatedGameObject::UseLight() const
{
	return m_pShaderParameters->useLight;
}

void AnimatedGameObject::SetModel(BDVModel * const pModel)
{
	m_pModel = pModel;

	m_pToParent = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
	m_pToRoot = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
	m_pToWorld = (XMFLOAT4X4 *)_aligned_malloc(pModel->GetNumJoints() * sizeof(XMFLOAT4X4), 16);
}

void AnimatedGameObject::TransformOnHardware(bool value)
{
	m_pShaderParameters->transformOnHardware = value;
}

void AnimatedGameObject::Interpolate()
{
	BDVModel *pBDVModel = ((BDVModel *)m_pModel);
	Animation *pAnim = pBDVModel->GetAnimation(m_AnimationIndex);
	int numJoints = pBDVModel->GetNumJoints();

	XMVECTOR s0;
	XMVECTOR s1;
			   
	XMVECTOR t0;
	XMVECTOR t1;
			   
	XMVECTOR q0;
	XMVECTOR q1;

	XMVECTOR S;
	XMVECTOR T;
	XMVECTOR Q;

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < numJoints; i++)
	{
		if (m_Time <= pAnim->pAnimSamples[0].time)
		{
			S = XMLoadFloat4(&pAnim->pAnimSamples[0].pJointAnimations[i].s);
			T = XMLoadFloat4(&pAnim->pAnimSamples[0].pJointAnimations[i].t);
			Q = XMLoadFloat4(&pAnim->pAnimSamples[0].pJointAnimations[i].q);

			XMStoreFloat4x4(&m_pToParent[i], XMMatrixAffineTransformation(S, zero, Q, T));
		}
		else
		{
			for (UINT j = 0; j < pAnim->numAnimSamples - 1; j++)
			{
				AnimSample *pS1 = &pAnim->pAnimSamples[j];
				AnimSample *pS2 = &pAnim->pAnimSamples[j + 1];

				if (m_Time >= pS1->time && m_Time <= pS2->time)
				{
					float lerpPercent = (m_Time - pS1->time) / (pS2->time - pS1->time);

					s0 = XMLoadFloat4(&pS1->pJointAnimations[i].s);
					s1 = XMLoadFloat4(&pS2->pJointAnimations[i].s);

					t0 = XMLoadFloat4(&pS1->pJointAnimations[i].t);
					t1 = XMLoadFloat4(&pS2->pJointAnimations[i].t);

					q0 = XMLoadFloat4(&pS1->pJointAnimations[i].q);
					q1 = XMLoadFloat4(&pS2->pJointAnimations[i].q);

					S = XMVectorLerp(s0, s1, lerpPercent);
					T = XMVectorLerp(t0, t1, lerpPercent);
					
					// Comment out Slerp and uncomment Lerp for speed
					//Q = XMVectorLerp(q0, q1, lerpPercent);
					Q = XMQuaternionSlerp(q0, q1, lerpPercent);

					XMStoreFloat4x4(&m_pToParent[i], XMMatrixAffineTransformation(S, zero, Q, T));

					break;
				}
			}
		}
	}

	Joint *pJoints = pBDVModel->GetJoints();

	m_pToRoot[0] = m_pToParent[0];

	for (int i = 1; i < numJoints; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&m_pToParent[i]);

		int parentIndex = pJoints[i].parentIndex;
		XMMATRIX parentToRoot = XMLoadFloat4x4(&m_pToRoot[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&m_pToRoot[i], toRoot);
	}

	for (int i = 0; i < numJoints; ++i)
	{
		XMMATRIX invBindPose = XMLoadFloat4x4(&pJoints[i].invBindPose);
		XMMATRIX toRoot = XMLoadFloat4x4(&m_pToRoot[i]);
		XMStoreFloat4x4(&m_pToWorld[i], XMMatrixMultiply(invBindPose, toRoot));
	}
}

void AnimatedGameObject::InterpolateHardware()
{
	BDVModel *pBDVModel = ((BDVModel *)m_pModel);
	Animation *pAnim = pBDVModel->GetAnimation(m_AnimationIndex);

	m_pShaderParameters->pInterpolationData->numJoints = pBDVModel->GetNumJoints();

	m_pShaderParameters->pInterpolationData->pJointsBuffer = pBDVModel->GetJointsBuffer();

	if (m_Time <= pAnim->pAnimSamples[0].time)
	{
		m_pShaderParameters->pInterpolationData->interpolate = 0U;
		m_pShaderParameters->pInterpolationData->lerp = 0.0f;

		m_pShaderParameters->pInterpolationData->pBeginAnimBuffer = pBDVModel->GetAnimSampleBuffers(m_AnimationIndex, 0);
		m_pShaderParameters->pInterpolationData->pEndAnimBuffer = pBDVModel->GetAnimSampleBuffers(m_AnimationIndex, 0);
	}
	else
	{
		AnimSample *pS1 = nullptr;
		AnimSample *pS2 = nullptr;

		for (UINT j = 0; j < pAnim->numAnimSamples - 1; j++)
		{
			pS1 = &pAnim->pAnimSamples[j];
			pS2 = &pAnim->pAnimSamples[j + 1];

			if (m_Time >= pS1->time && m_Time <= pS2->time)
			{
				m_pShaderParameters->pInterpolationData->interpolate = 1U;
				m_pShaderParameters->pInterpolationData->lerp = (m_Time - pS1->time) / (pS2->time - pS1->time);

				m_pShaderParameters->pInterpolationData->pBeginAnimBuffer = pBDVModel->GetAnimSampleBuffers(m_AnimationIndex, j);
				m_pShaderParameters->pInterpolationData->pEndAnimBuffer = pBDVModel->GetAnimSampleBuffers(m_AnimationIndex, j + 1);

				break;
			}
		}
	}
}

int AnimatedGameObject::GetCurrentAnimationIndex() const
{
	return m_AnimationIndex;
}