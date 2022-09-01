#include "Scene.h"


//***************************************************
//
//		
//
//***************************************************
///////////////////////////

static float clearColor[3] = { 0.2f, 0.2f, 0.2f };
static float ambientColor[3] = { 0.0f, 0.0f, 0.0f };
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
static bool show_another_window = false;
static float slider3 = 0.0f;
static float Color[3] = { 0.625f, 0.937f, 0.86f };
static int counter = 0;

///////////////////////////

//---------------------------------------------------
//
//		初期化・開放
//
//---------------------------------------------------

bool SceneTest0XX::Initialize()
{
	//ライト初期化
	Light::Init();

	//コンスタントバッファ
	CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(ConstantBufferForPerFrame));

	//サンプラー
	sampler_clamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	sampler_wrap = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);


	//シェーダー
	phongEx = std::make_unique<ShaderEx>();
	phongEx->Create(L"Shaders/3DObj_vs", L"Shaders/3DObj_ps");

	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/sprite_vs", L"Shaders/sprite_ps");

	objModelEx = std::make_unique<ShaderEx>();
	objModelEx->Create(L"Shaders/OBJModel_vs", L"Shaders/OBJModel_ps");


	//オブジェクト
	suika_texture = std::make_shared<Texture>();
	suika_texture->Load(L"ASSETS/Suika.gif");
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Test.png");
	bisuko_texture = std::make_shared<Texture>();
	bisuko_texture->Load(L"ASSETS/bisuko.jpg");
	wood_texture = std::make_shared<Texture>();
	wood_texture->Load(L"ASSETS/WOOD.jpg");


	blue_back = std::make_shared<Texture>();
	blue_back->Load(L"ASSETS/Red_Back.png");
	bisuko = std::make_unique<CubeMesh>(bisuko_texture);
	cube = std::make_unique<CubeMesh>(test_texture);
	wood = std::make_unique<CubeMesh>(wood_texture);
	ground = std::make_unique<CubeMesh>(test_texture);


	sphere = std::make_unique<SphereMesh>(suika_texture, 8, 8);
	plain = std::make_unique<PlainMesh>(test_texture, 5, 5);

	slime = std::make_unique<ObjMesh>("Assets/Slime/Slime.obj");
	mr_inc = std::make_unique<ObjMesh>("ASSETS/Mr.Inc/Mr.Incredible.obj");


	//レンダーターゲット用テクスチャ
	screen = std::make_unique<Texture>();
	screen->Create(640, 480, DXGI_FORMAT_R16G16B16A16_FLOAT);

	sprite = std::make_shared<Sprite>(suika_texture);

	return true;
}

//---------------------------------------------------
//
//		更新
//
//---------------------------------------------------
void SceneTest0XX::imGuiUpdate()
{

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

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

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

void SceneTest0XX::Update()
{
	//ライト・カメラ操作設定
	Scene::Operate();

	//imGui
	imGuiUpdate();
	XMFLOAT3 dir;
	dir.x = Light::LightDir.x;
	dir.y = Light::LightDir.y;
	dir.z = Light::LightDir.z;

	Light::SetAmbient(XMFLOAT3(ambientColor));
	Light::SetDirLight(dir, XMFLOAT3(diffuseColor));

}
//---------------------------------------------------
//
//		描画
//
//---------------------------------------------------


void SceneTest0XX::Render()
{

	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	// デフォルト画面クリア
	//pSystem->Clear(clearColor);
	ID3D11RenderTargetView* rtv[1] = { pSystem->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();
	device_context->OMSetRenderTargets(1, rtv, dsv);
	device_context->ClearRenderTargetView(rtv[0], clearColor);
	device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//ビュー更新
	view->Activate();

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

	//サンプラー設定
	sampler_clamp->Set(0);
	// 行列設定
	static  XMFLOAT3 aY;
	aY.y += pSystem->elapsed_time;
	bisuko->SetAngle(aY);
	bisuko->SetScale(XMFLOAT3(1.0f, 2.0f, 0.5f));
	bisuko->SetPos(XMFLOAT3(2.0f, 2.0f, 0.0f));
	bisuko->Update();
	bisuko->Render(phongEx.get(), view->GetView(), view->GetProjection());

	sampler_clamp->Set(0);
	cube->SetAngle(aY);
	cube->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	cube->SetPos(XMFLOAT3(-2.0f, 2.0f, 0.0f));
	cube->Update();
	cube->Render(phongEx.get(), view->GetView(), view->GetProjection());

	sampler_wrap->Set(0);
	sphere->SetAngle(aY);
	sphere->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphere->SetPos(XMFLOAT3(0.0f, 2.0f, 0.0f));
	sphere->Update();
	sphere->Render(phongEx.get(), view->GetView(), view->GetProjection());


	//サンプラー設定
	sampler_clamp->Set(0);
	plain->SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));
	plain->SetPos(XMFLOAT3(0, 0, 0));
	plain->SetAngle(XMFLOAT3(-XM_PI / 2, 0, 0));
	plain->Update();
	plain->Render(phongEx.get(), view->GetView(), view->GetProjection());

	//サンプラー設定
	sampler_clamp->Set(0);
	slime->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	slime->SetPos(XMFLOAT3(0, 0, 2.0f));
	slime->SetAngle(XMFLOAT3(0, XM_PI, 0));
	slime->Update();
	slime->Render(objModelEx.get(), view->GetView(), view->GetProjection());

	mr_inc->SetScale(XMFLOAT3(0.4f, 0.4f, 0.4f));
	mr_inc->SetPos(XMFLOAT3(-2.0f, 0.0f, 2.0f));
	mr_inc->SetAngle(aY);
	mr_inc->Update();
	mr_inc->Render(objModelEx.get(), view->GetView(), view->GetProjection());


	sprite->Render(spriteEx.get(),300,400,200,200,0,0,200,200);

	//imGui
	Scene::imGuiRender();

	//画面フリップ
	pSystem->Flip(1);
}
