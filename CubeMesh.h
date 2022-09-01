#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"
#include "Shader.h"
//--------------------------------------------------------------------
//	CubeMesh�N���X
//--------------------------------------------------------------------
class CubeMesh
{
private:
protected:
	struct VERTEX
	{
		XMFLOAT3 Pos;	//�ʒu
		XMFLOAT3 Normal;//�@��
		XMFLOAT2 Tex;	//UV���W
		XMFLOAT4 Color;	//���_�F
	};
	struct MESH
	{
		ComPtr<ID3D11Buffer> VertexBuffer;
		int iNumVertices;	// ���_���@
		ComPtr<ID3D11Buffer> IndexBuffer;
		int iNumIndices;	// �C���f�b�N�X���@
	};

	MESH Mesh;
	//�e�N�X�`�����p
	std::shared_ptr<Texture> texture = nullptr;
	
	ComPtr<ID3D11Buffer> ConstantBuffer;
	struct ConstantBufferForPerMesh
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 matWVP;


	};


	// ���
	XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 WorldMatrix;


public:
	CubeMesh(const wchar_t* filename = nullptr);
	CubeMesh(std::shared_ptr<Texture>texture);

	~CubeMesh() {}

	void Update();
	void Render(Shader* shader,const XMMATRIX& view, const XMMATRIX& projection);
	void SetPos(const XMFLOAT3& pos) { this->pos = pos; }
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	void SetAngle(const XMFLOAT3& angle) { this->angle = angle; }

	inline XMFLOAT4X4 GetWorldMatrix(){ return WorldMatrix; }
};

