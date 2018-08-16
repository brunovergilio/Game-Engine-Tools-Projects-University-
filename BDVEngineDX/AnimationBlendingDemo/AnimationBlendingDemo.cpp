#include "AnimationBlendingDemo.h"

static bool hardwareOn = false;

AnimationBlendingDemo::AnimationBlendingDemo(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width, const UINT height)
	: Engine(hInstance, pWindowName, width, height), m_pGraphics(nullptr), m_pCamera(nullptr)
{
}

AnimationBlendingDemo::~AnimationBlendingDemo()
{
}

bool AnimationBlendingDemo::Initialize()
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
	m_pCamera->LookAt(XMVectorSet(0.0f, 0.0f, 6.5f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
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

	m_PointLight.m_Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_PointLight.m_Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_PointLight.m_Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.4f);
	m_PointLight.m_Position = XMFLOAT3(-1.0f, 0.0f, 2.0f);
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

	m_pAnimShader = new AnimBlendShader();
	if (!m_pAnimShader->Initialize(m_pGraphics, "animBlendShader"))
	{
		return false;
	}

	m_pObj = new AnimBlendGameObject(m_pBDVModel, m_pAnimShader, XMFLOAT4(0.2f, 0.4f, 0.7f, 1.0f), nullptr, nullptr, material);
	m_pObj->SetWorldMatrix(XMMatrixScaling(0.01f, 0.01f, 0.01f));
	m_pObj->UseColor(true);
	m_pObj->UseLight(true);

	// Animation Blending settings
	m_pObj->SetFadeTime(0.5f);

	m_pFPS = new FPS();

	m_IsRunning = true;

	return true;
}

void AnimationBlendingDemo::Shutdown()
{
	delete m_pGraphics;

	delete m_pCamera;

	delete m_pSamplerPoint;
	delete m_pSamplerAnisotropic;

	delete m_pFPS;

	delete m_pBatch;
	delete m_pFontBatch;
	delete m_pBDVModel;

	delete m_pObj;
}

void AnimationBlendingDemo::Update()
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
		m_pObj->BlendAnimation(0);
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		m_pObj->BlendAnimation(1);
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		m_pObj->BlendAnimation(2);
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
	}
}

void AnimationBlendingDemo::Render()
{
	m_pGraphics->Clear();

	m_pAnimShader->SetPerFrameData(m_PointLight, m_pCamera->GetPosition());

	m_pObj->Render(m_pGraphics, m_pCamera);

	static char strInfo[30];
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

	sprintf_s(strInfo, "Blending: %s", m_pObj->IsBlending() ? "On" : "Off");
	m_pFontBatch->RenderText(m_pCamera, strInfo, 0.0f, m_Height - 90.0f, XMFLOAT4(0.2f, 0.4f, 0.7f, 1.0f));

	sprintf_s(strInfo, "Blend Fade Time: %2.2f", m_pObj->GetFadeTime());
	m_pFontBatch->RenderText(m_pCamera, strInfo, 0.0f, m_Height - 60.0f, XMFLOAT4(0.2f, 0.4f, 0.7f, 1.0f));

	m_pGraphics->Present();
}

// Init window resources
bool AnimationBlendingDemo::InitWindowResources()
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
LRESULT AnimationBlendingDemo::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

	AnimationBlendingDemo app(hInstance, "Animation Blending Demo");
	app.Run();

	return 0;
}