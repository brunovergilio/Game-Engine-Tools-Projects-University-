#ifndef BDVMODEL_H
#define BDVMODEL_H

#include "Model.h"
#include "Texture.h"

struct BDVTableOfContents
{
	unsigned int optionsOffset;
	unsigned int verticesOffset;
	unsigned int indicesOffset;
	unsigned int texturesOffset;
	unsigned int jointsOffset;
	unsigned int animationOffset;

	BDVTableOfContents() : optionsOffset(0), verticesOffset(0), indicesOffset(0), texturesOffset(0), jointsOffset(0), animationOffset(0) {}
};

enum class AxisType : int
{
	LH,
	RH,
};

enum class CompressionType : int
{
	UNCOMPRESSED,
	COMPRESSED,
};

struct ModelSettings
{
	AxisType axisType;
	CompressionType compressionType;
	float errorThreshold;

	ModelSettings() : axisType(AxisType::RH), compressionType(CompressionType::UNCOMPRESSED) {}
};

// BDV Model Vertex
struct BDVVertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 bitangent;
	unsigned int joints[4];
	float weights[4];

	BDVVertex() { memset(this, 0, sizeof(BDVVertex)); }
};

const i32 TEXTURE_NAME_SIZE = 32;

struct BDVTexture
{
	char textureName[TEXTURE_NAME_SIZE];
};

const i32 JOINT_NAME_SIZE = 32;

struct Joint
{
	XMFLOAT4X4 invBindPose;
	i8 name[JOINT_NAME_SIZE];
	i32 parentIndex;
};

__declspec(align(16)) struct JointData
{
	XMFLOAT4X4A invBindPose;
	i32 parentIndex;
};

struct JointAnim
{
	XMFLOAT4 t;
	XMFLOAT4 s;
	XMFLOAT4 q;

	JointAnim() { memset(this, 0, sizeof(JointAnim)); }
};

struct AnimSample
{
	f32 time;
	u32 numJointAnimations;
	JointAnim *pJointAnimations;
};

const i32 ANIM_NAME_SIZE = 32;

struct Animation
{
	i8 name[ANIM_NAME_SIZE];
	u32 numAnimSamples;
	AnimSample *pAnimSamples;
};

class BDVModel : public Model
{
public:
	BDVModel();
	virtual ~BDVModel();

	bool Initialize(const Graphics * const pGraphics, const char * const pFileName);

	void Draw(const Graphics * const pGraphics) override;

	// Model Settings
	ModelSettings *GetModelSettings() const;

	// Textures
	bool HasTexture() const;
	Texture *GetTexture() const;

	// Joints
	bool HasJoints() const;
	unsigned int GetNumJoints() const;
	Joint *GetJoints() const;

	// Animations
	bool HasAnimations() const;
	unsigned int GetNumAnimations() const;
	Animation *GetAnimation(const unsigned int index) const;

	// For GPU animation
	ID3D11Buffer *GetJointsBuffer() const;
	ID3D11Buffer *GetAnimSampleBuffers(unsigned int animationIndex, unsigned int sampleIndex) const;

private:
	bool CreateAnimationBuffers(const Graphics * const pGraphics);

	// Not needed
	BDVModel(const BDVModel &) = delete;
	BDVModel & operator = (const BDVModel &) = delete;

private:
	ModelSettings *m_pModelSettings;

	Texture *m_pTexture;

	unsigned int m_NumJoints;
	Joint *m_pJoints;

	unsigned int m_NumAnimations;
	Animation *m_pAnimations;

	// For GPU animation
	ComPtr<ID3D11Buffer> m_pJointsBuffer;
	ComPtr<ID3D11Buffer> **m_pAnimSampleBuffers;
};

#endif