#include "Scene.h"


//***************************************************
//
//		DeferredShader(MRT)
//
//***************************************************
///////////////////////////

static float clearColor[3] = { 0.2f, 0.2f, 0.2f };
static float ambientColor[3] = { 0.0f, 0.0f, 0.0f };
static float diffuseColor[3] = { 0.5f, 0.5f, 0.5f };

///////////////////////////
static int Anime = 0;
static bool beAnime1 = true;
static bool beAnime2 = false;
static bool beAnime3 = false;

static float dissolveLevel = 0.0f;
static float emissiveWidth = 0.0f;


///////////////////////////

static bool checkbox1 = false;
static float slider1 = 2;
static float slider2 = 1.0f;

///////////////////////////

static int counter1 = 0;
static float value1 = 0.0f;
static float value2 = 0.0f;

///////////////////////////

static bool show_demo_window = false;
static bool show_another_window = false;
static u_int texture_window = 0;

static float slider3 = 0.0f;
static float Color[3] = { 0.625f, 0.937f, 0.86f };
static int counter = 0;

///////////////////////////

//---------------------------------------------------
//
//		初期化・開放
//
//---------------------------------------------------

bool SceneTestMRT::Initialize()
{

	Light::Init();
	//コンスタントバッファ
	CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(ConstantBufferForPerFrame));

	//サンプラー
	sampler = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR,D3D11_TEXTURE_ADDRESS_CLAMP);

	//シェーダー
	phongEx = std::make_unique<ShaderEx>();
	phongEx->Create(L"Shaders/3DObj_vs", L"Shaders/3DObj_ps");

	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/sprite_vs", L"Shaders/sprite_ps");


	//オブジェクト用テクスチャ
	earth_texture = std::make_unique<Texture>();
	earth_texture->Load(L"ASSETS/EARTH/earthmap.jpg");

	//オブジェクト
	//white = std::make_shared<Texture>();
	//white->Load(L"ASSETS/White.png");
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Test.png");
	blue_back = std::make_shared<Texture>();
	blue_back->Load(L"ASSETS/Red_Back.png");
	bisuko = std::make_unique<CubeMesh>(L"ASSETS/bisuko.jpg");
	cube = std::make_unique<CubeMesh>(test_texture);
	wood = std::make_unique<CubeMesh>(L"ASSETS/WOOD.jpg");
	ground = std::make_unique<CubeMesh>(test_texture);

	sphere = std::make_unique<SphereMesh>(earth_texture, 8, 8);
	plain = std::make_unique<PlainMesh>(test_texture, 5, 5);


	//レンダーターゲット用テクスチャ
	color_texture = std::make_unique<Texture>();
	color_texture->Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);

	normal_texture = std::make_unique<Texture>();
	normal_texture->Create(1280, 720, DXGI_FORMAT_R16G16B16A16_FLOAT);

	position_texture = std::make_unique<Texture>();
	position_texture->Create(1280, 720, DXGI_FORMAT_R16G16B16A16_FLOAT);

	sprite = std::make_unique<Sprite>();

	toGbuffer = std::make_unique<ShaderEx>();
	toGbuffer->Create(L"Shaders/toGbuffer_vs", L"Shaders/toGbuffer_ps");


	return true;
}

//---------------------------------------------------
//
//		更新
//
//---------------------------------------------------
void SceneTestMRT::imGuiUpdate()
{

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Change Scene 
	imGuiSceneChanger();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin(u8"シェーダーテスト");      // Create a window called "Hello, world!" and append into it.
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"-------クリアカラー-------");
		ImGui::ColorEdit3(u8"クリア", (float*)&clearColor); // Edit 3 floats representing a color
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------ライト---------");

		ImGui::ColorEdit3(u8"環境光", (float*)&ambientColor); // Edit 3 floats representing a color
		ImGui::ColorEdit3(u8"平行光", (float*)&diffuseColor); // Edit 3 floats representing a color
		ImGui::Separator();
		ImGui::CheckboxFlags(u8"テクスチャウインドを開く", &texture_window, 1);

		ImGui::End();
	}
	if (texture_window)
	{
		ImGui::Begin(u8"テクスチャ", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		{
			const int w = 1280 / 4;
			const int h = 720 / 4;
			ImGui::Text("color_texture");
			ImGui::Image(color_texture->GetShaderResourceView(), { w, h });
			ImGui::Separator();
			ImGui::Text("normal_texture");
			ImGui::Image(normal_texture->GetShaderResourceView(), { w, h });
			ImGui::Separator();
			ImGui::Text("position_texture");
			ImGui::Image(position_texture->GetShaderResourceView(), { w, h });
		}
		ImGui::End();
	}

}

void SceneTestMRT::Update()
{
	//ライト・カメラ操作設定
	Scene::Operate();


	XMFLOAT3 dir;
	dir.x = Light::LightDir.x;
	dir.y = Light::LightDir.y;
	dir.z = Light::LightDir.z;

	Light::SetAmbient(XMFLOAT3(ambientColor));
	Light::SetDirLight(dir, XMFLOAT3(diffuseColor));


	//imGui
	imGuiUpdate();

}
//---------------------------------------------------
//
//		描画
//
//---------------------------------------------------
void SceneTestMRT::RenderTexture()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	// レンダーターゲット、
	ID3D11RenderTargetView *rtv[3] = {
		color_texture->GetRenderTargetView(),
		normal_texture->GetRenderTargetView(),
		position_texture->GetRenderTargetView(),
	};
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();

	device_context->OMSetRenderTargets(3, rtv, dsv);
	// 画面クリア
	float clearColor[4] = { 0.2f,0.2f,0.2f,0.0f };
	for (int i = 0; i < 3;i++) {
		device_context->ClearRenderTargetView(rtv[i], clearColor);
	}
	device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	//ビュー更新
	view->Activate();
	// ビューポートの設定
	pSystem->SetViewPort(pSystem->GetScreenWidth(), pSystem->GetScreenHeight());


	//ブレンドステート設定
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ALPHA), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//デプスステンシルステート設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

	//サンプラー設定
	sampler->Set(0);
	// 行列設定
	static  XMFLOAT3 aY;
	aY.y += pSystem->elapsed_time;
	bisuko->SetAngle(aY);
	bisuko->SetScale(XMFLOAT3(1.0f, 2.0f, 0.5f));
	bisuko->SetPos(XMFLOAT3(2.0f, 2.0f, 0.0f));
	bisuko->Update();
	bisuko->Render(toGbuffer.get(), view->GetView(), view->GetProjection());

	cube->SetAngle(aY);
	cube->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	cube->SetPos(XMFLOAT3(-2.0f, 2.0f, 0.0f));
	cube->Update();
	cube->Render(toGbuffer.get(), view->GetView(), view->GetProjection());

	sphere->SetAngle(aY);
	sphere->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphere->SetPos(XMFLOAT3(0.0f, 2.0f, 0.0f));
	sphere->Update();
	sphere->Render(toGbuffer.get(), view->GetView(), view->GetProjection());

	//サンプラー設定
	sampler->Set(0);

	plain->SetScale(XMFLOAT3(10.0f, 10.0f, 1.0f));
	plain->SetPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	plain->SetAngle(XMFLOAT3(-XM_PI / 2.0f, 0.0f, 0.0f));
	plain->Update();
	plain->Render(toGbuffer.get(), view->GetView(), view->GetProjection());

}


void SceneTestMRT::Render()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	//Gbufferへの描画
	RenderTexture();

	// 画面クリア
	ID3D11RenderTargetView* rtv[1] = { pSystem->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();
	device_context->OMSetRenderTargets(1, rtv, dsv);
	device_context->ClearRenderTargetView(rtv[0], clearColor);
	device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//ビュー更新
	view->Activate();

	// ビューポートの設定
	pSystem->SetViewPort(pSystem->GetScreenWidth(), pSystem->GetScreenHeight());

	//ブレンドステート設定
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ALPHA), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//デプスステンシルステート設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

	//Gbuffer描画
	sprite->Render(spriteEx.get(), color_texture.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);

	//imGui
	Scene::imGuiRender();


	//画面フリップ

	pSystem->Flip(1);
}
