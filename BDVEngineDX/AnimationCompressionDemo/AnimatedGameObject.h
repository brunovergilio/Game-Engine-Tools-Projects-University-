#ifndef ANIMATEDGAMEOBJECT_H
#define ANIMATEDGAMEOBJECT_H

#include "GameObject.h"
#include "AnimatedShader.h"
#include "BDVModel.h"

class AnimatedGameObject : public GameObject
{
public:
	AnimatedGameObject();
	AnimatedGameObject(BDVModel * const pModel, AnimatedShader * const pShader);
	AnimatedGameObject(BDVModel * const pModel, AnimatedShader * const pShader, const XMFLOAT4 &color, Texture * const pTexture, Sampler * const pSampler, const Material &material);
	virtual ~AnimatedGameObject();

	void Update(const float dt) override;
	void Render(const Graphics * const pGraphics, Camera * const pCamera) override;

	void SetAnimation(unsigned int animationIndex);

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

	int GetCurrentAnimationIndex() const;

private:
	void Interpolate();
	void InterpolateHardware();

	// Not needed
	AnimatedGameObject(const AnimatedGameObject &) = delete;
	AnimatedGameObject & operator = (const AnimatedGameObject &) = delete;

private:
	AnimatedShaderParameters *m_pShaderParameters;

	float m_Time;

	unsigned int m_AnimationIndex;

	XMFLOAT4X4 *m_pToParent;
	XMFLOAT4X4 *m_pToRoot;
	XMFLOAT4X4 *m_pToWorld;
};

#endif