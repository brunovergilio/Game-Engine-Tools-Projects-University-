#ifndef BasicGameObject_H
#define BasicGameObject_H

#include "GameObject.h"
#include "BasicShader.h"
#include "BDVModel.h"

class BasicGameObject : public GameObject
{
public:
	BasicGameObject();
	BasicGameObject(BDVModel * const pModel, BasicShader * const pShader);
	BasicGameObject(BDVModel * const pModel, BasicShader * const pShader, const XMFLOAT4 &color, Texture * const pTexture, Sampler * const pSampler, const Material &material);
	virtual ~BasicGameObject();

	void Update(const float dt) override;
	void Render(const Graphics * const pGraphics, Camera * const pCamera) override;

	void SetColor(const XMFLOAT4 &color);
	void SetTexture(Texture * const pTexture);
	void SetSampler(Sampler * const pSampler);
	void SetMaterial(const Material &material);
	void SetNormalMap(Texture * const pTexture);

	void UseColor(const bool value);
	void UseTexture(const bool value);
	void UseLight(const bool value);
	void UseNormalMapping(const bool value);

	bool UseColor() const;
	bool UseTexture() const;
	bool UseLight() const;
	bool UseNormalMapping() const;

private:
	// Not needed
	BasicGameObject(const BasicGameObject &) = delete;
	BasicGameObject & operator = (const BasicGameObject &) = delete;

private:
	BasicShaderParameters *m_pShaderParameters;
};

#endif