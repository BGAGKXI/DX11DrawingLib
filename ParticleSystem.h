#pragma once
#include <d3d11.h>
#include <vector>

#include "Texture.h"
#include "Shader.h"

class cParticleSystem{
private:
	struct VERTEX
	{
		XMFLOAT3 Pos;	//位置
		XMFLOAT3 Normal;//法線
		XMFLOAT2 Tex;	//UV座標
		XMFLOAT4 Color;	//頂点色		
		XMFLOAT4 Param;	//汎用パラメータ
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
	ParticleData* data; //パーティクルデータ

	VERTEX* v;			//頂点データ

	int num_particles = 0;
	int komax, komay;
	bool anime;

	// 頂点データ
	ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	// 定数バッファ
	ComPtr<ID3D11Buffer> ConstantBuffer = nullptr;
	//テクスチャ利用
	std::shared_ptr<Texture> texture = nullptr;
	// 定数バッファのデータ定義
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4	view;			// ビュー変換行列
		XMFLOAT4X4	projection;		// 透視変換行列
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