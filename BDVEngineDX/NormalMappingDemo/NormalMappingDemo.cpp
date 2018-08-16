#include "NormalMappingDemo.h"

NormalMappingDemo::NormalMappingDemo(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width, const UINT height)
	: Engine(hInstance, pWindowName, width, height), m_pGraphics(nullptr), m_pCamera(nullptr)
{
}

NormalMappingDemo::~NormalMappingDemo()
{
}

bool NormalMappingDemo::Initialize()
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
	m_pCamera->LookAt(XMVectorSet(-1.0f, 0.0f, 2.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	m_pCamera->SetPerspective(XM_PIDIV4, (float)m_Width / (float)m_Height, 1.0f, 1000.0f);
	m_pCamera->SetOrthographic((float)m_Width, (float)m_Height, 0.0f, 1.0f);

	m_pBlueWallTex = new Texture();
	if (!m_pBlueWallTex->Initialize(m_pGraphics, "bluewall.jpg"))
	{
		return false;
	}

	m_pBlueWallNormalMapTex = new Texture();
	if (!m_pBlueWallNormalMapTex->Initialize(m_pGraphics, "bluewallNormalMap.bmp"))
	{
		return false;
	}

	m_pBlackWallTex = new Texture();
	if (!m_pBlackWallTex->Initialize(m_pGraphics, "blackwall.bmp"))
	{
		return false;
	}

	m_pBlackWallNormalMapTex = new Texture();
	if (!m_pBlackWallNormalMapTex->Initialize(m_pGraphics, "blackwallNormalMap.bmp"))
	{
		return false;
	}

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
	if (!m_pBDVModel->Initialize(m_pGraphics, "cube"))
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
	//if (!m_pFontBatch->Initialize(m_pGraphics, "Consolas", 24))
	if (!m_pFontBatch->Initialize(m_pGraphics, "Consolas24.bmp", "Consolas24.bdvfont"))
	{
		return false;
	}

	m_pBasicShader = new BasicShader();
	if (!m_pBasicShader->Initialize(m_pGraphics, "basicShader"))
	{
		return false;
	}

	m_pObj1 = new BasicGameObject(m_pBDVModel, m_pBasicShader, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), m_pBlueWallTex, m_pSamplerAnisotropic, material);
	m_pObj1->UseTexture(true);
	m_pObj1->SetNormalMap(m_pBlueWallNormalMapTex);
	m_pObj1->SetWorldMatrix(XMMatrixTranslation(-0.5f, 0.0f, 0.0f));

	m_pObj2 = new BasicGameObject(m_pBDVModel, m_pBasicShader, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), m_pBlackWallTex, m_pSamplerAnisotropic, material);
	m_pObj2->UseTexture(true);
	m_pObj2->SetNormalMap(m_pBlackWallNormalMapTex);
	m_pObj2->SetWorldMatrix(XMMatrixTranslation(0.5f, 0.0f, 0.0f));

	m_pFPS = new FPS();

	m_IsRunning = true;

	return true;
}

void NormalMappingDemo::Shutdown()
{
	delete m_pGraphics;

	delete m_pCamera;

	delete m_pSamplerPoint;
	delete m_pSamplerAnisotropic;

	delete m_pFPS;

	delete m_pBatch;
	delete m_pFontBatch;
	delete m_pBDVModel;

	delete m_pObj1;
	delete m_pObj2;
}

void NormalMappingDemo::Update()
{
	static float dt;
	dt = m_Timer.DeltaTime();

	m_pFPS->Update(m_Timer.PlayingTime());

	if (GetAsyncKeyState('N') & 0x8000)
	{
		m_PointLight.m_Position.z += dt * 3.0f;
	}
	if (GetAsyncKeyState('M') & 0x8000)
	{
		m_PointLight.m_Position.z -= dt * 3.0f;
	}

	if (GetAsyncKeyState('V') & 0x8000)
	{
		m_pCamera->Walk(dt * 5.0f);
	}

	if (GetAsyncKeyState('B') & 0x8000)
	{
		m_pCamera->Walk(dt * -5.0f);
	}

	if (IsKeyPressed('L'))
	{
		m_pObj1->UseLight(!m_pObj1->UseLight());
		m_pObj2->UseLight(!m_pObj2->UseLight());
	}

	if (IsKeyPressed('H'))
	{
		m_pObj1->UseNormalMapping(!m_pObj1->UseNormalMapping());
		m_pObj2->UseNormalMapping(!m_pObj2->UseNormalMapping());
	}
}

void NormalMappingDemo::Render()
{
	m_pGraphics->Clear();

	m_pBasicShader->SetPerFrameData(m_PointLight, m_pCamera->GetPosition());

	m_pObj1->Render(m_pGraphics, m_pCamera);
	m_pObj2->Render(m_pGraphics, m_pCamera);

	static char fpsStr[30];
	sprintf_s(fpsStr, "FPS: %d", m_pFPS->GetFPS());
	m_pFontBatch->RenderText(m_pCamera, fpsStr, 0.0f, 0.0f);

	static char lightStr[30];
	sprintf_s(lightStr, "Lights: %s", m_pObj1->UseLight() ? "On" : "Off");
	static char normalMapStr[30];
	sprintf_s(normalMapStr, "Normal Map: %s", m_pObj1->UseLight() && m_pObj1->UseNormalMapping() ? "On" : "Off");

	m_pFontBatch->RenderText(m_pCamera, lightStr, (float)m_Width * 0.4f, 0.0f);
	m_pFontBatch->RenderText(m_pCamera, normalMapStr, (float)m_Width * 0.4f, 30.0f);

	m_pGraphics->Present();
}

// Init window resources
bool NormalMappingDemo::InitWindowResources()
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
LRESULT NormalMappingDemo::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

bool NormalMappingDemo::IsKeyPressed(const int key)
{
	static bool keysPressed[256] = {};

	bool pressed = false;
	if (GetAsyncKeyState(key) & 0x8000)
	{
		if (!keysPressed[key])
		{
			pressed = true;
			keysPressed[key] = true;
		}
	}
	else
	{
		keysPressed[key] = false;
	}

	return pressed;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	NormalMappingDemo app(hInstance, "Normal Mapping Demo");
	app.Run();

	return 0;
}