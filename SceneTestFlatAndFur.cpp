#include "Scene.h"


//***************************************************
//
//		
//
//***************************************************
///////////////////////////

static float	clearColor[3] = { 0.2f, 0.2f, 0.2f };
static	float	ambientColor[3] = { 0.5f, 0.5f, 0.5f };
static	float	diffuseColor[3] = { 0.5f, 0.5f, 0.5f };
static	bool	show_demo_window = false;
static	bool	show_another_window = false;



//---------------------------------------------------
//
//		�������E�J��
//
//---------------------------------------------------

bool SceneTestFlatAndFur::Initialize()
{
	//���C�g������
	Light::Init();

	//�R���X�^���g�o�b�t�@
	CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(ConstantBufferForPerFrame));
	CreateConstantBuffer(fur_constantbuffer.GetAddressOf(), sizeof(ConstantBufferForFur));

	//�V�F�[�_�[
	phongEx = std::make_unique<ShaderEx>();
	phongEx->Create(L"Shaders/3DObj_vs", L"Shaders/3DObj_ps");
	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/sprite_vs", L"Shaders/sprite_ps");

	//�T���v���[
	sampler_cramp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	sampler_wrap = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

	//�e�N�X�`��
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Stone_Path_003_baseColor.jpg");

	fur_texture = std::make_unique<Texture>();
	fur_texture->Load(L"ASSETS/Slime/fur.png");



	//�I�u�W�F�N�g
	cube = std::make_unique<CubeMesh>(test_texture);
	sphere = std::make_unique<SphereMesh>(test_texture, 8, 8);
	plain = std::make_unique<PlainMesh>(test_texture, 5, 5);
	objmesh = std::make_unique<ObjMesh>("ASSETS/Mr.Incredible/Mr.Incredible.obj");
	slime = std::make_unique<ObjMesh>("ASSETS/Slime/slime.obj");

	//�t�@�[�V�F�[�_
	fur_shader = std::make_unique<ShaderEx>();
	fur_shader->Create(L"Shaders/furShader_vs",
		L"Shaders/furShader_gs",
		L"Shaders/furShader_ps");

	//�t���b�g�V�F�[�_
	flat = std::make_unique<ShaderEx>();
	flat->Create(L"Shaders/flatShader_vs",
		L"Shaders/flatShader_gs",
		L"Shaders/flatShader_ps");
	return	true;
}

//---------------------------------------------------
//
//		�X�V
//
//---------------------------------------------------
void SceneTestFlatAndFur::imGuiUpdate()
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

		ImGui::Begin(u8"�V�F�[�_�[�e�X�g");      // Create a window called "Hello, world!" and append into it.
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"-------�N���A�J���[-------");
		ImGui::ColorEdit3(u8"�N���A", (float*)&clearColor); // Edit 3 floats representing a color
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------���C�g---------");

		ImGui::ColorEdit3(u8"����", (float*)&ambientColor); // Edit 3 floats representing a color
		ImGui::ColorEdit3(u8"���s��", (float*)&diffuseColor); // Edit 3 floats representing a color
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------�t�@�[---------");
		ImGui::SliderFloat(u8"���x", &density, 1.0f, 2.0f);
		ImGui::SliderFloat(u8"����", &distance, 0.0f, 0.05f);
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

void SceneTestFlatAndFur::Update()
{
	//���C�g�E�J��������ݒ�
	Scene::Operate();

	//imGui
	imGuiUpdate();
	XMFLOAT3 dir;
	dir.x = Light::LightDir.x;
	dir.y = Light::LightDir.y;
	dir.z = Light::LightDir.z;

	Light::SetAmbient(XMFLOAT3(ambientColor));
	Light::SetDirLight(dir, XMFLOAT3(diffuseColor));

	// �s��ݒ�
	static  XMFLOAT3 aY;
	aY.y += pSystem->elapsed_time;

	cube->SetAngle(aY);
	cube->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	cube->SetPos(XMFLOAT3(-2.0f, 2.0f, 0.0f));
	cube->Update();

	sphere->SetAngle(aY);
	sphere->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphere->SetPos(XMFLOAT3(2.0f, 2.0f, 0.0f));
	sphere->Update();

	objmesh->SetAngle(aY);
	objmesh->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	objmesh->SetPos(XMFLOAT3(-2, 0, 0));
	objmesh->Update();

	slime->SetAngle(XMFLOAT3(0,XM_PI,0));
	slime->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	slime->SetPos(XMFLOAT3(2, 0, 0));
	slime->Update();


	plain->SetScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	plain->SetPos(XMFLOAT3(0, 0, 0));
	plain->SetAngle(XMFLOAT3(-XM_PI / 2, 0, 0));
	plain->Update();
}

//---------------------------------------------------
//
//		�`��
//
//---------------------------------------------------
void SceneTestFlatAndFur::Render()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	// �f�t�H���g��ʃN���A
	ID3D11RenderTargetView* rtv[1] = { pSystem->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();
	device_context->OMSetRenderTargets(1, rtv, dsv);
	device_context->ClearRenderTargetView(rtv[0], clearColor);
	device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//�r���[�X�V
	view->Activate();

	// �r���[�|�[�g�̐ݒ�
	pSystem->SetViewPort(pSystem->GetScreenWidth(), pSystem->GetScreenHeight());

	//�u�����h�X�e�[�g�ݒ�
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ALPHA), nullptr, 0xFFFFFFFF);
	//���X�^���C�U�\�ݒ�
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

	{
		sampler_cramp->Set(0);
		// �R���X�^���g�o�b�t�@�ݒ�
		ConstantBufferForPerFrame cb;
		cb.AmbientColor = Light::Ambient;
		cb.LightDir = Light::LightDir;
		cb.LightColor = Light::DirLightColor;
		cb.EyePos.x = view->GetPos().x;
		cb.EyePos.y = view->GetPos().y;
		cb.EyePos.z = view->GetPos().z;
		cb.EyePos.w = 1.0f;

		//�R���X�^���g�o�b�t�@�X�V
		device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
		device_context->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		device_context->GSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());

		plain->Render(phongEx.get(), view->GetView(), view->GetProjection());
	}
	//	�t�@�[�\��
	{
		// �R���X�^���g�o�b�t�@�ݒ�
		ConstantBufferForFur cb{};
		cb.Density = density;
		cb.Distance = distance;

		cb.Force = XMFLOAT4(0.0f, -0.04f, 0.0f, 0.0f);

		//�R���X�^���g�o�b�t�@�X�V
		device_context->UpdateSubresource(fur_constantbuffer.Get(), 0, NULL, &cb, 0, 0);
		device_context->GSSetConstantBuffers(3, 1, fur_constantbuffer.GetAddressOf());
		device_context->PSSetConstantBuffers(3, 1, fur_constantbuffer.GetAddressOf());
	}

	{
		fur_texture->Set(1);
		sampler_wrap->Set(0);
		sampler_wrap->Set(1);
		// �R���X�^���g�o�b�t�@�ݒ�
		ConstantBufferForPerFrame cb;
		cb.AmbientColor = Light::Ambient;
		cb.LightDir = Light::LightDir;
		cb.LightColor = Light::DirLightColor;
		cb.EyePos.x = view->GetPos().x;
		cb.EyePos.y = view->GetPos().y;
		cb.EyePos.z = view->GetPos().z;
		cb.EyePos.w = 1.0f;

		//�R���X�^���g�o�b�t�@�X�V
		device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
		device_context->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		device_context->GSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());

		slime->Render(phongEx.get(), view->GetView(), view->GetProjection());

		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_WRITE_FALSE), 1);

		slime->Render(fur_shader.get(), view->GetView(), view->GetProjection());
		fur_texture->Set(1, false);
		sampler_wrap->Set(0, false);
		sampler_wrap->Set(1, false);
	}
	//	�t���b�g�\��
	{
		sampler_cramp->Set(0);
		// �R���X�^���g�o�b�t�@�ݒ�
		ConstantBufferForPerFrame cb;
		cb.AmbientColor = Light::Ambient;
		cb.LightDir = Light::LightDir;
		cb.LightColor = Light::DirLightColor;
		cb.EyePos.x = view->GetPos().x;
		cb.EyePos.y = view->GetPos().y;
		cb.EyePos.z = view->GetPos().z;
		cb.EyePos.w = 1.0f;

		//�R���X�^���g�o�b�t�@�X�V
		device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
		device_context->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		device_context->GSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

		objmesh->Render(flat.get(), view->GetView(), view->GetProjection());
		sampler_cramp->Set(0, false);
	}

	//imGui
	Scene::imGuiRender();

	//��ʃt���b�v
	pSystem->Flip(1);
}
