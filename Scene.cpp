#include "Scene.h"

//---------------------------------------------------
//
//		基本操作関数
//
//---------------------------------------------------




bool Scene::CreateConstantBuffer(ID3D11Buffer**ppCB, u_int size)
{
	ID3D11Device* device = pSystem->GetDevice();
	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = size;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, ppCB);
	assert(SUCCEEDED(hr));

	return true;
}

void Scene::Operate()
{
	float time = pSystem->elapsed_time;
	//if (GetKeyState(VK_SHIFT) < 0)  time *= 2;
	//カメラ
	const float camera_up_speed = 12.0f;
	const float camera_dist_speed = 10.0f; 
	if (KeyInput::KeyState() & KEY_J) cangle += time;
	if (KeyInput::KeyState() & KEY_L) cangle -= time;
	if (KeyInput::KeyState() & KEY_I) cpos.y += time * camera_up_speed;
	if (KeyInput::KeyState() & KEY_K) cpos.y -= time * camera_up_speed;
	if (KeyInput::KeyState() & KEY_U) cdist += time * camera_dist_speed;
	if (KeyInput::KeyState() & KEY_O) cdist -= time * camera_dist_speed;


	cpos.x = sinf(cangle)* cdist;
	cpos.z = cosf(cangle)* cdist;
	view = std::make_unique<View>();
	view->Set(cpos, XMFLOAT3(0, 1, 0), XMFLOAT3(0, 1, 0));
	view->SetProjection(XM_PI / 8.0f, (FLOAT)pSystem->GetScreenWidth() / (FLOAT)pSystem->GetScreenHeight(), 0.1f, 5000.0f);

	//ライト
	static float lightAngle = XM_PI;
	if (KeyInput::KeyState() & KEY_A) lightAngle += time * 2.0f;
	if (KeyInput::KeyState() & KEY_D) lightAngle -= time * 2.0f;

	Light::SetAmbient(XMFLOAT3(0.2f, 0.2f, 0.2f));
	//ライト方向
	LightDir.x = sinf(lightAngle);
	LightDir.y = -1.0f;
	LightDir.z = cosf(lightAngle);
	Light::SetDirLight(LightDir, XMFLOAT3(0.6f, 0.6f, 0.6f));

}

//---------------------------------------------------
//
//		imGui用パラメータ
//
//---------------------------------------------------

///////////////////////////

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



void Scene::imGuiUpdate()
{

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin(u8"シェーダーテスト");      // Create a window called "Hello, world!" and append into it.
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------ライト---------");

		ImGui::ColorEdit3(u8"環境光", ambientColor); // Edit 3 floats representing a color
		ImGui::ColorEdit3(u8"平行光", diffuseColor); // Edit 3 floats representing a color
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"---------表題２---------");
		ImGui::Checkbox(u8"チェックボックス１", &checkbox1);      // Edit bools storing our window open/close state
		ImGui::SliderFloat(u8"スライダー１", &slider1, 0.0f, 1.0f);
		ImGui::SliderFloat(u8"スライダー２", &slider2, 0.0f, 10.0f);
		ImGui::Separator();

		ImGui::TextColored(ImVec4(0, 1, 1, 1), u8"---------表題３---------");

		if (ImGui::Button(u8"ボタン１"))counter1++;
		ImGui::SameLine();
		ImGui::Text(u8"回数 %d", counter1);


		ImGui::InputFloat(u8"入力１", &value1);
		ImGui::InputFloat(u8"入力２", &value2);
		static float add = 0.0f;
		if (ImGui::Button(u8"合計")) {
			add = value1 + value2;
		}
		ImGui::SameLine();
		ImGui::Text(u8"合計 %f", add);


		ImGui::Separator();

		ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"---------表題４---------");
		ImGui::Checkbox("Demo Window", &show_demo_window);      
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &slider3, 0.0f, 1.0f);      
		ImGui::ColorEdit3("clear color", Color);		

		if (ImGui::Button("Button"))                           
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}


void Scene::imGuiSceneChanger()
{
	ImGui::Begin(u8"シーンチェンジャー"); 

	if (ImGui::Button(u8"シーンテスト"))
	{
		setScene(std::make_unique<SceneTest0XX>());
	}
	ImGui::Separator();

	if(ImGui::Button(u8"GaussianBlur"))
	{
		setScene(std::make_unique<SceneTestGaussianBlur>());
	}


	if (ImGui::Button(u8"Bloom & Bokeh Control"))
	{
		setScene(std::make_unique<SceneTestBloom>());
	}



	if (ImGui::Button(u8"被写界深度"))
	{
		setScene(std::make_unique<SceneTestDOF>());
	}



	ImGui::Separator();

	if(ImGui::Button(u8"Shadowmap"))
	{
		setScene(std::make_unique<SceneTestShadow>());
	}


	if(ImGui::Button(u8"VarianceShadowmap"))
	{
		setScene(std::make_unique<SceneTestVSM>());
	}

	ImGui::Separator();
	if(ImGui::Button(u8"Flat & Fur"))
	{
		setScene(std::make_unique<SceneTestFlatAndFur>());
	}

	if(ImGui::Button(u8"Toon & Destruction"))
	{
		setScene(std::make_unique<SceneTestToonAndDestruction>());
	}



	ImGui::Separator();

	if(ImGui::Button(u8"PointSprite(ベーシック)"))
	{
		setScene(std::make_unique<SceneTestBasicPointSprite>());
	}
	if(ImGui::Button(u8"PointSprite(アニメーション)"))
	{
		setScene(std::make_unique<SceneTestPointSprite>());
	}

	ImGui::Separator();

	if(ImGui::Button(u8"MRT"))
	{
		setScene(std::make_unique<SceneTestMRT>());
	}

	if(ImGui::Button(u8"MRT2(normal)"))
	{
		setScene(std::make_unique<SceneTestMRT2>());
	}

	if(ImGui::Button(u8"DeferredDirLight"))
	{
		setScene(std::make_unique<SceneTestDeferred>());
	}


	if(ImGui::Button(u8"Deferred_Direction_Point_Spot_Shadow"))
	{
		setScene(std::make_unique<SceneTestDeferredAll>());
	}

	ImGui::Separator();




	ImGui::End();
}

void Scene::imGuiRender()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

