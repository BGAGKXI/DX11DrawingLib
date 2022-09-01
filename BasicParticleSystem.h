#pragma once
#include <d3d11.h>

#include "Texture.h"
#include "Shader.h"

class cBasicParticleSystem
{
private:
		struct VERTEX
	{
		XMFLOAT3 Pos;	//�ʒu
		XMFLOAT3 Normal;//�@��
		XMFLOAT2 Tex;	//UV���W
		XMFLOAT4 Color;	//���_�F
	};
	struct ParticleData
	{
		float x, y, z;
		float w, h;

	};

	std::vector<VERTEX> vertices;	//���_�f�[�^

	// ���_�f�[�^
	ComPtr<ID3D11Buffer> VertexBuffer;
	// �萔�o�b�t�@
	ComPtr<ID3D11Buffer> ConstantBuffer;
	//�e�N�X�`�����p
	std::shared_ptr<Texture> texture = nullptr;
	// �萔�o�b�t�@�̃f�[�^��`
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4	view;			// �r���[�ϊ��s��
		XMFLOAT4X4	projection;		// �����ϊ��s��
	};
public:

	cBasicParticleSystem(int num = 1000);
	cBasicParticleSystem(std::shared_ptr<Texture> texture, int num = 1000);
	~cBasicParticleSystem() {  }


	void Render(Shader* shader,
		const XMMATRIX& v, const XMMATRIX& p,
		int blend = DxSystem::BS_ALPHA);

	void Set( DirectX::XMFLOAT3 p);

};