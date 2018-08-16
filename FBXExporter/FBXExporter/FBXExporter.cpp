#include "FBXExporter.h"
#include <cstdio>
#include <algorithm>

const float MATH_PI_180 = 0.0174532925f;

FBXExporter::FBXExporter()
	: m_pManager(nullptr), m_pScene(nullptr)
{
}

FBXExporter::~FBXExporter()
{
}

int FBXExporter::Process(char * const pArgs[], const int numArgs)
{
	int result = 0;

	if (ParseArgs(pArgs, numArgs))
	{
		printf("Processing file...\n\n");

		ProcessMesh(pArgs[1], pArgs[2]);

		printf("File \"%s\" created successfully.\n", pArgs[2]);
	}
	else
	{
		ShowFormat();

		result = 1;
	}

	return result;
}

bool FBXExporter::InitFBX(const char * const pFilename)
{
	m_pManager = FbxManager::Create();
	if (!m_pManager)
	{
		return false;
	}

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* pIOSettings = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(pIOSettings);

	//Load plugins from the executable directory (optional)
	FbxString path = FbxGetApplicationDirectory();
	m_pManager->LoadPluginsDirectory(path.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	m_pScene = FbxScene::Create(m_pManager, "3D Model Scene");
	if (!m_pScene)
	{
		return false;
	}

	FbxImporter* pImporter = FbxImporter::Create(m_pManager, "");
	if (!pImporter->Initialize(pFilename, -1, m_pManager->GetIOSettings()))
	{
		return false;
	}
	else
	{
		if (!pImporter->Import(m_pScene))
		{
			return false;
		}
	}

	pImporter->Destroy();

	return true;
}

void FBXExporter::ShutdownFBX()
{
	if (m_pScene)
	{
		m_pScene->Destroy(true);
	}

	if (m_pManager)
	{
		m_pManager->Destroy();
	}
}

bool FBXExporter::ProcessMesh(const char * const pInFilename, const char * const pOutFilename)
{
	if (!InitFBX(pInFilename))
	{
		return false;
	}

	FbxGeometryConverter lGeomConverter(m_pManager);
	lGeomConverter.Triangulate(m_pScene, true);

	FbxNode* pNode = nullptr;
	FbxNode* pTmp = nullptr;
	FbxNodeAttribute *pNodeAttribute = nullptr;
	for (int i = 0; i < m_pScene->GetRootNode()->GetChildCount(); i++)
	{
		pTmp = m_pScene->GetRootNode()->GetChild(i);
		pNodeAttribute = pTmp->GetNodeAttribute();

		if (pNodeAttribute && pNodeAttribute->GetAttributeType() && pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			pNode = pTmp;
			break;
		}
	}

	if (pNode)
	{
		FbxMesh* pMesh = pNode->GetMesh();
		pMesh->GenerateNormals();
		pMesh->GenerateTangentsDataForAllUVSets();

		m_ControlPointToVertexList.resize(pMesh->GetControlPointsCount());

		FbxVector4* pControlPoints = pMesh->GetControlPoints();

		m_VertexList.reserve(pMesh->GetPolygonVertexCount());

		m_IndexList.reserve(pMesh->GetPolygonCount() * 3);

		Vertex vertex;
		int controlPointIndex = 0;
		int vertexIndex = 0;

		for (int i = 0; i < pMesh->GetPolygonCount(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				controlPointIndex = pMesh->GetPolygonVertex(i, j);

				ProcessPosition(pControlPoints, controlPointIndex, vertex);
				ProcessUV(pMesh, controlPointIndex, vertexIndex, vertex);
				ProcessNormal(pMesh, controlPointIndex, vertexIndex, vertex);
				ProcessTangent(pMesh, controlPointIndex, vertexIndex, vertex);
				ProcessBitangent(pMesh, controlPointIndex, vertexIndex, vertex);

				m_ControlPointToVertexList[controlPointIndex].ctrlPointIndex = controlPointIndex;
				m_ControlPointToVertexList[controlPointIndex].vertexIndices.push_back(m_VertexList.size());

				m_VertexList.push_back(vertex);

				m_IndexList.push_back(vertexIndex++);
			}
		}

		ProcessTextures(pMesh);
	}

	ProcessJoints();
	ProcessAnimations();
	if (pNode)
	{
		ProcessSkinData(pNode->GetMesh());
	}

	UpdateVertexOptions();

	ShutdownFBX();

	return WriteToFile(pOutFilename);
}

void FBXExporter::ProcessPosition(const FbxVector4 * const pControlPoints, const int controlPointIndex, Vertex & vertex)
{
	vertex.x = (float)pControlPoints[controlPointIndex][0];
	vertex.y = (float)pControlPoints[controlPointIndex][1];
	vertex.z = (float)pControlPoints[controlPointIndex][2];
}

void FBXExporter::ProcessUV(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex)
{
	if (pMesh->GetElementUVCount() > 0)
	{
		FbxGeometryElementUV *pUV = pMesh->GetElementUV(0);

		switch (pUV->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			switch (pUV->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.u = (float)pUV->GetDirectArray().GetAt(controlPointIndex)[0];
				vertex.v = (float)pUV->GetDirectArray().GetAt(controlPointIndex)[1];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int uvIndex = pUV->GetIndexArray().GetAt(controlPointIndex);
				vertex.u = (float)pUV->GetDirectArray().GetAt(uvIndex)[0];
				vertex.v = (float)pUV->GetDirectArray().GetAt(uvIndex)[1];
				break;
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			switch (pUV->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.u = (float)pUV->GetDirectArray().GetAt(vertexIndex)[0];
				vertex.v = (float)pUV->GetDirectArray().GetAt(vertexIndex)[1];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int uvIndex = pUV->GetIndexArray().GetAt(vertexIndex);
				vertex.u = (float)pUV->GetDirectArray().GetAt(uvIndex)[0];
				vertex.v = (float)pUV->GetDirectArray().GetAt(uvIndex)[1];
				break;
			}
			break;
		}
	}
}

void FBXExporter::ProcessNormal(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex)
{
	if (pMesh->GetElementNormalCount() > 0)
	{
		FbxGeometryElementNormal *pNormal = pMesh->GetElementNormal(0);

		switch (pNormal->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			switch (pNormal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.nx = (float)pNormal->GetDirectArray().GetAt(controlPointIndex)[0];
				vertex.ny = (float)pNormal->GetDirectArray().GetAt(controlPointIndex)[1];
				vertex.nz = (float)pNormal->GetDirectArray().GetAt(controlPointIndex)[2];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int normalIndex = pNormal->GetIndexArray().GetAt(controlPointIndex);
				vertex.nx = (float)pNormal->GetDirectArray().GetAt(normalIndex)[0];
				vertex.ny = (float)pNormal->GetDirectArray().GetAt(normalIndex)[1];
				vertex.nz = (float)pNormal->GetDirectArray().GetAt(normalIndex)[2];
				break;
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			switch (pNormal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.nx = (float)pNormal->GetDirectArray().GetAt(vertexIndex)[0];
				vertex.ny = (float)pNormal->GetDirectArray().GetAt(vertexIndex)[1];
				vertex.nz = (float)pNormal->GetDirectArray().GetAt(vertexIndex)[2];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int normalIndex = pNormal->GetIndexArray().GetAt(vertexIndex);
				vertex.nx = (float)pNormal->GetDirectArray().GetAt(normalIndex)[0];
				vertex.ny = (float)pNormal->GetDirectArray().GetAt(normalIndex)[1];
				vertex.nz = (float)pNormal->GetDirectArray().GetAt(normalIndex)[2];
				break;
			}
			break;
		}
	}
}

void FBXExporter::ProcessTangent(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex)
{
	if (pMesh->GetElementTangentCount() > 0)
	{
		FbxGeometryElementTangent *pTangent = pMesh->GetElementTangent(0);

		switch (pTangent->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			switch (pTangent->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.tx = (float)pTangent->GetDirectArray().GetAt(controlPointIndex)[0];
				vertex.ty = (float)pTangent->GetDirectArray().GetAt(controlPointIndex)[1];
				vertex.tz = (float)pTangent->GetDirectArray().GetAt(controlPointIndex)[2];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int tangentIndex = pTangent->GetIndexArray().GetAt(controlPointIndex);
				vertex.tx = (float)pTangent->GetDirectArray().GetAt(tangentIndex)[0];
				vertex.ty = (float)pTangent->GetDirectArray().GetAt(tangentIndex)[1];
				vertex.tz = (float)pTangent->GetDirectArray().GetAt(tangentIndex)[2];
				break;
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			switch (pTangent->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.tx = (float)pTangent->GetDirectArray().GetAt(vertexIndex)[0];
				vertex.ty = (float)pTangent->GetDirectArray().GetAt(vertexIndex)[1];
				vertex.tz = (float)pTangent->GetDirectArray().GetAt(vertexIndex)[2];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int tangentIndex = pTangent->GetIndexArray().GetAt(vertexIndex);
				vertex.tx = (float)pTangent->GetDirectArray().GetAt(tangentIndex)[0];
				vertex.ty = (float)pTangent->GetDirectArray().GetAt(tangentIndex)[1];
				vertex.tz = (float)pTangent->GetDirectArray().GetAt(tangentIndex)[2];
				break;
			}
			break;
		}
	}
}

void FBXExporter::ProcessBitangent(FbxMesh * const pMesh, const int controlPointIndex, const int vertexIndex, Vertex & vertex)
{
	if (pMesh->GetElementBinormalCount() > 0)
	{
		FbxGeometryElementBinormal *pBitangent = pMesh->GetElementBinormal(0);

		switch (pBitangent->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			switch (pBitangent->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.bx = (float)pBitangent->GetDirectArray().GetAt(controlPointIndex)[0];
				vertex.by = (float)pBitangent->GetDirectArray().GetAt(controlPointIndex)[1];
				vertex.bz = (float)pBitangent->GetDirectArray().GetAt(controlPointIndex)[2];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int bitangentIndex = pBitangent->GetIndexArray().GetAt(controlPointIndex);
				vertex.bx = (float)pBitangent->GetDirectArray().GetAt(bitangentIndex)[0];
				vertex.by = (float)pBitangent->GetDirectArray().GetAt(bitangentIndex)[1];
				vertex.bz = (float)pBitangent->GetDirectArray().GetAt(bitangentIndex)[2];
				break;
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			switch (pBitangent->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				vertex.bx = (float)pBitangent->GetDirectArray().GetAt(vertexIndex)[0];
				vertex.by = (float)pBitangent->GetDirectArray().GetAt(vertexIndex)[1];
				vertex.bz = (float)pBitangent->GetDirectArray().GetAt(vertexIndex)[2];
				break;

			case FbxGeometryElement::eIndexToDirect:
				int bitangentIndex = pBitangent->GetIndexArray().GetAt(vertexIndex);
				vertex.bx = (float)pBitangent->GetDirectArray().GetAt(bitangentIndex)[0];
				vertex.by = (float)pBitangent->GetDirectArray().GetAt(bitangentIndex)[1];
				vertex.bz = (float)pBitangent->GetDirectArray().GetAt(bitangentIndex)[2];
				break;
			}
			break;
		}
	}
}

void FBXExporter::ComputeTangentBitangent()
{
	float s1, s2, t1, t2;
	Float3 e1, e2;

	float oneOverDet;
	Float3 tangent;
	Float3 bitangent;
	Float3 normal;

	FbxVector4 tv, bv, nv;

	Vertex *pCurrVertices;
	for (unsigned int i = 0; i < m_IndexList.size(); i += 3)
	{
		pCurrVertices = &m_VertexList[m_IndexList[i]];

		// Q1 = P1 - P0
		// Q2 = P2 - P0
		e1 = Float3(pCurrVertices[1].x - pCurrVertices[0].x, pCurrVertices[1].y - pCurrVertices[0].y, pCurrVertices[1].z - pCurrVertices[0].z);
		e2 = Float3(pCurrVertices[2].x - pCurrVertices[0].x, pCurrVertices[2].y - pCurrVertices[0].y, pCurrVertices[2].z - pCurrVertices[0].z);

		// s1 = u1 - u0
		// s2 = u2 - u0
		s1 = pCurrVertices[1].u - pCurrVertices[0].u;
		s2 = pCurrVertices[2].u - pCurrVertices[0].u;

		// t1 = v1 - v0
		// t2 = v2 - v0
		t1 = pCurrVertices[1].v - pCurrVertices[0].v;
		t2 = pCurrVertices[2].v - pCurrVertices[0].v;

		// [s1 t1] (-1) -- [ t2 -t1] * ________1________
		// [s2 t2]      -- [-s2  s1]   s1 * t2 - t1 * s2
		oneOverDet = 1.0f / (s1 * t2 - t1 * s2);

		normal = *(Float3 *)(&pCurrVertices[0].nx);
		tangent = Float3(oneOverDet * (t2 * e1.x + -t1 * e2.x), oneOverDet * (t2 * e1.y + -t1 * e2.y), oneOverDet * (t2 * e1.z + -t1 * e2.z));
		bitangent = Float3(oneOverDet * (-s2 * e1.x + s1 * e2.x), oneOverDet * (-s2 * e1.y + s1 * e2.y), oneOverDet * (-s2 * e1.z + s1 * e2.z));

		tv.Set(tangent.x, tangent.y, tangent.z, 0.0f);
		bv.Set(bitangent.x, bitangent.y, bitangent.z, 0.0f);
		nv.Set(normal.x, normal.y, normal.z, 0.0f);

		tv = tv - nv * (nv.DotProduct(tv));
		tv.Normalize();

		bv = bv - nv * (nv.DotProduct(bv)) - tv * (tv.DotProduct(bv));
		bv.Normalize();

		pCurrVertices[0].tx = pCurrVertices[1].tx = pCurrVertices[2].tx = (float)tv.mData[0];
		pCurrVertices[0].ty = pCurrVertices[1].ty = pCurrVertices[2].ty = (float)tv.mData[1];
		pCurrVertices[0].tz = pCurrVertices[1].tz = pCurrVertices[2].tz = (float)tv.mData[2];

		pCurrVertices[0].bx = pCurrVertices[1].bx = pCurrVertices[2].bx = (float)bv.mData[0];
		pCurrVertices[0].by = pCurrVertices[1].by = pCurrVertices[2].by = (float)bv.mData[1];
		pCurrVertices[0].bz = pCurrVertices[1].bz = pCurrVertices[2].bz = (float)bv.mData[2];
	}
}

void FBXExporter::ProcessTextures(FbxMesh * const pMesh)
{
	FbxNode *pNode = pMesh->GetNode();

	for (int i = 0; i < pNode->GetSrcObjectCount<FbxSurfaceMaterial>(); i++)
	{
		FbxSurfaceMaterial *pMaterial = (FbxSurfaceMaterial*)pNode->GetSrcObject<FbxSurfaceMaterial>(i);
		if (pMaterial)
		{
			// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
			FbxProperty property = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

			// Not using layered textures
			// Directly get textures
			for (int j = 0; j < property.GetSrcObjectCount<FbxTexture>(); j++)
			{
				const FbxTexture* pTexture = FbxCast<FbxTexture>(property.GetSrcObject<FbxTexture>(j));
				// Then, you can get all the properties of the texture, include its name
				const char * pTextureName = pTexture->GetName();

				Texture texture;
#pragma warning(disable : 4996)
				strcpy(texture.textureName, pTextureName);
#pragma warning(default : 4996)

				m_TextureList.push_back(texture);
			}
		}
	}
}

void FBXExporter::ProcessJoints()
{
	FbxNode *pNode = m_pScene->GetRootNode();

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		ProcessJointsRecursive(pNode->GetChild(i), -1);
	}
}

void FBXExporter::ProcessJointsRecursive(FbxNode * const pNode, const int parentIndex)
{
	int index = parentIndex;

	FbxNodeAttribute *pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute && pNodeAttribute->GetAttributeType() && pNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
	{
		m_JointList.push_back(Joint(pNode->GetName(), parentIndex));

		index = m_JointList.size() - 1;
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		ProcessJointsRecursive(pNode->GetChild(i), index);
	}
}

void FBXExporter::ProcessAnimations()
{
	int startFrame;
	int endFrame;
	FbxTimeSpan timeSpan;

	for (int i = 0; i < m_pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* pAnimStack = m_pScene->GetSrcObject<FbxAnimStack>(i);
		m_pScene->SetCurrentAnimationStack(pAnimStack);

		FbxTakeInfo *pTakeInfo = m_pScene->GetTakeInfo(pAnimStack->GetName());
		if (pTakeInfo)
		{
			startFrame = (int)pTakeInfo->mLocalTimeSpan.GetStart().GetFrameCount(FbxTime::eFrames30);
			endFrame = (int)pTakeInfo->mLocalTimeSpan.GetStop().GetFrameCount(FbxTime::eFrames30);
		}
		else
		{
			m_pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeSpan);
			startFrame = (int)timeSpan.GetStart().GetFrameCount(FbxTime::eFrames30);
			endFrame = (int)timeSpan.GetStop().GetFrameCount(FbxTime::eFrames30);
		}

		Animation animation;
		strcpy_s(animation.name, pAnimStack->GetName());
		m_AnimationList.push_back(animation);
		m_AnimationList[i].animSamples.reserve(endFrame - startFrame);

		ProcessAnimationSamples(pAnimStack, endFrame - startFrame, i);
	}
}

void FBXExporter::ProcessAnimationSamples(FbxAnimStack * const pAnimStack, const int numSamples, const int animStackIndex)
{
	// I'm considering only the first animation layer of each animation
	FbxAnimLayer *pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>();

	FbxTime sampleTime;

	for (int i = 0; i < numSamples; i++)
	{
		sampleTime.SetTime(0, 0, 0, i, 0, 0, FbxTime::eFrames30);

		AnimSample animSample;
		animSample.time = (float)sampleTime.GetMilliSeconds() * 0.001f;

		m_AnimationList[animStackIndex].animSamples.push_back(animSample);

		ProcessAnimationSamplesRecursive(pAnimLayer, m_pScene->GetRootNode(), sampleTime, animStackIndex, i);
	}
}

void FBXExporter::ProcessAnimationSamplesRecursive(FbxAnimLayer * const pAnimLayer, FbxNode * const pNode, const FbxTime &sampleTime, const int animStackIndex, const int animSampleIndex)
{
	if (pNode->GetNodeAttribute() && pNode->GetNodeAttribute()->GetAttributeType() && pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxAMatrix matrix = pNode->EvaluateLocalTransform(sampleTime);

		FbxVector4 t = matrix.GetT();
		FbxQuaternion q = matrix.GetQ();
		FbxVector4 s = matrix.GetS();

		JointAnim jointAnim;
		jointAnim.t = Float4((float)t[0], (float)t[1], (float)t[2], 0.0f);
		jointAnim.s = Float4((float)s[0], (float)s[1], (float)s[2], 0.0f);
		jointAnim.q = Float4((float)q[0], (float)q[1], (float)q[2], (float)q[3]);

		m_AnimationList[animStackIndex].animSamples[animSampleIndex].jointAnimations.push_back(jointAnim);
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		ProcessAnimationSamplesRecursive(pAnimLayer, pNode->GetChild(i), sampleTime, animStackIndex, animSampleIndex);
	}
}

void FBXExporter::ProcessSkinData(FbxMesh * const pMesh)
{
	FbxSkin *pSkin = (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	if (pSkin)
	{
		std::vector<int> numWeightsList;
		numWeightsList.resize(m_VertexList.size());

		int numClusters = pSkin->GetClusterCount();

		FbxCluster *pCluster = nullptr;
		int jointIndex = -1;
		for (int i = 0; i < numClusters; i++)
		{
			pCluster = pSkin->GetCluster(i);
			FbxNode *pNode = pCluster->GetLink();

			jointIndex = FindJointIndex(pNode->GetName());
			if (jointIndex > -1)
			{
				// Usually Identity, but the SDK states that it should also be used
				FbxAMatrix geometryMatrix(pNode->GetGeometricTranslation(FbxNode::eSourcePivot), pNode->GetGeometricRotation(FbxNode::eSourcePivot), pNode->GetGeometricScaling(FbxNode::eSourcePivot));

				// Node Transform
				FbxAMatrix nodeTransform;
				pCluster->GetTransformMatrix(nodeTransform);

				// Link Transform - This should be the bind pose
				FbxAMatrix linkTransform;
				pCluster->GetTransformLinkMatrix(linkTransform);

				FbxAMatrix bindPoseMatrix = linkTransform.Inverse() * nodeTransform * geometryMatrix;

				m_JointList[jointIndex].invBindPose.row[0] = Float4((float)bindPoseMatrix[0][0], (float)bindPoseMatrix[0][1], (float)bindPoseMatrix[0][2], (float)bindPoseMatrix[0][3]);
				m_JointList[jointIndex].invBindPose.row[1] = Float4((float)bindPoseMatrix[1][0], (float)bindPoseMatrix[1][1], (float)bindPoseMatrix[1][2], (float)bindPoseMatrix[1][3]);
				m_JointList[jointIndex].invBindPose.row[2] = Float4((float)bindPoseMatrix[2][0], (float)bindPoseMatrix[2][1], (float)bindPoseMatrix[2][2], (float)bindPoseMatrix[2][3]);
				m_JointList[jointIndex].invBindPose.row[3] = Float4((float)bindPoseMatrix[3][0], (float)bindPoseMatrix[3][1], (float)bindPoseMatrix[3][2], (float)bindPoseMatrix[3][3]);
			}

			int numIndices = pCluster->GetControlPointIndicesCount();
			int *pIndices = pCluster->GetControlPointIndices();
			double *pWeights = pCluster->GetControlPointWeights();

			for (int j = 0; j < numIndices; j++)
			{
				// A control point might be referenced in more than one vertex
				for (int k = 0; k < (int)m_ControlPointToVertexList[pIndices[j]].vertexIndices.size(); k++)
				{
					int weightIndex = numWeightsList[m_ControlPointToVertexList[pIndices[j]].vertexIndices[k]];
					if (weightIndex < 4)
					{
						m_VertexList[m_ControlPointToVertexList[pIndices[j]].vertexIndices[k]].joints[weightIndex] = jointIndex;
						m_VertexList[m_ControlPointToVertexList[pIndices[j]].vertexIndices[k]].weights[weightIndex] = (float)pWeights[j];
						numWeightsList[m_ControlPointToVertexList[pIndices[j]].vertexIndices[k]]++;
					}
				}
			}
		}

		for (int i = 0; i < (int)m_VertexList.size(); i++)
		{
			float sum = m_VertexList[i].weights[0] + m_VertexList[i].weights[1] + m_VertexList[i].weights[2] + m_VertexList[i].weights[3];
			for (int j = 0; j < 4; j++)
			{
				m_VertexList[i].weights[j] /= sum;
			}
		}
	}
}

int FBXExporter::FindJointIndex(const char * const pJointName)
{
	int index = -1;
	for (int i = 0; i < (int)m_JointList.size(); i++)
	{
		if (!strcmp(m_JointList[i].name, pJointName))
		{
			index = i;
			break;
		}
	}

	return index;
}


void FBXExporter::UpdateVertexOptions()
{
	// Axis Conversion
	AxisType axisType = AxisType::RH;
	if (m_pScene->GetGlobalSettings().GetAxisSystem().GetCoorSystem() == FbxAxisSystem::eLeftHanded)
	{
		axisType = AxisType::LH;
	}

	if (axisType != m_Options.axisType)
	{
		ConvertAxisSystem();
	}

	// Animation Data Compression
	if (m_Options.compressionType == CompressionType::COMPRESSED && m_AnimationList.size() > 0)
	{
		CompressAnimation();
	}
}

void FBXExporter::ConvertAxisSystem()
{
	for (int i = 0; i < (int)m_VertexList.size(); i++)
	{
		// Negate Z
		m_VertexList[i].z = -m_VertexList[i].z;
		m_VertexList[i].nz = -m_VertexList[i].nz;
		m_VertexList[i].tz = -m_VertexList[i].tz;
		m_VertexList[i].bz = -m_VertexList[i].bz;

		// DirectX's texture coords start at the top left corner
		// OpenGL's texture coords start at the bottom left corner
		m_VertexList[i].v = 1.0f - m_VertexList[i].v;
	}

	for (int i = 0; i < (int)m_IndexList.size(); i += 3)
	{
		m_IndexList[i] ^= m_IndexList[i + 2];
		m_IndexList[i + 2] ^= m_IndexList[i];
		m_IndexList[i] ^= m_IndexList[i + 2];
	}
}

void FBXExporter::CompressAnimation()
{
	unsigned int i1, i2, iMid;

	AnimSample *pSamples;
	JointAnim *pAnim1, *pAnim2, *pAnimMiddle;

	FbxQuaternion q1, q2, q;
	float lerpPercent;
	float lerpAngle, quatAngle;
	float errorThreshold = m_Options.errorThreshold * MATH_PI_180;

	std::vector<unsigned int> samplesToRemove;

	bool removeSample;

	for (unsigned int i = 0; i < m_AnimationList.size(); i++)
	{
		removeSample = true;
		i1 = 0; i2 = 2; iMid = 1;
		pSamples = &m_AnimationList[i].animSamples[0];

		while (i2 < m_AnimationList[i].animSamples.size())
		{
			for (unsigned int j = i1 + 1; j < i2; j++)
			{
				lerpPercent = (pSamples[j].time - pSamples[i1].time) / (pSamples[i2].time - pSamples[i1].time);
				for (unsigned int k = 0; k < pSamples->jointAnimations.size(); k++)
				{
					pAnim1 = &pSamples[i1].jointAnimations[k];
					pAnim2 = &pSamples[i2].jointAnimations[k];
					pAnimMiddle = &pSamples[j].jointAnimations[k];

					q1.Set(pAnim1->q.x, pAnim1->q.y, pAnim1->q.z, pAnim1->q.w);
					q2.Set(pAnim2->q.x, pAnim2->q.y, pAnim2->q.z, pAnim2->q.w);

					q = q1.Slerp(q2, lerpPercent);

					lerpAngle = 2.0f * acosf((float)q.mData[3]);
					quatAngle = 2.0f * acosf(pAnimMiddle->q.w);

					if (abs(lerpAngle - quatAngle) > errorThreshold)
					{
						removeSample = false;
						break;
					}
				}

				if (!removeSample)
				{
					break;
				}
			}

			if (removeSample)
			{
				samplesToRemove.push_back(iMid);
			}
			else
			{
				i1 = iMid;
				//i1++;
				removeSample = true;
			}
			iMid++;
			i2++;
		}

		if (samplesToRemove.size() > 0)
		{
			std::reverse(samplesToRemove.begin(), samplesToRemove.end());
			for (unsigned int sampleIndex = 0; sampleIndex < samplesToRemove.size(); sampleIndex++)
			{
				m_AnimationList[i].animSamples[samplesToRemove[sampleIndex]].jointAnimations.clear();
				m_AnimationList[i].animSamples.erase(m_AnimationList[i].animSamples.begin() + samplesToRemove[sampleIndex]);
			}

			samplesToRemove.clear();
			m_AnimationList[i].animSamples.shrink_to_fit();
		}
	}
}

bool FBXExporter::WriteToFile(const char * const pOutFilename)
{
	FILE *pFile = nullptr;
#pragma warning (disable : 4996)
	pFile = fopen(pOutFilename, "wb");
#pragma warning (default : 4996)
	if (pFile)
	{
		BVTableOfContents contents;
		contents.optionsOffset = sizeof(BVTableOfContents);
		contents.verticesOffset = contents.optionsOffset + sizeof(ModelSettings);
		contents.indicesOffset = contents.verticesOffset + sizeof(u32) + m_VertexList.size() * sizeof(Vertex);
		contents.texturesOffset = contents.indicesOffset + sizeof(u32) + m_IndexList.size() * sizeof(u32);
		contents.jointsOffset = contents.texturesOffset + sizeof(u32) + m_TextureList.size() * sizeof(Texture);
		contents.animationOffset = contents.jointsOffset + sizeof(u32) + m_JointList.size() * sizeof(Joint);
		fwrite(&contents, sizeof(BVTableOfContents), 1, pFile);

		// Options
		fwrite(&m_Options, sizeof(ModelSettings), 1, pFile);

		// Vertices
		u32 numVertices = m_VertexList.size();
		fwrite(&numVertices, sizeof(u32), 1, pFile);
		if (numVertices > 0)
		{
			fwrite(&m_VertexList[0], sizeof(Vertex) * m_VertexList.size(), 1, pFile);
		}

		// Indices
		u32 numIndices = m_IndexList.size();
		fwrite(&numIndices, sizeof(u32), 1, pFile);
		if (numIndices > 0)
		{
			fwrite(&m_IndexList[0], sizeof(u32) * m_IndexList.size(), 1, pFile);
		}

		// Textures
		u32 numTextures = m_TextureList.size();
		fwrite(&numTextures, sizeof(u32), 1, pFile);
		if (numTextures > 0)
		{
			fwrite(&m_TextureList[0], sizeof(Texture) * m_TextureList.size(), 1, pFile);
		}

		// Joints
		u32 numJoints = m_JointList.size();
		fwrite(&numJoints, sizeof(u32), 1, pFile);
		if (numJoints > 0)
		{
			fwrite(&m_JointList[0], sizeof(Joint) * m_JointList.size(), 1, pFile);
		}

		// Animations
		u32 numAnimations = m_AnimationList.size();
		u32 numAnimSamples;
		u32 numJointAnims;

		fwrite(&numAnimations, sizeof(u32), 1, pFile);
		if (numAnimations > 0)
		{
			for (int i = 0; i < (int)numAnimations; i++)
			{
				fwrite(m_AnimationList[i].name, ANIM_NAME_SIZE, 1, pFile);

				numAnimSamples = m_AnimationList[i].animSamples.size();
				fwrite(&numAnimSamples, sizeof(u32), 1, pFile);

				for (int j = 0; j < (int)m_AnimationList[i].animSamples.size(); j++)
				{
					numJointAnims = m_AnimationList[i].animSamples[j].jointAnimations.size();
					fwrite(&m_AnimationList[i].animSamples[j].time, sizeof(f32), 1, pFile);
					fwrite(&numJointAnims, sizeof(u32), 1, pFile);

					// Write all Joint Animations in a single write, passing the address of the first element
					fwrite(&m_AnimationList[i].animSamples[j].jointAnimations[0], m_AnimationList[i].animSamples[j].jointAnimations.size() * sizeof(JointAnim), 1, pFile);
				}
			}
		}

		fclose(pFile);
	}
	else
	{
		printf("Error creating file\n");
		return false;
	}

	return true;
}

bool FBXExporter::ParseArgs(char * const pArgs[], const int numArgs)
{
	printf("Parsing arguments...\n");

	bool ok = true;
	bool skipNext = false;

	if (numArgs > 2)
	{
		for (int i = 3; i < numArgs; i++)
		{
			if (strstr(pArgs[i], "-xs") || strstr(pArgs[i], "-XS"))
			{
				if (i + 1 >= numArgs)
				{
					printf("Invalid arguments.\n");

					ok = false;
					break;
				}
				else
				{
					if (strstr(pArgs[i + 1], "lh") || strstr(pArgs[i + 1], "LH"))
					{
						m_Options.axisType = AxisType::LH;
						skipNext = true;
					}
					else if (strstr(pArgs[i + 1], "rh") || strstr(pArgs[i + 1], "RH"))
					{
						m_Options.axisType = AxisType::LH;
						skipNext = true;
					}
					else
					{
						printf("Invalid arguments.\n");

						ok = false;
						break;
					}
				}
			}
			if (strstr(pArgs[i], "-cp") || strstr(pArgs[i], "-CP"))
			{
				if (i + 1 >= numArgs)
				{
					printf("Invalid arguments.\n");

					ok = false;
					break;
				}
				else
				{
					m_Options.errorThreshold = strtof(pArgs[i + 1], nullptr);
					if (m_Options.errorThreshold != 0.0f)
					{
						m_Options.compressionType = CompressionType::COMPRESSED;
						skipNext = true;
					}
					else
					{
						printf("Invalid arguments.\n");

						ok = false;
						break;
					}
				}
			}
			else
			{
				if (!skipNext)
				{
					printf("Invalid arguments.\n");

					ok = false;
					break;
				}
				skipNext = false;
			}
		}
	}
	else
	{
		printf("Invalid number of arguments.\n");

		ok = false;
	}

	if (ok)
	{
		printf("Parsing OK.\n");
	}

	printf("\n");

	return ok;
}

void FBXExporter::ShowFormat()
{
	printf("Format:\n\n");
	printf("FBXExporter.exe <inFile> <outFile> <options>\n");
	printf("<inFile>: input file\n");
	printf("<outFile>: output file\n");
	printf("<options>:\n");
	printf("* -xs (LH | RH):  switches the axis system between right and left handed\n");
	printf("* -cp (degrees):  performs keyframe compression with the specified number of degrees as error threshold\n");
}