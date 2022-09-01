#include "Scene.h"


//***************************************************
//
//		
//
//***************************************************
///////////////////////////

static float clearColor[3] = { 0.2f, 0.2f, 0.2f };
static float ambientColor[3] = { 0.5f, 0.5f, 0.5f };
static float diffuseColor[3] = { 0.5f, 0.5f, 0.5f };
static bool show_demo_window = false;
static bool show_another_window = false;

/////////////////////////////
static u_int texture_window = false;
static  float deviation = 10.0f;
static  int karnel_size = 7;
static  float texcel_size = 1.0f;

//---------------------------------------------------
//
//		初期化・開放
//
//---------------------------------------------------

bool SceneTestGaussianBlur::Initialize()
{
	// ライト初期化
	Light::Init();

	// コンスタントバッファ
	CreateConstantBuffer(gaussianBlurCB.GetAddressOf(), sizeof(ConstantBufferForPerFrameGaussianBlur));

	// シェーダー
	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/sprite_vs", L"Shaders/sprite_ps");
	gaussianBlur = std::make_unique<ShaderEx>();
	gaussianBlur->Create(L"Shaders/gaussianFilter_vs", L"Shaders/gaussianFilter_ps");

	// サンプラー
	sampler_cramp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	sampler_wrap = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

	// テクスチャ
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Stone_Path_003_baseColor.jpg");

	// 
	blur_texture = std::make_unique<Texture>();
	blur_texture->Create(static_cast<UINT>(test_texture->GetWidth())
						, static_cast<UINT>(test_texture->GetHeight())
						, DXGI_FORMAT_R8G8B8A8_UNORM );

	//オブジェクト
	sprite = std::make_shared<Sprite>();
	return true;
}

//---------------------------------------------------
//
//		更新
//
//---------------------------------------------------
void SceneTestGaussianBlur::imGuiUpdate()
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
		ImGui::ColorEdit3(u8"クリア", clearColor); 

		ImGui::SliderFloat( u8"標準偏差", &deviation, 1, 20 );
		if( ImGui::SliderInt( u8"カーネルサイズ", &karnel_size, 1, KARNEL_SIZE - 1 ) )
		{
			//	偶数なら奇数に直す
			if (karnel_size % 2 == 0)
			{
				karnel_size += 1;
			}
		}

		ImGui::SliderFloat(u8"テクセルサイズ", &texcel_size, 1.0f, 8.0f);
		ImGui::CheckboxFlags(u8"テクスチャウインドを開く", &texture_window, 1);

		ImGui::End();
	}

	if (texture_window)
	{
		ImGui::Begin(u8"テクスチャ", &show_another_window);   
		{
			const int w = 512 / 2;
			const int h = 512 / 2;
			ImGui::Text(u8"スクリーン画像");
			ImGui::Image(test_texture->GetShaderResourceView(), { w, h });
			ImGui::Separator();
			ImGui::Text(u8"ぼかし画像");
			ImGui::Image(blur_texture->GetShaderResourceView(), { w, h });
		}

		ImGui::End();
	}
}

void SceneTestGaussianBlur::Update()
{
	//imGui
	imGuiUpdate();
}

void SceneTestGaussianBlur::CalcGaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma)
{
	float sum = 0.0f;;
	int id = 0;
	for (int y = -karnel_size / 2; y <= karnel_size / 2; y++) {
		for (int x = -karnel_size / 2; x <= karnel_size / 2; x++) {
			array[id].x = (float)x;
			array[id].y = (float)y;
			array[id].z = (float)exp(-(x * x + y * y) / (2.0f * sigma * sigma) ) / (2.0f * PI * sigma * sigma);
			sum += array[id].z;
			id++;
		}
	}
	for (int i = 0; i < karnel_size * karnel_size; i++) {
		array[i].z /= sum;
	}
}

//---------------------------------------------------
//
//		描画
//
//---------------------------------------------------
void SceneTestGaussianBlur::RenderGaussBlur()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	//	ブラーをかける
	{
		{
			// サンプリングの位置オフセットとウェイトを計算する
			DirectX::XMFLOAT4 weight[BUFFER_SIZE];
			ZeroMemory(weight,sizeof(weight));
			CalcGaussianFilter(weight, karnel_size, deviation);

			//コンスタントバッファ更新
			ConstantBufferForPerFrameGaussianBlur cb;
			memcpy(cb.Weight, weight, sizeof(DirectX::XMFLOAT4) * BUFFER_SIZE);
			cb.KarnelSize = static_cast<float>(karnel_size);
			cb.texcel.x	= texcel_size / test_texture->GetWidth() ;
			cb.texcel.y	= texcel_size / test_texture->GetHeight();
			device_context->UpdateSubresource(gaussianBlurCB.Get(), 0, NULL, &cb, 0, 0);
			device_context->PSSetConstantBuffers(3, 1, gaussianBlurCB.GetAddressOf());
		}

		ID3D11RenderTargetView* rtv[1] = { blur_texture->GetRenderTargetView() };
		device_context->OMSetRenderTargets(1, rtv, nullptr);
		// 画面クリア
		float clearColor[4] = { 0, 0, 0, 0 };
		device_context->ClearRenderTargetView( rtv[0], clearColor );
		// ビューポートの設定
		pSystem->SetViewPort(blur_texture->GetWidth(), blur_texture->GetHeight() );
		//	ブレンドステート設定
		device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_NONE), nullptr, 0xFFFFFFFF);
		//ラスタライザ―設定
		device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_NONE));
		//デプスステンシルステート設定
		device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 1);
		//サンプラー設定
		sampler_cramp->Set(0);
		//ブラー
		sprite->Render( gaussianBlur.get(), test_texture.get(),
						0, 0, pSystem->GetScreenWidth(), pSystem->GetScreenHeight(),
						0, 0, test_texture->GetWidth(), test_texture->GetHeight() );
	}

}

void SceneTestGaussianBlur::Render()
{
	//ガウシアンブラー生成
	RenderGaussBlur();

	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	// デフォルト画面クリア
	//pSystem->Clear(clearColor);
	ID3D11RenderTargetView* rtv[1] = { pSystem->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();
	device_context->OMSetRenderTargets(1, rtv, dsv);
	device_context->ClearRenderTargetView(rtv[0], clearColor);
	device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ビューポートの設定
	pSystem->SetViewPort(pSystem->GetScreenWidth(), pSystem->GetScreenHeight());
	// ブレンドステート設定
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_NONE), nullptr, 0xFFFFFFFF);
	// ラスタライザ―設定
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	// デプスステンシルステート設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

	// スプライト表示
	{
		sampler_cramp->Set(0);
		float	x = 0, y = 0;
		float	w = pSystem->GetScreenWidth() / 2, h = pSystem->GetScreenHeight();
		sprite->Render( spriteEx.get(), test_texture.get(),
						x, y, w, h,
						0, 0, test_texture->GetWidth(), test_texture->GetHeight() );
		x	+= w;
		sprite->Render( spriteEx.get(), blur_texture.get(),
						x, y, w, h,
						0, 0, blur_texture->GetWidth(), blur_texture->GetHeight() );
		sampler_cramp->Set(0, FALSE);
	}
	//imGui
	Scene::imGuiRender();

	//画面フリップ
	pSystem->Flip(0);
}
