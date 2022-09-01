#include "Scene.h"


//***************************************************
//
//	screenMap : ScreenSpace
//
//***************************************************


#define SHADOWMAP_WIDTH 512
#define SHADOWMAP_HEIGHT 512


static float clearColor[3] = { 0.2f, 0.2f, 0.2f };
static float ambientColor[3] = { 0.2f, 0.2f, 0.2f };
static float diffuseColor[3] = { 0.5f, 0.5f, 0.5f };

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
static bool show_another_window = true;
static u_int texture_window = 0;
static float slider3 = 0.0f;
static float Color[3] = { 0.625f, 0.937f, 0.86f };
static int counter = 0;


//---------------------------------------------------
//
//		初期化・開放
//
//---------------------------------------------------

bool SceneTestDeferred::Initialize()
{
	Light::Init();

	//コンスタントバッファ
	CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(ConstantBufferForPerFrame));

	//サンプラー
	sampler_wrap = std::make_shared<Sampler>(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP);

	sampler_clamp = std::make_shared<Sampler>(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP);
	FLOAT color[4] = { 1.0f,1.0f,1.0f,1.0f };
	sampler_border = std::make_shared<Sampler>(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_BORDER, color);


	//シェーダー
	phongEx = std::make_unique<ShaderEx>();
	phongEx->Create(L"Shaders/3DObj_vs", L"Shaders/3DObj_ps");

	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/sprite_vs", L"Shaders/sprite_ps");



	//オブジェクト用テクスチャ
	earth_texture = std::make_unique<Texture>();
	earth_texture->Load(L"ASSETS/EARTH/earthmap.jpg");

	earth_normal_texture = std::make_unique<Texture>();
	earth_normal_texture->Load(L"ASSETS/EARTH/earthnormal.jpg");


	//オブジェクト
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Test.png");

	test_normal_texture = std::make_shared<Texture>();
	test_normal_texture->Load(L"ASSETS/Test_normal2.png");

	field_normal_texture = std::make_shared<Texture>();
	//field_normal_texture->Load(L"ASSETS/FIELD/nsea.png");
	field_normal_texture->Load(L"ASSETS/FIELD/normal_rough_sand_texture.png");


	slime = std::make_unique<ObjMesh>("ASSETS/Slime/Slime.obj");
	mr_inc = std::make_unique<ObjMesh>("ASSETS/Mr.Inc/Mr.Incredible.obj");
	wood = std::make_unique<CubeMesh>(L"ASSETS/WOOD.jpg");



	sphere = std::make_unique<SphereMesh>(earth_texture, 8, 8);
	field = std::make_unique<PlainMesh>(L"ASSETS/FIELD/rough_sand_texture.jpg", 5, 5);
	sky = std::make_unique<ObjMesh>("ASSETS/SKY/sky.obj");


	//レンダーターゲット用テクスチャ
	color_texture = std::make_unique<Texture>();
	color_texture->Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);

	normal_texture = std::make_unique<Texture>();
	normal_texture->Create(1280, 720, DXGI_FORMAT_R16G16B16A16_FLOAT);

	position_texture = std::make_unique<Texture>();
	position_texture->Create(1280, 720, DXGI_FORMAT_R16G16B16A16_FLOAT);

	shadow_texture = std::make_unique<Texture>();
	shadow_texture->Create(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	background_texture = std::make_unique<Texture>();
	background_texture->Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);

	sprite = std::make_unique<Sprite>();

	toGbuffer = std::make_unique<ShaderEx>();
	toGbuffer->Create(L"Shaders/toGbuffer_vs", L"Shaders/toGbuffer_ps");

	toGbufferNormal = std::make_unique<ShaderEx>();
	toGbufferNormal->Create(L"Shaders/toGbuffer_normalMap_vs", L"Shaders/toGbuffer_normalMap_ps");

	deferredLightEx = std::make_unique<ShaderEx>();
	deferredLightEx->Create(L"Shaders/deferredLight_vs", L"Shaders/deferredLight_ps");

	deferredNoLight = std::make_unique<ShaderEx>();
	deferredNoLight->Create(L"Shaders/deferredNoLight_vs", L"Shaders/deferredNoLight_ps");

	return true;
}

//---------------------------------------------------
//
//		更新
//
//---------------------------------------------------
void SceneTestDeferred::imGuiUpdate()
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
			ImGui::Separator();
			ImGui::Text("backcolor_texture");
			ImGui::Image(background_texture->GetShaderResourceView(), { w, h });
		}

		ImGui::End();
	}


}

void SceneTestDeferred::Update()
{
	//ライト・カメラ操作設定
	Scene::Operate();
	XMFLOAT3 dir;
	dir.x = Light::LightDir.x;
	dir.y = Light::LightDir.y;
	dir.z = Light::LightDir.z;

	Light::SetAmbient(XMFLOAT3(ambientColor));
	Light::SetDirLight(dir, XMFLOAT3(diffuseColor));
	
	//オブジェクト
	static  XMFLOAT3 aY;
	slime->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
	slime->SetScale(XMFLOAT3(0.4f, 0.4f, 0.4f));
	slime->SetPos(XMFLOAT3(2.0f, 0.0f, 0.0f));
	slime->Update();

	mr_inc->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
	mr_inc->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	mr_inc->SetPos(XMFLOAT3(-2.0f, 0.0f, 0.0f));
	mr_inc->Update();

	aY.y += pSystem->elapsed_time;
	sphere->SetAngle(aY);
	sphere->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphere->SetPos(XMFLOAT3(0.0f, 2.0f, 0.0f));
	sphere->Update();


	//imGui
	imGuiUpdate();
}
//---------------------------------------------------
//
//		描画
//
//---------------------------------------------------
void SceneTestDeferred::RenderTexture()
{

	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	ID3D11RenderTargetView *rtv[3] = {
		color_texture->GetRenderTargetView(),
		normal_texture->GetRenderTargetView(),
		position_texture->GetRenderTargetView(),
	};
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();
	device_context->OMSetRenderTargets(3, rtv, dsv);

	// 画面クリア(透明色)
	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
	for (int i = 0; i < 3; i++) {
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

	// 行列設定
	//サンプラー設定
	sampler_clamp->Set(0);
	slime->Render(toGbuffer.get(), view->GetView(), view->GetProjection());

	//テクスチャ設定
	test_normal_texture->Set(1);
	mr_inc->Render(toGbuffer.get(), view->GetView(), view->GetProjection());

	//テクスチャ設定
	earth_normal_texture->Set(1);
	sphere->Render(toGbufferNormal.get(), view->GetView(), view->GetProjection());

	field->SetScale(XMFLOAT3(20.0f, 20.0f, 20.0f));
	field->SetPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	field->SetAngle(XMFLOAT3(-XM_PI / 2.0f, 0.0f, 0.0f));
	field->Update();
	field_normal_texture->Set(1);

	field->Render(toGbufferNormal.get(), view->GetView(), view->GetProjection());
}

void SceneTestDeferred::RenderTexture2()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	ID3D11RenderTargetView *rtv[1] = {
		background_texture->GetRenderTargetView(),
	};
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();

	device_context->OMSetRenderTargets(1, rtv, dsv);
	// 画面クリア
	float clearColor[4] = { 0.2f,0.2f,0.2f,1.0f };
	for (int i = 0; i < 1; i++) {
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

	// 行列設定
	sampler_clamp->Set(0);
	sky->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	sky->SetPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	sky->SetAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	sky->Update();
	sky->Render(toGbuffer.get(), view->GetView(), view->GetProjection());


}


void SceneTestDeferred::Render()
{
  	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	//Gbufferへの描画
	RenderTexture();
	RenderTexture2();

	// デフォルト画面クリア
	ID3D11RenderTargetView* rtv[1] = { pSystem->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();
	device_context->OMSetRenderTargets(1, rtv, dsv);
	device_context->ClearRenderTargetView(rtv[0], clearColor);
	device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ビューポートの設定
	pSystem->SetViewPort(pSystem->GetScreenWidth(), pSystem->GetScreenHeight());
	// コンスタントバッファ設定
	ConstantBufferForPerFrame cb;
	cb.AmbientColor = Light::Ambient;
	cb.LightDir = Light::LightDir;
	cb.LightColor = Light::DirLightColor;
	cb.EyePos.x = view->GetPos().x;
	cb.EyePos.y = view->GetPos().y;
	cb.EyePos.z = view->GetPos().z;
	cb.EyePos.w = 1.0f;

	//コンスタントバッファ更新
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	device_context->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());

	//ブレンドステート設定
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ALPHA), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//デプスステンシルステート設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);


	sprite->Render(spriteEx.get(), background_texture.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);

	normal_texture->Set(1);
	position_texture->Set(2);
	background_texture->Set(3);
	sampler_border->Set(4);
	sprite->Render(deferredLightEx.get(), color_texture.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);

	normal_texture->Set(1, false);
	position_texture->Set(2, false);
	background_texture->Set(3,false);

	//imGui
	Scene::imGuiRender();

	//画面フリップ

	pSystem->Flip(1);
}
