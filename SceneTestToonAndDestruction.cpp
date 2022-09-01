#include "Scene.h"


//***************************************************
//
//		
//
//***************************************************
///////////////////////////

static float clearColor[3] = { 0.2f, 0.2f, 0.2f };
static	float	ambientColor[3]		= { 0.5f, 0.5f, 0.5f };
static	float	diffuseColor[3]		= { 0.5f, 0.5f, 0.5f };
static	bool	show_demo_window	= false;
static	bool	show_another_window	= false;
///////////////////////////


//---------------------------------------------------
//
//		初期化・開放
//
//---------------------------------------------------

bool SceneTestToonAndDestruction::Initialize()
{
	//ライト初期化
	Light::Init();

	//コンスタントバッファ
	CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(ConstantBufferForPerFrame));
	CreateConstantBuffer(toon_constantbuffer.GetAddressOf(), sizeof(ConstantBufferForToon));
	CreateConstantBuffer(destruction_constantbuffer.GetAddressOf(), sizeof(ConstantBufferForDestruction));

	//シェーダー
	phongEx = std::make_unique<ShaderEx>();
	phongEx->Create(L"Shaders/3DObj_vs", L"Shaders/3DObj_ps");
	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/sprite_vs", L"Shaders/sprite_ps");

	//サンプラー
	sampler_cramp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	sampler_wrap = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

	//テクスチャ
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Stone_Path_003_baseColor.jpg");

	//オブジェクト
	cube = std::make_unique<CubeMesh>(test_texture);
	sphere = std::make_unique<SphereMesh>(test_texture, 8, 8);
	plain = std::make_unique<PlainMesh>(test_texture, 5, 5);
	objmesh = std::make_unique<ObjMesh>("ASSETS/Mr.Incredible/Mr.Incredible.obj");
	slime = std::make_unique<ObjMesh>("ASSETS/Slime/slime.obj");

	//トゥーンレンダリング
	toon_rendering = std::make_unique<ShaderEx>();
	toon_rendering->Create( L"Shaders/ToonRendering_vs",
							L"Shaders/ToonRendering_gs",
							L"Shaders/ToonRendering_ps" );
	toon_diffuse_texture = std::make_shared<Texture>();
	toon_diffuse_texture->Load(L"ASSETS/ramp3.png");

	//破壊表現
	destraction = std::make_unique<ShaderEx>();
	destraction->Create( L"Shaders/Destraction_vs",
						 L"Shaders/Destraction_gs",
						 L"Shaders/Destraction_ps" );
	return	true;
}

//---------------------------------------------------
//
//		更新
//
//---------------------------------------------------
void SceneTestToonAndDestruction::imGuiUpdate()
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

		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------輪郭線---------");
		ImGui::SliderFloat(u8"輪郭線の太さ", &outLineSize, 0.0f, 1.0f, "%.8f" );

		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------破壊表現---------");
		ImGui::SliderFloat(u8"破壊レート", &destractionRate, 0.0f, 1.0f, "%.8f" );
		ImGui::SliderFloat(u8"拡縮係数", &scalingFactor, 0.0f, 1.0f, "%.8f" );
		ImGui::SliderFloat(u8"回転係数", &rotationFactor, -DirectX::XM_PI * 3.0f, DirectX::XM_PI * 3.0f, "%.8f" );
		ImGui::SliderFloat(u8"法線方向への移動係数", &translationFactor, 0.0f, 10.0f, "%.8f" );

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

void SceneTestToonAndDestruction::Update()
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

	// 行列設定
	static  XMFLOAT3 aY;
	aY.y += pSystem->elapsed_time;

	cube->SetAngle(aY);
	cube->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	cube->SetPos(XMFLOAT3(-2.0f, 2.0f, 0.0f));
	cube->Update();


	slime->SetAngle(XMFLOAT3(0,XM_PI,0));
	slime->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	slime->SetPos(XMFLOAT3(2, 0, 0));
	slime->Update();



	objmesh->SetAngle(aY);
	objmesh->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	objmesh->SetPos(XMFLOAT3(-2, 0, 0));
	objmesh->Update();

	plain->SetScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	plain->SetPos(XMFLOAT3(0, 0, 0));
	plain->SetAngle(XMFLOAT3(-XM_PI / 2, 0, 0));
	plain->Update();
}

//---------------------------------------------------
//
//		描画
//
//---------------------------------------------------
void SceneTestToonAndDestruction::Render()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	// デフォルト画面クリア
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
		toon_diffuse_texture->Set( 1 );
		sampler_cramp->Set( 0 );
		sampler_cramp->Set( 1 );
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
		device_context->GSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());

		plain->Render(phongEx.get(), view->GetView(), view->GetProjection());
	}
	//	トゥーンレンダリング
	{
		toon_diffuse_texture->Set( 1 );
		sampler_cramp->Set( 0 );
		sampler_cramp->Set( 1 );
		{
			// コンスタントバッファ設定
			ConstantBufferForPerFrame cb;
			cb.AmbientColor = Light::Ambient;
			cb.LightDir = Light::LightDir;
			cb.LightColor = Light::DirLightColor;
			cb.EyePos.x = view->GetPos().x;
			cb.EyePos.y = view->GetPos().y;
			cb.EyePos.z = view->GetPos().z;
			cb.EyePos.w = 1.0f;
			//cb.Paramter.x = outLineSize;

			//コンスタントバッファ更新
			device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
			device_context->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
			device_context->GSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
			device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		}
		{
			ConstantBufferForToon cb;
			cb.Paramter.x = outLineSize;
			device_context->UpdateSubresource(toon_constantbuffer.Get(), 0, NULL, &cb, 0, 0);
			device_context->GSSetConstantBuffers(3, 1, toon_constantbuffer.GetAddressOf());

		}

		slime->Render(toon_rendering.get(), view->GetView(), view->GetProjection());
		toon_diffuse_texture->Set( 1, false );
		sampler_cramp->Set( 0, false );
		sampler_cramp->Set( 1, false );
	}
	//	破壊表現
	{
		sampler_cramp->Set( 0 );
		
		{
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
			device_context->GSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
			device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		}

		{
			ConstantBufferForDestruction cb;
			cb.Paramter.x = destractionRate;
			cb.Paramter.y = scalingFactor * destractionRate;
			cb.Paramter.z = rotationFactor * destractionRate;
			cb.Paramter.w = translationFactor * destractionRate;
			device_context->UpdateSubresource(destruction_constantbuffer.Get(), 0, NULL, &cb, 0, 0);
			device_context->VSSetConstantBuffers(4, 1, destruction_constantbuffer.GetAddressOf());
			device_context->GSSetConstantBuffers(4, 1, destruction_constantbuffer.GetAddressOf());

		}

		
		objmesh->Render(destraction.get(), view->GetView(), view->GetProjection());
		sampler_cramp->Set( 0, false );
	}

	//imGui
	Scene::imGuiRender();

	//画面フリップ
	pSystem->Flip(1);
}
