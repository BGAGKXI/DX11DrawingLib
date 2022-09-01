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
static u_int texture_window = false;

///////////////////////////
static float shadowColor[3] = { 0.5f, 0.5f, 0.5f };
static float ShadowErea = 10;

static float bias = 0.0f;
static float deviation = 50.0f;
static int karnel_size = 15;


//---------------------------------------------------
//
//		�������E�J��
//
//---------------------------------------------------

bool SceneTestVSM::Initialize()
{
	//���C�g������
	Light::Init();

	//�R���X�^���g�o�b�t�@
	CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(ConstantBufferForPerFrame));

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

	//�I�u�W�F�N�g
	cube = std::make_unique<CubeMesh>(test_texture);
	sphere = std::make_unique<SphereMesh>(test_texture, 8, 8);
	plain = std::make_unique<PlainMesh>(test_texture, 5, 5);
	objmesh = std::make_unique<ObjMesh>("ASSETS/Mr.Incredible/Mr.Incredible.obj");

	shadowmap = std::make_unique<Texture>();
	shadowmap->Create( SHADOWBUFFER, SHADOWBUFFER, DXGI_FORMAT_R32G32_FLOAT );

	shadowmapVSM = std::make_unique<Texture>();
	shadowmapVSM->Create( SHADOWBUFFER, SHADOWBUFFER, DXGI_FORMAT_R32G32_FLOAT );
	shadowmap_depth = std::make_unique<Texture>();
	shadowmap_depth->CreateDepth( SHADOWBUFFER, SHADOWBUFFER );
	varianceShadowmap = std::make_unique<ShaderEx>();
	varianceShadowmap->Create(L"Shaders/VarianceShadowmap_vs", L"Shaders/VarianceShadowmap_ps");

	gaussianSprite = std::make_shared<Sprite>();
	gaussianBlur = std::make_unique<ShaderEx>();
	gaussianBlur->Create(L"Shaders/gaussianFilter_vs", L"Shaders/gaussianFilter_ps");

	CreateConstantBuffer(&gaussianBlurCB, sizeof(ConstantBufferForPerFrameGaussianBlur));
	CreateConstantBuffer(&shadowmapCB, sizeof(ConstantBufferForPerFrameShadowmap));
	float	boarderColor[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
	sampler_shadow = std::make_shared<Sampler>( D3D11_FILTER_MIN_MAG_MIP_LINEAR,
												D3D11_TEXTURE_ADDRESS_BORDER,
												boarderColor );

	phongVSM = std::make_unique<ShaderEx>();
	phongVSM->Create(L"Shaders/3DObj_VSM_vs", L"Shaders/3DObj_VSM_ps");
	return true;
}

//---------------------------------------------------
//
//		�X�V
//
//---------------------------------------------------
void SceneTestVSM::imGuiUpdate()
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
		ImGui::CheckboxFlags(u8"�e�N�X�`���E�C���h���J��", &texture_window, 1);

		ImGui::ColorEdit3(u8"�V���h�E�J���[", (float*)&shadowColor); // Edit 3 floats representing a color
		
		ImGui::SliderFloat(u8"�V���h�[�G���A", &ShadowErea, 10, 100);

		ImGui::SliderFloat(u8"�V���h�E�o�C�A�X", &bias, 0.0f, 1.0f, "%0.0f" );
		ImGui::SliderFloat( u8"�W���΍�", &deviation, 1, 100 );
		if( ImGui::SliderInt( u8"�J�[�l���T�C�Y", &karnel_size, 1, KARNEL_SIZE - 1 ) )
		{
			//	�����Ȃ��ɒ���
			if (karnel_size % 2 == 0)
			{
				karnel_size += 1;
			}
		}

		ImGui::End();
	}

	if (texture_window)
	{
		ImGui::Begin(u8"�e�N�X�`��", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		{
			const int w = SHADOWBUFFER / 2;
			const int h = SHADOWBUFFER / 2;
			ImGui::Text("shadowmap");
			ImGui::Image(shadowmap->GetShaderResourceView(), { w, h });
			ImGui::Separator();
			ImGui::Text("VarianceShadowmap");
			ImGui::Image(shadowmapVSM->GetShaderResourceView(), { w, h });
		}

		ImGui::End();
	}
}

void SceneTestVSM::Update()
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

	{
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
		objmesh->SetScale(XMFLOAT3(0.25f, 0.25f, 0.25f));
		objmesh->SetPos(XMFLOAT3(0, 0, 0));
		objmesh->Update();

		plain->SetScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
		plain->SetPos(XMFLOAT3(0, 0, 0));
		plain->SetAngle(XMFLOAT3(-XM_PI / 2, 0, 0));
		plain->Update();
	}
}

void SceneTestVSM::CalcGaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma)
{
	float sum = 0.0f;
	int id = 0;
	for (int y = -karnel_size / 2; y <= karnel_size / 2; y++) {
		for (int x = -karnel_size / 2; x <= karnel_size / 2; x++) {
			array[id].x = (float)x;
			array[id].y = (float)y;
			array[id].z = (float)exp(-(x * x + y * y) / (2.0f * sigma * sigma) ) / (2.0f * PI * sigma);
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
//		�`��
//
//---------------------------------------------------
void SceneTestVSM::RenderShadow()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	//	�V���h�E�}�b�v����
	{
		// �r���[�̐ݒ�
		View v;
		{
			XMFLOAT3 t(0, 0, 0);
			XMFLOAT3 p;
			p.x = t.x - LightDir.x * 30.0f;
			p.y = t.y - LightDir.y * 30.0f;
			p.z = t.z - LightDir.z * 30.0f;
			v.Set(p, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0));
			// ���s���e
			v.SetOrtho(ShadowErea, ShadowErea, 1.0f, 100.0f);
			v.Activate();
			//���C�g���PV�s��
			XMStoreFloat4x4(&LightViewProjection, v.GetView()*v.GetProjection());
		}
		ID3D11RenderTargetView* rtv[1] = { shadowmap->GetRenderTargetView() };
		ID3D11DepthStencilView* dsv = shadowmap_depth->GetDepthStencilView();
		device_context->OMSetRenderTargets(1, rtv, dsv);
		// ��ʃN���A
		float clearColor[4] = { 1.0f,1.0f,1.0f,1.0f };
		device_context->ClearRenderTargetView(rtv[0], clearColor);
		device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		// �r���[�|�[�g�̐ݒ�
		pSystem->SetViewPort( SHADOWBUFFER, SHADOWBUFFER );
		//�u�����h�X�e�[�g�ݒ�
		device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ALPHA), nullptr, 0xFFFFFFFF);
		//���X�^���C�U�\�ݒ�
		device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

		//	�I�u�W�F�N�g�a��
 		cube->Render(varianceShadowmap.get(), v.GetView(), v.GetProjection());
		sphere->Render(varianceShadowmap.get(), v.GetView(), v.GetProjection());
		objmesh->Render(varianceShadowmap.get(), v.GetView(), v.GetProjection());
		plain->Render(varianceShadowmap.get(), v.GetView(), v.GetProjection());
	}
	//	�u���[��������
	{
		{
			// �T���v�����O�̈ʒu�I�t�Z�b�g�ƃE�F�C�g���v�Z����
			DirectX::XMFLOAT4 weight[BUFFER_SIZE];
			ZeroMemory(weight,sizeof(weight));
			CalcGaussianFilter(weight, karnel_size, deviation);

			ConstantBufferForPerFrameGaussianBlur cb;
			memcpy(cb.Weight, weight, sizeof(DirectX::XMFLOAT4) * BUFFER_SIZE);
			cb.KarnelSize = static_cast<float>(karnel_size);
			cb.texcel.x	= 1.0f / static_cast<float>( SHADOWBUFFER );
			cb.texcel.y	= 1.0f / static_cast<float>( SHADOWBUFFER );
			//�R���X�^���g�o�b�t�@�X�V
			device_context->UpdateSubresource(gaussianBlurCB.Get(), 0, NULL, &cb, 0, 0);
			device_context->VSSetConstantBuffers(3, 1, gaussianBlurCB.GetAddressOf());
			device_context->PSSetConstantBuffers(3, 1, gaussianBlurCB.GetAddressOf());
		}
		ID3D11RenderTargetView* rtv[1] = { shadowmapVSM->GetRenderTargetView() };
		ID3D11DepthStencilView* dsv = shadowmap_depth->GetDepthStencilView();
		device_context->OMSetRenderTargets(1, rtv, dsv);
		// ��ʃN���A
		float clearColor[4] = { FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX };
		device_context->ClearRenderTargetView(rtv[0], clearColor);
		device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		// �r���[�|�[�g�̐ݒ�
		pSystem->SetViewPort( SHADOWBUFFER, SHADOWBUFFER );
		//	�u�����h�X�e�[�g�ݒ�
		device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_NONE), nullptr, 0xFFFFFFFF);
		//���X�^���C�U�\�ݒ�
		device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_NONE));
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 1);
		//�T���v���[�ݒ�
		sampler_cramp->Set(0);
		//�u���[
		gaussianSprite->Render(gaussianBlur.get(), shadowmap.get(), 0, 0, 1280, 720, 0, 0, SHADOWBUFFER, SHADOWBUFFER );
	}

}

void SceneTestVSM::Render()
{
	//	�V���h�E�}�b�v����
	RenderShadow();

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
	device_context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());

	//�u�����h�X�e�[�g�ݒ�
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ALPHA), nullptr, 0xFFFFFFFF);
	//���X�^���C�U�\�ݒ�
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

	//	�V���h�E�}�b�v�p�����[�^�[�ݒ�
	{
		ConstantBufferForPerFrameShadowmap cb;
		cb.LightViewProjection = LightViewProjection;
		cb.ShadowParameter = { shadowColor[0], shadowColor[1], shadowColor[2], bias };
		device_context->UpdateSubresource(shadowmapCB.Get(), 0, NULL, &cb, 0, 0);
		device_context->VSSetConstantBuffers(3, 1, shadowmapCB.GetAddressOf());
		device_context->PSSetConstantBuffers(3, 1, shadowmapCB.GetAddressOf());

		//�V���h�E�}�b�v�ݒ�
		shadowmapVSM->Set(5);
		sampler_cramp->Set(5);
		sampler_shadow->Set(5);
	}
	{
		//�T���v���[�ݒ�
		sampler_cramp->Set(0);
		cube->Render(phongVSM.get(), view->GetView(), view->GetProjection());
		sphere->Render(phongVSM.get(), view->GetView(), view->GetProjection());
		plain->Render(phongVSM.get(), view->GetView(), view->GetProjection());
		objmesh->Render(phongVSM.get(), view->GetView(), view->GetProjection());
	}
	//{
	//	shadowmapVSM->Set(5, FALSE);
	//	sampler_shadow->Set(5, FALSE);
	//}
	//imGui
	Scene::imGuiRender();

	//��ʃt���b�v
	pSystem->Flip(1);
}
