#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Utils.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

class GameObject
{
public:
	GameObject();
	GameObject(Model * const pModel, Shader * const pShader);
	virtual ~GameObject();

	void SetModel(Model * const pModel);
	void SetShader(Shader * const pShader);

	virtual void Update(const float dt) = 0;
	virtual void Render(const Graphics * const pGraphics, Camera * const pCamera) = 0;

	void SetWorldMatrix(CXMMATRIX world);

private:
	// Not needed
	GameObject(const GameObject &) = delete;
	GameObject & operator = (const GameObject &) = delete;

protected:
	XMFLOAT4X4 m_WorldMatrix;
	Model *m_pModel;
	Shader *m_pShader;
};

#endif