#pragma once
#include <d3d11.h>
#include <vector>

#include "Texture.h"
#include "Shader.h"

class cParticleSystem{
private:
	struct VERTEX
	{
		XMFLOAT3 Pos;	//�ʒu
		XMFLOAT3 Normal;//�@��
		XMFLOAT2 Tex;	//UV���W
		XMFLOAT4 Color;	//���_�F		
		XMFLOAT4 Param;	//�ėp�p�����[�^
	};
	struct ParticleData
	{
		float x, y, z;
		float w, h;
		float aw, ah;
		float vx, vy, vz;
		float ax, ay, az;
		float alpha;
		float timer;
		float anime_timer;
		float type;
	};
	ParticleData* data; //�p�[�e�B�N���f�[�^

	VERTEX* v;			//���_�f�[�^

	int num_particles = 0;
	int komax, komay;
	bool anime;

	// ���_�f�[�^
	ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	// �萔�o�b�t�@
	ComPtr<ID3D11Buffer> ConstantBuffer = nullptr;
	//�e�N�X�`�����p
	std::shared_ptr<Texture> texture = nullptr;
	// �萔�o�b�t�@�̃f�[�^��`
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4	view;			// �r���[�ϊ��s��
		XMFLOAT4X4	projection;		// �����ϊ��s��
		float		size;
		float		dummy0;
		float		dummy1;
		float		dummy2;
	};

public:

	cParticleSystem(int num = 1000);
	cParticleSystem(std::shared_ptr<Texture> texture,int komax = 4,int komay = 4,bool anime = false,int num = 1000);

	~cParticleSystem() { delete[] data; delete[] v; }
	void Update();

	void Animation(float speed = 0.1f);

	void Render(Shader* shader, 
		const XMMATRIX& v, const XMMATRIX& p,
		int blend = DxSystem::BS_ALPHA);

	void Set(
		int type,
		float timer,
		DirectX::XMFLOAT3 p,
		DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(1.0f, 1.0f)
	);

	void Snow(DirectX::XMFLOAT3 pos, int max);

	void Spark(DirectX::XMFLOAT3 pos, int max);
	void Fire(DirectX::XMFLOAT3 pos, int max);
	void Smoke(DirectX::XMFLOAT3 pos, int max);
};