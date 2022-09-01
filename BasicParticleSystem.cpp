#include "DxSystem.h"
#include "BasicParticleSystem.h"

cBasicParticleSystem::cBasicParticleSystem(int num)
{
	ID3D11Device* device = pSystem->GetDevice();

	vertices.resize(num);
	//�p�[�e�B�N���쐬�Ɖ摜���[�h
	texture = std::make_unique<Texture>();
	texture->Load(L"Assets/particle1.png");

	//���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//���_�����̃o�b�t�@
	bd.ByteWidth = sizeof(VERTEX)*num;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}

cBasicParticleSystem::cBasicParticleSystem(std::shared_ptr<Texture> texture, int num)
{
	//���_�f�[�^�쐬
	vertices.resize(num);
	//�p�[�e�B�N���쐬�Ɖ摜�ݒ�
	this->texture = texture;

	ID3D11Device* device = pSystem->GetDevice();
	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//���_�����̃o�b�t�@
	bd.ByteWidth = sizeof(VERTEX)*num;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}


void cBasicParticleSystem::Render(
	Shader* shader, const XMMATRIX& view, const XMMATRIX& projection,
	int blend)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	//�V�F�[�_�L����
	shader->Activate();

	//�萔�o�b�t�@�̍X�V
	ConstantBufferForPerFrame cb;
	XMFLOAT4X4 fv, fp;
	XMStoreFloat4x4(&fv, view);
	cb.view = fv;
	XMStoreFloat4x4(&fp, projection);
	cb.projection = fp;
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());

	//�u�����h�X�e�[�g�ݒ�
	device_context->OMSetBlendState(pSystem->GetBlendState(blend), nullptr, 0xFFFFFFFF);
	//���X�^���C�U�\�ݒ�
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_WRITE_FALSE), 1);
	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//�e�N�X�`���ݒ�
	if (texture) texture->Set(0);


	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
	//�����_�����O(�C���f�b�N�X�t��)
	device_context->Draw(vertices.size(), 0);
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 1);
	//�V�F�[�_������
	shader->Inactivate();
}

void cBasicParticleSystem::Set( DirectX::XMFLOAT3 p )
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	for (auto& v :vertices) 
	{
		v.Pos.x = (rand() % 1001 - 500) * 0.01f;
		v.Pos.y = (rand() % 1001 - 500) * 0.01f;
		v.Pos.z = (rand() % 1001 - 500) * 0.01f;
		v.Color = {1,1,1,1};
	}
	//���_�f�[�^�X�V
	device_context->UpdateSubresource(VertexBuffer.Get(), 0, nullptr, vertices.data(), 0, 0);
}


