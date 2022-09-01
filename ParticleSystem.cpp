#include "DxSystem.h"
#include "ParticleSystem.h"

cParticleSystem::cParticleSystem(int num)
{
	ID3D11Device* device = pSystem->GetDevice();

	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData)*num);

	v = new VERTEX[num];
	ZeroMemory(v, sizeof(VERTEX)*num);

	num_particles = num;
	for (int i = 0; i < num_particles; i++) { data[i].type = -1;}
	//�p�[�e�B�N���쐬�Ɖ摜���[�h
	texture = std::make_unique<Texture>();
	texture->Load(L"Assets/Explosion01_5x5.png");

	//���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//���_�����̃o�b�t�@
	bd.ByteWidth = sizeof(VERTEX)* num_particles; 
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

cParticleSystem::cParticleSystem(std::shared_ptr<Texture> texture,int komax,int komay, bool anime,int num)
{
	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData)*num);
	v = new VERTEX[num];
	ZeroMemory(v, sizeof(VERTEX)*num);

	this->komax = komax;
	this->komay = komay;
	this->anime = anime;

	num_particles = num;
	for (int i = 0; i < num_particles; i++) { data[i].type = -1; }
	//�p�[�e�B�N���쐬�Ɖ摜�ݒ�
	this->texture = texture;

	ID3D11Device* device = pSystem->GetDevice();

	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//���_�����̃o�b�t�@
	bd.ByteWidth = sizeof(VERTEX)* num_particles;
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
void cParticleSystem::Animation(float speed)
{
	float time = pSystem->elapsed_time;
	for (int i = 0; i < num_particles; i++) {
		if (data[i].type < 0) continue;
	
		data[i].type += time*speed; // speed�R�}/�b
	}
}


void cParticleSystem::Update()
{
	float time = pSystem->elapsed_time;
	for (int i = 0; i < num_particles; i++) {
		if (data[i].type < 0) continue;

		data[i].vx += data[i].ax * time;
		data[i].vy += data[i].ay * time;
		data[i].vz += data[i].az * time;

		data[i].x += data[i].vx * time;
		data[i].y += data[i].vy * time;
		data[i].z += data[i].vz * time;

		data[i].timer -= time;
		data[i].alpha = sqrtf(data[i].timer);
		if (data[i].timer <= 0) data[i].type = -1;
	}
	if(anime) Animation(30.0f);
}

void cParticleSystem::Render(
	Shader* shader, const XMMATRIX& view, const XMMATRIX& projection,
	int blend)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	//�V�F�[�_�L����
	shader->Activate();

	//�萔�o�b�t�@�̍X�V
	ConstantBufferForPerFrame cb;
	XMFLOAT4X4 fv,fp;
	XMStoreFloat4x4(&fv, view);
	cb.view = fv;
	XMStoreFloat4x4(&fp, projection);
	cb.projection = fp;
	cb.size = 0.1f;
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

	int n = 0; //�p�[�e�B�N��������
	for (int i = 0; i < num_particles; i++)
	{
		if (data[i].type < 0) continue;

		v[n].Pos.x = data[i].x;
		v[n].Pos.y = data[i].y;
		v[n].Pos.z = data[i].z;
		v[n].Tex.x = data[i].w;
		v[n].Tex.y = data[i].h;		  
		v[n].Color.x = v[n].Color.y = v[n].Color.z = 1.0f;
		v[n].Color.w = data[i].alpha;
		
		v[n].Param.x = 0;
		v[n].Param.y = data[i].type;
		v[n].Param.z = (float)komax; //���R�}��
		v[n].Param.w = (float)komay; //�c�R�}��
		++n;
	}
	//���_�f�[�^�X�V
	device_context->UpdateSubresource(VertexBuffer.Get(), 0, nullptr, v, 0, 0);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);

	//�����_�����O(�C���f�b�N�X�t��)
	device_context->Draw(n, 0);

	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 1);

	//�V�F�[�_������
	shader->Inactivate();
}

void cParticleSystem::Set(int type,float timer,
	DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size)
{
	for (int i = 0; i < num_particles; i++) {
		if (data[i].type >= 0) continue;
		data[i].type = (float)type;
		data[i].x = p.x;
		data[i].y = p.y;
		data[i].z = p.z;
		data[i].vx = v.x;
		data[i].vy = v.y;
		data[i].vz = v.z;
		data[i].ax = f.x;
		data[i].ay = f.y;
		data[i].az = f.z;
		data[i].w = size.x;
		data[i].h = size.y;
		data[i].alpha = 1.0f;
		data[i].timer = timer;

		break;
	}
}

void cParticleSystem::Snow(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		//�����ʒu
		DirectX::XMFLOAT3 p = { 0,0,0 };
		p.x = pos.x + (rand() % 10001 - 5000) * 0.01f;
		p.y = pos.y;
		p.z = pos.z + (rand() % 10001 - 5000) * 0.01f;
		//��������
		DirectX::XMFLOAT3 v = { 0,0,0 };
		v.y = -(rand() % 10001) * 0.0002f - 0.002f;
		//��
		DirectX::XMFLOAT3 f = { 0,0,0 };
		f.x = (rand() % 10001) * 0.00001f + 0.1f;
		f.z = (rand() % 10001 - 5000) * 0.00001f;
		//�傫��
		DirectX::XMFLOAT2 s = { .2f,.2f };

		Set(12, 5, p, v, f, s);
	}
}

void cParticleSystem::Spark(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		p.x = pos.x;
		p.y = pos.y;
		p.z = pos.z;

		DirectX::XMFLOAT3 v = { 0,0,0 };
		v.x = (rand() % 10001 - 5000) * 0.0001f;
		v.y = (rand() % 10001) * 0.0002f + 1.2f;
		v.z = (rand() % 10001 - 5000) * 0.0001f;

		DirectX::XMFLOAT3 f = { 0,-1.2f,0 };
		DirectX::XMFLOAT2 s = { 0.05f,0.05f };

		Set(2, 3, p, v, f, s);

	}
}
void cParticleSystem::Fire(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		DirectX::XMFLOAT3 v = { 0,0,0 };
		DirectX::XMFLOAT3 f = { 0,-2.0f,0 };
		DirectX::XMFLOAT2 s = { 0.8f,0.8f };

		p.x = pos.x + (rand() % 10001 - 5000) * 0.00005f;
		p.y = pos.y + (rand() % 10001) * 0.0001f + 0.2f;
		p.z = pos.z + (rand() % 10001 - 5000) * 0.00005f;

		v.x = (rand() % 10001 - 5000) * 0.0002f;
		v.y = (rand() % 10001) * 0.0004f + 0.005f;
		v.z = (rand() % 10001 - 5000) * 0.0002f;

		Set(13, 0.4f, p, v, f, s);

	}
}

void cParticleSystem::Smoke(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		DirectX::XMFLOAT3 v = { 0,0,0 };
		DirectX::XMFLOAT3 f = { 0,-2.0f,0 };
		DirectX::XMFLOAT2 s = { .4f,.4f };

		p.x = pos.x + (rand() % 10001 - 5000) * 0.0001f;
		p.y = pos.y + (rand() % 10001) * 0.0001f + 0.5f;
		p.z = pos.z + (rand() % 10001 - 5000) * 0.0001f;

		v.x = (rand() % 10001 - 5000) * 0.0001f;
		v.y = (rand() % 10001) * 0.0004f + 0.005f;
		v.z = (rand() % 10001 - 5000) * 0.0001f;
		Set(14, 0.6f, p, v, f, s);

	}
}




