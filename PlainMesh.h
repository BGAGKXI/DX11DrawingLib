#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"
#include "Shader.h"

//--------------------------------------------------------------------
//	PlainMeshクラス
//--------------------------------------------------------------------

class PlainMesh
{
private:
		struct VERTEX
	{
		XMFLOAT3 Pos;	//位置
		XMFLOAT3 Normal;//法線
		XMFLOAT2 Tex;	//UV座標
		XMFLOAT4 Color;	//頂点色
	};
	ComPtr<ID3D11Buffer> VertexBuffer;
	int iNumVertices;	// 頂点数　
	ComPtr<ID3D11Buffer> IndexBuffer;
	int iNumIndices;	// インデックス数　

	//テクスチャ利用
	std::shared_ptr<Texture> texture;

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
	PlainMesh(int row, int col);
	PlainMesh(const wchar_t* filename,int row, int col);
	PlainMesh(std::shared_ptr<Texture> texture,int row, int col);

	~PlainMesh();
	void Update();
	void Render(Shader* shader, const XMMATRIX& view, const XMMATRIX& projection, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Render(Shader* shader, Texture* texture, const XMMATRIX& view, const XMMATRIX& projection, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void SetPos(const XMFLOAT3& pos) { this->pos = pos; }
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	void SetAngle(const XMFLOAT3& angle) { this->angle = angle; }

	inline XMFLOAT4X4 GetWorldMatrix(){ return WorldMatrix; }
};