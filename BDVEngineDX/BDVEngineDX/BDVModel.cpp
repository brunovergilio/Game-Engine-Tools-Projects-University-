#include "BDVModel.h"

BDVModel::BDVModel()
	: m_pModelSettings(nullptr), m_pTexture(nullptr), m_NumJoints(0), m_pJoints(nullptr), m_NumAnimations(0), m_pAnimations(nullptr), m_pAnimSampleBuffers(nullptr)
{
}

BDVModel::~BDVModel()
{
	if (m_pModelSettings)
	{
		delete m_pModelSettings;
	}

	if (m_pTexture)
	{
		delete m_pTexture;
	}

	if (m_pJoints)
	{
		delete[] m_pJoints;
	}

	if (m_pAnimations)
	{
		for (unsigned int i = 0; i < m_NumAnimations; i++)
		{
			for (unsigned int j = 0; j < m_pAnimations[i].numAnimSamples; j++)
			{
				delete[] m_pAnimations[i].pAnimSamples[j].pJointAnimations;
			}

			delete[] m_pAnimations[i].pAnimSamples;
		}

		delete[] m_pAnimations;
	}

	for (unsigned int i = 0; i < m_NumAnimations; i++)
	{
		delete[] m_pAnimSampleBuffers[i];
	}
	delete[] m_pAnimSampleBuffers;
}

bool BDVModel::Initialize(const Graphics * const pGraphics, const char * const pFileName)
{
	char pFullFileName[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pFullFileName);
	strcat_s(pFullFileName, MAX_PATH, "\\Data\\Models\\");
	strcat_s(pFullFileName, MAX_PATH, pFileName);
	strcat_s(pFullFileName, MAX_PATH, ".bdv");

	FILE *pFile;
	fopen_s(&pFile, pFullFileName, "rb");

	BDVVertex *pVertices = nullptr;
	u32 *pIndices = nullptr;

	if (pFile)
	{
		BDVTableOfContents contents;
		fread(&contents, sizeof(BDVTableOfContents), 1, pFile);

		// Get the options
		m_pModelSettings = new ModelSettings();
		fseek(pFile, contents.optionsOffset, SEEK_SET);
		fread(m_pModelSettings, sizeof(ModelSettings), 1, pFile);

		// Get the vertices
		fseek(pFile, contents.verticesOffset, SEEK_SET);
		fread(&m_NumVertices, sizeof(u32), 1, pFile);

		pVertices = new BDVVertex[m_NumVertices];
		fread(pVertices, sizeof(BDVVertex) * m_NumVertices, 1, pFile);

		// DirectX Texture Coordinates start at the top-left corner
		// RH systems use the bottom-left corner as the origin
		if (m_pModelSettings->axisType == AxisType::RH)
		{
			for (unsigned int i = 0; i < m_NumVertices; i++)
			{
				pVertices[i].uv.y = 1.0f - pVertices[i].uv.y;
			}
		}

		// Get the indices
		fseek(pFile, contents.indicesOffset, SEEK_SET);
		fread(&m_NumIndices, sizeof(u32), 1, pFile);

		pIndices = new u32[m_NumIndices];
		fread(pIndices, sizeof(u32) * m_NumIndices, 1, pFile);

		// Process the joints
		fseek(pFile, contents.jointsOffset, SEEK_SET);
		fread(&m_NumJoints, sizeof(u32), 1, pFile);

		if (m_NumJoints > 0)
		{
			m_pJoints = new Joint[m_NumJoints];
			fread(m_pJoints, sizeof(Joint) * m_NumJoints, 1, pFile);
		}

		// Process the animations
		fseek(pFile, contents.animationOffset, SEEK_SET);
		fread(&m_NumAnimations, sizeof(u32), 1, pFile);

		if (m_NumAnimations > 0)
		{
			m_pAnimations = new Animation[m_NumAnimations];
			for (unsigned int i = 0; i < m_NumAnimations; i++)
			{
				fread(m_pAnimations[i].name, ANIM_NAME_SIZE, 1, pFile);
				fread(&m_pAnimations[i].numAnimSamples, sizeof(u32), 1, pFile);
				m_pAnimations[i].pAnimSamples = new AnimSample[m_pAnimations[i].numAnimSamples];

				for (unsigned int j = 0; j < m_pAnimations[i].numAnimSamples; j++)
				{
					fread(&m_pAnimations[i].pAnimSamples[j].time, sizeof(f32), 1, pFile);
					fread(&m_pAnimations[i].pAnimSamples[j].numJointAnimations, sizeof(u32), 1, pFile);
					m_pAnimations[i].pAnimSamples[j].pJointAnimations = new JointAnim[m_pAnimations[i].pAnimSamples[j].numJointAnimations];

					fread(m_pAnimations[i].pAnimSamples[j].pJointAnimations, sizeof(JointAnim) * m_pAnimations[i].pAnimSamples[j].numJointAnimations, 1, pFile);
				}
			}
		}

		fclose(pFile);
	}

	m_VertexSize = sizeof(BDVVertex);

	bool ret = true;
	if (!Model::Initialize(pGraphics, pVertices, m_NumVertices, m_VertexSize, pIndices, m_NumIndices))
	{
		ret = false;
	}

	delete[] pVertices;
	delete[] pIndices;

	if (m_NumJoints > 0U && m_NumAnimations > 0U)
	{
		if (!CreateAnimationBuffers(pGraphics))
		{
			return false;
		}
	}

	return ret;
}

void BDVModel::Draw(const Graphics * const pGraphics)
{
	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	static UINT offset = 0;

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_VertexSize, &offset);
	pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	pContext->DrawIndexed(m_NumIndices, 0, 0);
}

ModelSettings *BDVModel::GetModelSettings() const
{
	return m_pModelSettings;
}

bool BDVModel::HasTexture() const
{
	return m_pTexture != nullptr;
}

Texture *BDVModel::GetTexture() const
{
	return m_pTexture;
}

bool BDVModel::HasJoints() const
{
	return m_NumJoints > 0;
}

unsigned int BDVModel::GetNumJoints() const
{
	return m_NumJoints;
}

Joint *BDVModel::GetJoints() const
{
	return m_pJoints;
}

bool BDVModel::HasAnimations() const
{
	return m_NumAnimations > 0;
}

unsigned int BDVModel::GetNumAnimations() const
{
	return m_NumAnimations;
}

Animation *BDVModel::GetAnimation(const unsigned int index) const
{
	return index > m_NumAnimations - 1 ? nullptr : &m_pAnimations[index];
}

bool BDVModel::CreateAnimationBuffers(const Graphics * const pGraphics)
{
	// For GPU animation
	ID3D11Device *pDevice = pGraphics->GetDevice(); pDevice;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.ByteWidth = sizeof(JointData) * m_NumJoints;

	JointData *pJoints = (JointData *)_aligned_malloc(sizeof(JointData) * m_NumJoints, 16);
	for (UINT i = 0; i < m_NumJoints; i++)
	{
		memcpy(&pJoints[i].invBindPose, &m_pJoints[i].invBindPose, sizeof(XMFLOAT4X4));
		pJoints[i].parentIndex = m_pJoints[i].parentIndex;
	}

	D3D11_SUBRESOURCE_DATA bufferData = { 0 };
	bufferData.pSysMem = pJoints;

	if (FAILED(pDevice->CreateBuffer(&bufferDesc, &bufferData, m_pJointsBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	_aligned_free(pJoints);

	m_pAnimSampleBuffers = new ComPtr<ID3D11Buffer>*[m_NumAnimations];
	for (unsigned int i = 0; i < m_NumAnimations; i++)
	{
		m_pAnimSampleBuffers[i] = new ComPtr<ID3D11Buffer>[m_pAnimations[i].numAnimSamples];
		
		for (unsigned int j = 0; j < m_pAnimations[i].numAnimSamples; j++)
		{
			bufferDesc.ByteWidth = sizeof(JointAnim) * m_NumJoints;
			bufferData.pSysMem = m_pAnimations[i].pAnimSamples[j].pJointAnimations;

			if (FAILED(pDevice->CreateBuffer(&bufferDesc, &bufferData, m_pAnimSampleBuffers[i][j].ReleaseAndGetAddressOf())))
			{
				return false;
			}
		}
	}

	return true;
}

ID3D11Buffer *BDVModel::GetJointsBuffer() const
{
	return m_pJointsBuffer.Get();
}

ID3D11Buffer *BDVModel::GetAnimSampleBuffers(unsigned int animationIndex, unsigned int sampleIndex) const
{
	return m_pAnimSampleBuffers[animationIndex][sampleIndex].Get();
}