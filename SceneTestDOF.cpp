#include "Scene.h"


//***************************************************
//
//		
//
//***************************************************
///////////////////////////

static float clearColor[3] = { 0.2f, 0.2f, 0.2f };
static float ambientColor[3] = { 0.2f, 0.2f, 0.2f };
static float diffuseColor[3] = { 0.7f, 0.7f, 0.7f };

///////////////////////////

static bool checkbox1 = false;
static float slider1 = 2;
static float slider2 = 1.0f;
static float slider3 = 1.0f;

///////////////////////////

static int counter1 = 0;
static float value1 = 0.0f;
static float value2 = 0.0f;

///////////////////////////

static bool show_demo_window = false;
static bool show_another_window = false;
static u_int texture_window = false;

static float slider4 = 0.0f;
static float Color[3] = { 0.625f, 0.937f, 0.86f };
static int counter = 0;
static float Threshold = 0.5f;
static float Sigma = 1.0f;
static float Dist = 15.0f;
static float Focus = 6.0f;


//---------------------------------------------------
//
//		初期化・開放
//
//---------------------------------------------------

bool SceneTestDOF::Initialize()
{
	//ライト初期化
	Light::Init();


	//コンスタントバッファ
	CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(ConstantBufferForPerFrame));
	CreateConstantBuffer(gaussianBlurCB.GetAddressOf(), sizeof(ConstantBufferForPerFrameGaussianBlur));
	CreateConstantBuffer(depth_of_fieldCB.GetAddressOf(), sizeof(ConstantBufferForDOF));


	//サンプラー
	sampler_clamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	sampler_wrap = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);


	//シェーダー
	phongEx = std::make_unique<ShaderEx>();
	phongEx->Create(L"Shaders/3DObj_vs", L"Shaders/3DObj_ps");
	phongDepthEx = std::make_unique<ShaderEx>();
	phongDepthEx->Create(L"Shaders/PhongDepth_vs", L"Shaders/PhongDepth_ps");
	noLightEx = std::make_unique<ShaderEx>();
	noLightEx->Create(L"Shaders/3DNoLightObj_vs", L"Shaders/3DNoLightObj_ps");
	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/Sprite_vs", L"Shaders/Sprite_ps");
	hdrEx = std::make_unique<ShaderEx>();
	hdrEx->Create(L"Shaders/hdr_vs", L"Shaders/hdr_ps");

	dofEx = std::make_unique<ShaderEx>();
	dofEx->Create(L"Shaders/DOF_vs", L"Shaders/DOF_ps");
	bokehEx = std::make_unique<ShaderEx>();
	bokehEx->Create(L"Shaders/bokeh_vs", L"Shaders/bokeh_ps");


	//オブジェクト
	suika_texture = std::make_shared<Texture>();
	suika_texture->Load(L"ASSETS/Suika.gif");
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Test.png");
	bisuko_texture = std::make_shared<Texture>();
	bisuko_texture->Load(L"ASSETS/bisuko.jpg");

	blue_back = std::make_shared<Texture>();
	blue_back->Load(L"ASSETS/Red_Back.png");
	bisuko = std::make_unique<CubeMesh>(bisuko_texture);
	cube = std::make_unique<CubeMesh>(test_texture);
	ground = std::make_unique<CubeMesh>(test_texture);


	sphere = std::make_unique<SphereMesh>(suika_texture, 8, 8);
	plain = std::make_unique<PlainMesh>(test_texture, 5, 5);

	slime = std::make_unique<ObjMesh>("ASSETS/Slime/slime.obj");
	mr_inc = std::make_unique<ObjMesh>("ASSETS/Mr.Inc/Mr.Incredible.obj");
	sky = std::make_unique<ObjMesh>("ASSETS/SKY/sky.obj");


	bokeh_texture = std::make_shared<Texture>();
	bokeh_texture->Create(BOKEH_WIDTH, BOKEH_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);



	sprite = std::make_shared<Sprite>();

	//レンダーターゲット用テクスチャ
	screen_texture = std::make_unique<Texture>();
	screen_texture->Create(SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	depth_texture = std::make_unique<Texture>();
	depth_texture->Create(SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	

	return true;
}

//---------------------------------------------------
//
//		更新
//
//---------------------------------------------------
void SceneTestDOF::imGuiUpdate()
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

		//ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"--------- BRIGHT ---------");

		//ImGui::SliderFloat(u8"しきい値", &Threshold, 0.0f, 1.0f);

		//ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"--------- BOKEH ---------");

		//ImGui::SliderFloat(u8"標準偏差", &Sigma, 0.2f, 2.0f);
		////ImGui::Checkbox(u8"ブルーム有効", &bBloom);
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"--------- 被写界深度 ---------");

		ImGui::SliderFloat(u8"焦点距離", &Dist, 0.0f, 50.0f);
		ImGui::Separator();
		ImGui::SliderFloat(u8"被写界深度", &Focus, 0.0f, 50.0f);


		ImGui::End();
	}

	if (texture_window)
	{
		ImGui::Begin(u8"テクスチャ", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		{
			const int w = 1280 / 4;
			const int h = 720 / 4;
			ImGui::Text("screen_texture");
			ImGui::Image(screen_texture->GetShaderResourceView(), { w, h });
			ImGui::Separator();
			ImGui::Text("bokeh_texture");
			ImGui::Image(bokeh_texture->GetShaderResourceView(), { w, h });
			ImGui::Separator();
			ImGui::Text("depth_texture");
			ImGui::Image(bokeh_texture->GetShaderResourceView(), { w, h });
		}

		ImGui::End();
	}
}

void SceneTestDOF::Update()
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


	sky->SetScale(XMFLOAT3(3.0f, 1.0f, 3.0f));
	sky->SetPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	sky->SetAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	sky->Update();


	plain->SetScale(XMFLOAT3(30.0f, 30.0f, 30.0f));
	plain->SetPos(XMFLOAT3(0, 0, 0));
	plain->SetAngle(XMFLOAT3(-XM_PI / 2, 0, 0));
	plain->Update();

}
//---------------------------------------------------
//
//		描画
//
//---------------------------------------------------
void SceneTestDOF::RenderScreen()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	{
		ID3D11RenderTargetView *rtv[2] = {
			screen_texture->GetRenderTargetView(),
			depth_texture->GetRenderTargetView()
		};
		ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();


		device_context->OMSetRenderTargets(2, rtv, dsv);
		// 画面クリア
		float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
		for (int i = 0; i < 2; i++) {
			device_context->ClearRenderTargetView(rtv[i], clearColor);
		}
		device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


		//ビュー更新
		view->Activate();

		// ビューポートの設定
		pSystem->SetViewPort(SCREEN_WIDTH, SCREEN_HEIGHT);

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


		// 行列設定
		static  XMFLOAT3 aY;
		aY.y += pSystem->elapsed_time;

		slime->SetScale(XMFLOAT3(.4f, .4f, .4f));
		slime->SetPos(XMFLOAT3(1.0f, 0.0f, 0.0f));
		slime->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
		slime->Update();
		sampler_clamp->Set(0);
		slime->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		slime->SetScale(XMFLOAT3(.4f, .4f, .4f));
		slime->SetPos(XMFLOAT3(-2.0f, 0.0f, 5.0f));
		slime->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
		slime->Update();
		sampler_clamp->Set(0);
		slime->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		slime->SetScale(XMFLOAT3(.4f, .4f, .4f));
		slime->SetPos(XMFLOAT3( 0.0f, 0.0f, -8.0f));
		slime->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
		slime->Update();
		sampler_clamp->Set(0);
		slime->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		mr_inc->SetScale(XMFLOAT3( 0.4f, 0.4f, 0.4f));
		mr_inc->SetPos(XMFLOAT3(-2.0f, 0.0f, 8.0f));
		mr_inc->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
		mr_inc->Update();
		sampler_clamp->Set(0);
		mr_inc->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		mr_inc->SetScale(XMFLOAT3(0.4f, 0.4f, 0.4f));
		mr_inc->SetPos(XMFLOAT3(10.0f, 0.0f, 3.0f));
		mr_inc->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
		mr_inc->Update();
		sampler_clamp->Set(0);
		mr_inc->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		mr_inc->SetScale(XMFLOAT3(0.4f, 0.4f, 0.4f));
		mr_inc->SetPos(XMFLOAT3(2.0f, 0.0f, -10.0f));
		mr_inc->SetAngle(XMFLOAT3(0.0f, XM_PI, 0.0f));
		mr_inc->Update();
		sampler_clamp->Set(0);
		mr_inc->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		sphere->SetAngle(aY);
		sphere->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		sphere->SetPos(XMFLOAT3(5.0f, 2.0f, -3.0f));
		sphere->Update();
		sampler_wrap->Set(0);
		sphere->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		sphere->SetAngle(aY);
		sphere->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		sphere->SetPos(XMFLOAT3(-5.0f, 2.0f, 8.0f));
		sphere->Update();
		sampler_wrap->Set(0);
		sphere->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());

		sphere->SetAngle(aY);
		sphere->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		sphere->SetPos(XMFLOAT3(8.0f, 2.0f, 5.0f));
		sphere->Update();
		sampler_wrap->Set(0);
		sphere->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());


		sampler_clamp->Set(0);
		//sky->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());
		sky->Render(noLightEx.get(), view->GetView(), view->GetProjection());

		//サンプラー設定
		sampler_clamp->Set(0);
		plain->Render(phongDepthEx.get(), view->GetView(), view->GetProjection());
	}




}

bool SceneTestDOF::GaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma)
{
	float sum = 0.0f;
	int id = 0;
	for (int y = -karnel_size / 2; y <= karnel_size / 2; y++) {
		for (int x = -karnel_size / 2; x <= karnel_size / 2; x++) {
			array[id].x = (float)x;
			array[id].y = (float)y;
			array[id].z = (float)expf(-(x * x + y * y) / 2.0f * sigma * sigma) / sqrtf(2.0f * PI * sigma);
			sum += array[id].z;
			id++;
		}
	}
	for (int i = 0; i < karnel_size * karnel_size; i++) {
		array[i].z /= sum;
	}
	return true;
}

void SceneTestDOF::RenderBokeh()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
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


	{//ぼけ画像作成

		// サンプリングの位置オフセットとウェイトを計算する
		DirectX::XMFLOAT4 weight[BUFFER_SIZE];
		ZeroMemory(weight, sizeof(weight));
		GaussianFilter(weight, KARNEL_SIZE, Sigma);

		ConstantBufferForPerFrameGaussianBlur cb{};
		memcpy(cb.Weight, weight, sizeof(DirectX::XMFLOAT4) * BUFFER_SIZE);
		cb.KarnelSize = KARNEL_SIZE;
		//コンスタントバッファ更新
		device_context->UpdateSubresource(gaussianBlurCB.Get(), 0, NULL, &cb, 0, 0);
		device_context->VSSetConstantBuffers(3, 1, gaussianBlurCB.GetAddressOf());
		device_context->PSSetConstantBuffers(3, 1, gaussianBlurCB.GetAddressOf());


		ID3D11RenderTargetView* rtv[1] = {
			bokeh_texture->GetRenderTargetView(),
		};
		ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();

		device_context->OMSetRenderTargets(1, rtv, dsv);
		// 画面クリア
		float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
		for (int i = 0; i < 1; i++) {
			device_context->ClearRenderTargetView(rtv[i], clearColor);
		}
		// ビューポートの設定
		pSystem->SetViewPort(BOKEH_WIDTH, BOKEH_HEIGHT);
		//	ブレンドステート設定
		device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ADD), nullptr, 0xFFFFFFFF);

		sprite->Render(bokehEx.get(), screen_texture.get(), 0, 0, 1280, 720, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	}

}



void SceneTestDOF::Render()
{

	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	RenderScreen();
	RenderBokeh();

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
	//ブレンドステート設定
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ALPHA), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//デプスステンシルステート設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

	{
		// コンスタントバッファ設定
		ConstantBufferForDOF cb;
		cb.dist = Dist;
		cb.focus = Focus;
		//コンスタントバッファ更新
		device_context->UpdateSubresource(depth_of_fieldCB.Get(), 0, NULL, &cb, 0, 0);
		device_context->PSSetConstantBuffers(4, 1, depth_of_fieldCB.GetAddressOf());
	}

	bokeh_texture->Set(1);
	depth_texture->Set(2);
	sprite->Render(dofEx.get(), screen_texture.get(), 0, 0, 1280, 720, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//imGui
	Scene::imGuiRender();

	//画面フリップ
	pSystem->Flip(1);
}
