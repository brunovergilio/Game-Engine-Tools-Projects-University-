#include "AnimationCompressionDemo.h"

static bool hardwareOn = false;

AnimationCompressionDemo::AnimationCompressionDemo(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width, const UINT height)
	: Engine(hInstance, pWindowName, width, height), m_pGraphics(nullptr), m_pCamera(nullptr)
{
}

AnimationCompressionDemo::~AnimationCompressionDemo()
{
}

bool AnimationCompressionDemo::Initialize()
{
	if (!InitWindowResources())
	{
		return false;
	}

	m_pGraphics = new Graphics(m_hWnd, m_Width, m_Height);
	if (!m_pGraphics->Initialize())
	{
		return false;
	}

	Sampler *pSampler = new Sampler();
	if (!pSampler->Initialize(m_pGraphics, FilterType::Point, AddressMode::Wrap, ComparisonFunction::Never))
	{
		return false;
	}

	m_pCamera = new Camera();
	m_pCamera->LookAt(XMVectorSet(0.0f, 0.0f, 6.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	m_pCamera->SetPerspective(XM_PIDIV4, (float)m_Width / (float)m_Height, 1.0f, 1000.0f);
	m_pCamera->SetOrthographic((float)m_Width, (float)m_Height, 0.0f, 1.0f);

	m_pSamplerAnisotropic = new Sampler();
	if (!m_pSamplerAnisotropic->Initialize(m_pGraphics, Anisotropic, Wrap, Never, 16))
	{
		return false;
	}

	m_pSamplerPoint = new Sampler();
	if (!m_pSamplerPoint->Initialize(m_pGraphics, Point, Wrap, Never))
	{
		return false;
	}

	m_pBDVModel = new BDVModel();
	if (!m_pBDVModel->Initialize(m_pGraphics, "humanoid"))
	{
		return false;
	}

	m_pBDVModelCompressed1 = new BDVModel();
	if (!m_pBDVModelCompressed1->Initialize(m_pGraphics, "humanoid-compressed1"))
	{
		return false;
	}

	m_pBDVModelCompressed2 = new BDVModel();
	if (!m_pBDVModelCompressed2->Initialize(m_pGraphics, "humanoid-compressed2"))
	{
		return false;
	}

	m_PointLight.m_Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_PointLight.m_Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_PointLight.m_Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.4f);
	m_PointLight.m_Position = XMFLOAT3(0.0f, 0.0f, 2.0f);
	m_PointLight.m_Range = 20.0f;
	m_PointLight.m_Attenuation = XMFLOAT3(0.0f, 0.2f, 0.0f);

	Material material;
	material.m_Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.m_Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.m_Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.5f);

	m_pBatch = new SpriteBatch(m_pGraphics);
	if (!m_pBatch->Initialize())
	{
		return false;
	}
	m_pBatch->SetSampler(m_pSamplerPoint);

	m_pFontBatch = new SpriteFont(m_pBatch);
	if (!m_pFontBatch->Initialize(m_pGraphics, "Consolas", 24))
	{
		return false;
	}

	m_pAnimShader = new AnimatedShader();
	if (!m_pAnimShader->Initialize(m_pGraphics, "animatedShader"))
	{
		return false;
	}

	m_pObj = new AnimatedGameObject(m_pBDVModel, m_pAnimShader, XMFLOAT4(0.2f, 0.4f, 0.7f, 1.0f), nullptr, nullptr, material);
	m_pObj->SetWorldMatrix(XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(-1.0f, 0.0f, 0.0f));
	m_pObj->UseColor(true);
	m_pObj->UseLight(true);

	m_pObjCompressed1 = new AnimatedGameObject(m_pBDVModelCompressed1, m_pAnimShader, XMFLOAT4(0.7f, 0.2f, 0.4f, 1.0f), nullptr, nullptr, material);
	m_pObjCompressed1->SetWorldMatrix(XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	m_pObjCompressed1->UseColor(true);
	m_pObjCompressed1->UseLight(true);

	m_pObjCompressed2 = new AnimatedGameObject(m_pBDVModelCompressed2, m_pAnimShader, XMFLOAT4(0.4f, 0.7f, 0.2f, 1.0f), nullptr, nullptr, material);
	m_pObjCompressed2->SetWorldMatrix(XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(1.0f, 0.0f, 0.0f));
	m_pObjCompressed2->UseColor(true);
	m_pObjCompressed2->UseLight(true);

	m_pFPS = new FPS();

	m_IsRunning = true;

	return true;
}

void AnimationCompressionDemo::Shutdown()
{
	delete m_pGraphics;

	delete m_pCamera;

	delete m_pSamplerPoint;
	delete m_pSamplerAnisotropic;

	delete m_pFPS;

	delete m_pBatch;
	delete m_pFontBatch;
	delete m_pBDVModel;
	delete m_pBDVModelCompressed1;
	delete m_pBDVModelCompressed2;

	delete m_pObj;
	delete m_pObjCompressed1;
	delete m_pObjCompressed2;
}

void AnimationCompressionDemo::Update()
{
	static float dt;
	dt = m_Timer.DeltaTime();

	m_pFPS->Update(m_Timer.PlayingTime());

	if (GetAsyncKeyState('N') & 0x8000)
	{
		m_PointLight.m_Position.z += 0.003f;
	}
	if (GetAsyncKeyState('M') & 0x8000)
	{
		m_PointLight.m_Position.z -= 0.003f;
	}

	if (GetAsyncKeyState('V') & 0x8000)
	{
		m_pCamera->Walk(dt * 8.0f);
	}

	if (GetAsyncKeyState('B') & 0x8000)
	{
		m_pCamera->Walk(dt * -8.0f);
	}

	if (GetAsyncKeyState('1') & 0x8000)
	{
		m_pObj->SetAnimation(0);
		m_pObjCompressed1->SetAnimation(0);
		m_pObjCompressed2->SetAnimation(0);
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		m_pObj->SetAnimation(1);
		m_pObjCompressed1->SetAnimation(1);
		m_pObjCompressed2->SetAnimation(1);
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		m_pObj->SetAnimation(2);
		m_pObjCompressed1->SetAnimation(2);
		m_pObjCompressed2->SetAnimation(2);
	}

	static bool paused = false;
	static bool pressed = false;
	if (GetAsyncKeyState('P') & 0x8000)
	{
		if (!pressed)
		{
			paused = !paused;
			pressed = true;
		}
	}
	else
	{
		pressed = false;
	}

	static bool hardwarepressed = false;
	if (GetAsyncKeyState('H') & 0x8000)
	{
		if (!hardwarepressed)
		{
			hardwareOn = !hardwareOn;
			m_pObj->TransformOnHardware(hardwareOn);
			m_pObjCompressed1->TransformOnHardware(hardwareOn);
			m_pObjCompressed2->TransformOnHardware(hardwareOn);
			hardwarepressed = true;
		}
	}
	else
	{
		hardwarepressed = false;
	}

	if (!paused)
	{
		m_pObj->Update(dt);
		m_pObjCompressed1->Update(dt);
		m_pObjCompressed2->Update(dt);
	}
}

void AnimationCompressionDemo::Render()
{
	m_pGraphics->Clear();

	m_pAnimShader->SetPerFrameData(m_PointLight, m_pCamera->GetPosition());

	m_pObj->Render(m_pGraphics, m_pCamera);
	m_pObjCompressed1->Render(m_pGraphics, m_pCamera);
	m_pObjCompressed2->Render(m_pGraphics, m_pCamera);

	static char strInfo[80];
	sprintf_s(strInfo, "FPS: %d", m_pFPS->GetFPS());
	m_pFontBatch->RenderText(m_pCamera, strInfo, 0.0f, 0.0f);
	if (hardwareOn)
	{
		m_pFontBatch->RenderText(m_pCamera, "Transform on Hardware: On", 0.0f, m_Height - 30.0f);
	}
	else
	{
		m_pFontBatch->RenderText(m_pCamera, "Transform on Hardware: Off", 0.0f, m_Height - 30.0f);
	}

	sprintf_s(strInfo, "(1) Animation %d: %d Key Frames", m_pObj->GetCurrentAnimationIndex(), m_pBDVModel->GetAnimation(m_pObj->GetCurrentAnimationIndex())->numAnimSamples);
	m_pFontBatch->RenderText(m_pCamera, strInfo, 0.0f, m_Height - 120.0f, XMFLOAT4(0.2f, 0.4f, 0.7f, 1.0f));

	sprintf_s(strInfo, "(2) Animation %d: %d Key Frames (%2.2f Degrees)", m_pObjCompressed1->GetCurrentAnimationIndex(), 
		m_pBDVModelCompressed1->GetAnimation(m_pObjCompressed1->GetCurrentAnimationIndex())->numAnimSamples, m_pBDVModelCompressed1->GetModelSettings()->errorThreshold);
	m_pFontBatch->RenderText(m_pCamera, strInfo, 0.0f, m_Height - 90.0f, XMFLOAT4(0.7f, 0.2f, 0.4f, 1.0f));

	sprintf_s(strInfo, "(3) Animation %d: %d Key Frames (%2.2f Degrees)", m_pObjCompressed2->GetCurrentAnimationIndex(),
		m_pBDVModelCompressed2->GetAnimation(m_pObjCompressed2->GetCurrentAnimationIndex())->numAnimSamples, m_pBDVModelCompressed2->GetModelSettings()->errorThreshold);
	m_pFontBatch->RenderText(m_pCamera, strInfo, 0.0f, m_Height - 60.0f, XMFLOAT4(0.2f, 0.7f, 0.4f, 1.0f));

	m_pGraphics->Present();
}

// Init window resources
bool AnimationCompressionDemo::InitWindowResources()
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = Engine::StaticWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = m_pWindowName;
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		return false;
	}

	RECT rect = { 0, 0, m_Width, m_Height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	m_hWnd = CreateWindowEx(0, m_pWindowName, m_pWindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		width, height, nullptr, nullptr, m_hInstance, this);
	if (!m_hWnd)
	{
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);

	return true;
}

// Windows Callback Procedure
LRESULT AnimationCompressionDemo::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		m_IsRunning = false;
		return 0;
	case WM_SIZE:
		m_Width = LOWORD(lParam);
		m_Height = HIWORD(lParam);
		if (m_pGraphics)
		{
			m_IsPaused = true;
			m_pGraphics->SizeChange(m_Width, m_Height);
			m_IsPaused = false;
		}

		if (m_pCamera)
		{
			m_pCamera->SetPerspective(XM_PIDIV4, (float)m_Width / (float)m_Height, 1.0f, 1000.0f);
			m_pCamera->SetOrthographic((float)m_Width, (float)m_Height, 0.0f, 1.0f);
		}
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	AnimationCompressionDemo app(hInstance, "Animation Compression Demo");
	app.Run();

	return 0;
}