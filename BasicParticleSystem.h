#pragma once
#include <d3d11.h>

#include "Texture.h"
#include "Shader.h"

class cBasicParticleSystem
{
private:
		struct VERTEX
	{
		XMFLOAT3 Pos;	//位置
		XMFLOAT3 Normal;//法線
		XMFLOAT2 Tex;	//UV座標
		XMFLOAT4 Color;	//頂点色
	};
	struct ParticleData
	{
		float x, y, z;
		float w, h;

	};

	std::vector<VERTEX> vertices;	//頂点データ

	// 頂点データ
	ComPtr<ID3D11Buffer> VertexBuffer;
	// 定数バッファ
	ComPtr<ID3D11Buffer> ConstantBuffer;
	//テクスチャ利用
	std::shared_ptr<Texture> texture = nullptr;
	// 定数バッファのデータ定義
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4	view;			// ビュー変換行列
		XMFLOAT4X4	projection;		// 透視変換行列
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