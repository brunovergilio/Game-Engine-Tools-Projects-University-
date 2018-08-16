#ifndef FBXEXPORTER_H
#define FBXEXPORTER_H

#include <fbxsdk.h>
#include "BVModel.h"

struct ControlPointToVertex
{
	int ctrlPointIndex;
	std::vector<int> vertexIndices;

	ControlPointToVertex() : ctrlPointIndex(0) { }
};
typedef std::vector<ControlPointToVertex> ControlPointToVertexList;

class FBXExporter
{
public:
	FBXExporter();
	~FBXExporter();

	int Process(char * const pArgs[], const int numArgs);

private:
	bool InitFBX(const char * const pFilename);
	void ShutdownFBX();

	// Process Vertices / Indices
	bool ProcessMesh(const char * const pInFilename, const char * const pOutFilename);

	void ProcessPosition(const FbxVector4 * const pControlPoints, const int controlPointIndex, Vertex & vertex);
	void ProcessUV(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex);
	void ProcessNormal(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex);
	void ProcessTangent(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex);
	void ProcessBitangent(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex);

	void ComputeTangentBitangent();

	// Process texture names (only diffuse for now)
	void ProcessTextures(FbxMesh * const pMesh);

	void ProcessJoints();
	void ProcessJointsRecursive(FbxNode * const pNode, const int parentIndex);

	void ProcessAnimations();
	void ProcessAnimationSamples(FbxAnimStack * const pAnimStack, const int numSamples, const int animStackIndex);
	void ProcessAnimationSamplesRecursive(FbxAnimLayer * const pAnimLayer, FbxNode * const pNode, const FbxTime &sampleTime, const int animStackIndex, const int animSampleIndex);

	void ProcessSkinData(FbxMesh * const pMesh);
	int FindJointIndex(const char * const pJointName);

	void UpdateVertexOptions();
	void ConvertAxisSystem();
	void CompressAnimation();

	bool WriteToFile(const char * const pOutFilename);

	bool ParseArgs(char * const pArgs[], const int numArgs);
	void ShowFormat();

	// Not needed
	FBXExporter(const FBXExporter &other) = delete;
	FBXExporter & operator=(const FBXExporter &other) = delete;

private:
	FbxManager *m_pManager;
	FbxScene *m_pScene;

	ModelSettings m_Options;

	VertexList m_VertexList;
	IndexList m_IndexList;
	TextureList m_TextureList;
	JointList m_JointList;
	AnimationList m_AnimationList;

	ControlPointToVertexList m_ControlPointToVertexList;
};

#endif