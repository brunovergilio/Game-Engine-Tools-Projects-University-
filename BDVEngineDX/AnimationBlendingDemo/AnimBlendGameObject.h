#ifndef ANIMBLENDGAMEOBJECT_H
#define ANIMBLENDGAMEOBJECT_H

#include "GameObject.h"
#include "AnimBlendShader.h"
#include "BDVModel.h"

class AnimBlendGameObject : public GameObject
{
public:
	AnimBlendGameObject();
	AnimBlendGameObject(BDVModel * const pModel, AnimBlendShader * const pShader);
	AnimBlendGameObject(BDVModel * const pModel, AnimBlendShader * const pShader, const XMFLOAT4 &color, Texture * const pTexture, Sampler * const pSampler, const Material &material);
	virtual ~AnimBlendGameObject();

	void Update(const float dt) override;
	void Render(const Graphics * const pGraphics, Camera * const pCamera) override;

	void SetAnimation(unsigned int animationIndex);

	// For animation blending
	void FixAnimPosition();
	void BlendAnimation(unsigned int animationIndex);
	bool IsBlending() const;
	void SetFadeTime(const float fadeTime);
	float GetFadeTime() const;

	void SetColor(const XMFLOAT4 &color);
	void SetTexture(Texture * const pTexture);
	void SetSampler(Sampler * const pSampler);
	void SetMaterial(const Material &material);

	void UseColor(const bool value);
	void UseTexture(const bool value);
	void UseLight(const bool value);

	bool UseColor() const;
	bool UseTexture() const;
	bool UseLight() const;

	void SetModel(BDVModel * const pModel);

	void TransformOnHardware(bool value);

private:
	void Interpolate();
	void InterpolateHardware();

	// Not needed
	AnimBlendGameObject(const AnimBlendGameObject &) = delete;
	AnimBlendGameObject & operator = (const AnimBlendGameObject &) = delete;

private:
	AnimBlendShaderParameters *m_pShaderParameters;

	float m_Time;

	unsigned int m_AnimationIndex;

	unsigned int m_AnimationIndex2;

	XMFLOAT4X4 *m_pToParent;
	XMFLOAT4X4 *m_pToRoot;
	XMFLOAT4X4 *m_pToWorld;

	float m_BlendTime;
	float m_FadeTime;
	bool m_IsBlending;
};

#endif