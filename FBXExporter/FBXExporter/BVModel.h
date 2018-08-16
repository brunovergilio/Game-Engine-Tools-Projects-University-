#ifndef BVMODEL_H
#define BVMODEL_H

#include <memory>
#include <vector>

//Typedefs
typedef char			   i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef float			   f32;
typedef double			   d64;

// Table of Contents, for easy access of all the data
struct BVTableOfContents
{
	u32 optionsOffset;
	u32 verticesOffset;
	u32 indicesOffset;
	u32 texturesOffset;
	u32 jointsOffset;
	u32 animationOffset;

	BVTableOfContents() : optionsOffset(0), verticesOffset(0), indicesOffset(0), texturesOffset(0), jointsOffset(0), animationOffset(0) {}
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

	ModelSettings() : axisType(AxisType::RH), compressionType(CompressionType::UNCOMPRESSED), errorThreshold(0.0f) {}
};

struct Float2
{
	f32 x, y;

	Float2() { memset(this, 0, sizeof(Float2)); }
	Float2(f32 x, f32 y) : x(x), y(y) {}
};

struct Float3
{
	f32 x, y, z;

	Float3() { memset(this, 0, sizeof(Float3)); }
	Float3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}
};

struct Float4
{
	f32 x, y, z, w;

	Float4() { memset(this, 0, sizeof(Float4)); }
	Float4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}
};

struct Float44
{
	Float4 row[4];
	Float44()
	{
		memset(this, 0, sizeof(Float44));
		row[0].x = 1;
		row[1].y = 1;
		row[2].z = 1;
		row[3].w = 1;
	}
};

// BDV Model Vertex
struct Vertex
{
	f32 x, y, z;
	f32 u, v;
	f32 nx, ny, nz;
	f32 tx, ty, tz;
	f32 bx, by, bz;
	u32 joints[4];
	f32 weights[4];

	Vertex() { memset(this, 0, sizeof(Vertex)); }
};

const i32 TEXTURE_NAME_SIZE = 32;

struct Texture
{
	char textureName[TEXTURE_NAME_SIZE];
};

const i32 JOINT_NAME_SIZE = 32;

struct Joint
{
	Float44 invBindPose;
	i8 name[JOINT_NAME_SIZE];
	i32 parentIndex;

	Joint(const i8 * const name, i32 parentIndex) : parentIndex(parentIndex)
	{
		strcpy_s(this->name, name);
	}
};

struct JointAnim
{
	Float4 t;
	Float4 s;
	Float4 q;

	JointAnim() { memset(this, 0, sizeof(JointAnim)); }
};

struct AnimSample
{
	std::vector<JointAnim> jointAnimations;
	f32 time;
};

const i32 ANIM_NAME_SIZE = 32;

struct Animation
{
	i8 name[ANIM_NAME_SIZE];
	std::vector<AnimSample> animSamples;
};

typedef std::vector<Vertex> VertexList;
typedef std::vector<u32> IndexList;
typedef std::vector<Texture> TextureList;
typedef std::vector<Joint> JointList;
typedef std::vector<Animation> AnimationList;

#endif