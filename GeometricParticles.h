#pragma once
#include <d3d11.h>

#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"
#include "Shader.h"

class GeometricParticles
{
private:
	// ���_�f�[�^
	struct VERTEX
	{
		XMFLOAT3 Position;      // ���W�l
		XMFLOAT3 Velocity;      // ���݂̑��x
		XMFLOAT3 VelocityFirst; // �ŏ��̑��x
		XMFLOAT3 Acceleration; // �����x

	};

	// �萔�o�b�t�@�̃f�[�^��`
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4 View;       // �r���[�ϊ��s��
		XMFLOAT4X4 Projection; // �����ϊ��s��
		XMFLOAT2   ParticleSize;  // �p�[�e�B�N���̑傫��
		FLOAT      Time;          // �o�ߎ���(�b)
		FLOAT      dummy;      // �_�~�[
	};
	// �萔�o�b�t�@�̃f�[�^
	ConstantBufferForPerFrame m_CBData;

	ComPtr<ID3D11VertexShader>		VS[2] = { nullptr }; // ���_�V�F�[�_
	ComPtr<ID3D11PixelShader>		PS[2] = { nullptr }; // �s�N�Z���V�F�[�_
	ComPtr<ID3D11GeometryShader>	GS[2] = { nullptr }; // �W�I���g���V�F�[�_

	int numVertexes;
	ID3D11Buffer*             VerBufferDrawFrom = NULL;  // ���_�o�b�t�@(�v�Z�O)�̃C���^�[�t�F�C�X
	ID3D11Buffer*             VerBufferStreamTo = NULL;  // ���_�o�b�t�@(�v�Z��)�̃C���^�[�t�F�C�X
	ComPtr<ID3D11Buffer>	  CBuffer = NULL;


	ComPtr<ID3D11InputLayout>		VertexLayout;

	Texture* particleTex = nullptr;

	//�V�F�[�_�[�P�̃R���p�C��
	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	HRESULT CreateShaderParticleVertex(void);
	HRESULT CreateShaderParticle(void);


	void RenderParticle(void);
	void RenderParticleVertex(void);

	static bool m_first;

public:
	GeometricParticles(int num=1000);
	void Render(const XMMATRIX* v, const XMMATRIX* p);

	void Set() { m_first = true; }



};