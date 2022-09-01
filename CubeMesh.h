#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"
#include "Shader.h"
//--------------------------------------------------------------------
//	CubeMeshクラス
//--------------------------------------------------------------------
class CubeMesh
{
private:
protected:
	struct VERTEX
	{
		XMFLOAT3 Pos;	//位置
		XMFLOAT3 Normal;//法線
		XMFLOAT2 Tex;	//UV座標
		XMFLOAT4 Color;	//頂点色
	};
	struct MESH
	{
		ComPtr<ID3D11Buffer> VertexBuffer;
		int iNumVertices;	// 頂点数　
		ComPtr<ID3D11Buffer> IndexBuffer;
		int iNumIndices;	// インデックス数　
	};

	MESH Mesh;
	//テクスチャ利用
	std::shared_ptr<Texture> texture = nullptr;
	
	ComPtr<ID3D11Buffer> ConstantBuffer;
	struct ConstantBufferForPerMesh
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 matWVP;


	};


	// 情報
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

