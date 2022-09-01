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
//		�������E�J��
//
//---------------------------------------------------

bool SceneTestGaussianBlur::Initialize()
{
	// ���C�g������
	Light::Init();

	// �R���X�^���g�o�b�t�@
	CreateConstantBuffer(gaussianBlurCB.GetAddressOf(), sizeof(ConstantBufferForPerFrameGaussianBlur));

	// �V�F�[�_�[
	spriteEx = std::make_unique<ShaderEx>();
	spriteEx->Create(L"Shaders/sprite_vs", L"Shaders/sprite_ps");
	gaussianBlur = std::make_unique<ShaderEx>();
	gaussianBlur->Create(L"Shaders/gaussianFilter_vs", L"Shaders/gaussianFilter_ps");

	// �T���v���[
	sampler_cramp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	sampler_wrap = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

	// �e�N�X�`��
	test_texture = std::make_shared<Texture>();
	test_texture->Load(L"ASSETS/Stone_Path_003_baseColor.jpg");

	// 
	blur_texture = std::make_unique<Texture>();
	blur_texture->Create(static_cast<UINT>(test_texture->GetWidth())
						, static_cast<UINT>(test_texture->GetHeight())
						, DXGI_FORMAT_R8G8B8A8_UNORM );

	//�I�u�W�F�N�g
	sprite = std::make_shared<Sprite>();
	return true;
}

//---------------------------------------------------
//
//		�X�V
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
		ImGui::Begin(u8"�V�F�[�_�[�e�X�g");      // Create a window called "Hello, world!" and append into it.
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"-------�N���A�J���[-------");
		ImGui::ColorEdit3(u8"�N���A", clearColor); 

		ImGui::SliderFloat( u8"�W���΍�", &deviation, 1, 20 );
		if( ImGui::SliderInt( u8"�J�[�l���T�C�Y", &karnel_size, 1, KARNEL_SIZE - 1 ) )
		{
			//	�����Ȃ��ɒ���
			if (karnel_size % 2 == 0)
			{
				karnel_size += 1;
			}
		}

		ImGui::SliderFloat(u8"�e�N�Z���T�C�Y", &texcel_size, 1.0f, 8.0f);
		ImGui::CheckboxFlags(u8"�e�N�X�`���E�C���h���J��", &texture_window, 1);

		ImGui::End();
	}

	if (texture_window)
	{
		ImGui::Begin(u8"�e�N�X�`��", &show_another_window);   
		{
			const int w = 512 / 2;
			const int h = 512 / 2;
			ImGui::Text(u8"�X�N���[���摜");
			ImGui::Image(test_texture->GetShaderResourceView(), { w, h });
			ImGui::Separator();
			ImGui::Text(u8"�ڂ����摜");
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
//		�`��
//
//---------------------------------------------------
void SceneTestGaussianBlur::RenderGaussBlur()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	//	�u���[��������
	{
		{
			// �T���v�����O�̈ʒu�I�t�Z�b�g�ƃE�F�C�g���v�Z����
			DirectX::XMFLOAT4 weight[BUFFER_SIZE];
			ZeroMemory(weight,sizeof(weight));
			CalcGaussianFilter(weight, karnel_size, deviation);

			//�R���X�^���g�o�b�t�@�X�V
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
		// ��ʃN���A
		float clearColor[4] = { 0, 0, 0, 0 };
		device_context->ClearRenderTargetView( rtv[0], clearColor );
		// �r���[�|�[�g�̐ݒ�
		pSystem->SetViewPort(blur_texture->GetWidth(), blur_texture->GetHeight() );
		//	�u�����h�X�e�[�g�ݒ�
		device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_NONE), nullptr, 0xFFFFFFFF);
		//���X�^���C�U�\�ݒ�
		device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_NONE));
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 1);
		//�T���v���[�ݒ�
		sampler_cramp->Set(0);
		//�u���[
		sprite->Render( gaussianBlur.get(), test_texture.get(),
						0, 0, pSystem->GetScreenWidth(), pSystem->GetScreenHeight(),
						0, 0, test_texture->GetWidth(), test_texture->GetHeight() );
	}

}

void SceneTestGaussianBlur::Render()
{
	//�K�E�V�A���u���[����
	RenderGaussBlur();

	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	// �f�t�H���g��ʃN���A
	//pSystem->Clear(clearColor);
	ID3D11RenderTargetView* rtv[1] = { pSystem->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = pSystem->GetDepthStencilView();
	device_context->OMSetRenderTargets(1, rtv, dsv);
	device_context->ClearRenderTargetView(rtv[0], clearColor);
	device_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// �r���[�|�[�g�̐ݒ�
	pSystem->SetViewPort(pSystem->GetScreenWidth(), pSystem->GetScreenHeight());
	// �u�����h�X�e�[�g�ݒ�
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_NONE), nullptr, 0xFFFFFFFF);
	// ���X�^���C�U�\�ݒ�
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	// �f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_TRUE), 1);

	// �X�v���C�g�\��
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

	//��ʃt���b�v
	pSystem->Flip(0);
}
