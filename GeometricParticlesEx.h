#pragma once
#include <d3d11.h>

#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"
#include "Shader.h"

class GeometricParticlesEx
{
private:
	// 頂点データ
	struct VERTEX
	{
		XMFLOAT3 Position;      // 座標値
		XMFLOAT3 Velocity;      // 現在の速度
		XMFLOAT3 VelocityFirst; // 最初の速度
		XMFLOAT3 Acceleration;	// 加速度

	};

	// 定数バッファのデータ定義
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4 View;			// ビュー変換行列
		XMFLOAT4X4 Projection;		// 透視変換行列
		XMFLOAT2   ParticleSize;	// パーティクルの大きさ
		FLOAT      Time;			// 経過時間(秒)
		FLOAT      dummy;			// ダミー
	};
	// 定数バッファのデータ
	ConstantBufferForPerFrame m_CBData;

	ComPtr<ID3D11VertexShader>		VS[2] = { nullptr }; // 頂点シェーダ
	ComPtr<ID3D11PixelShader>		PS[2] = { nullptr }; // ピクセルシェーダ
	ComPtr<ID3D11GeometryShader>	GS[2] = { nullptr }; // ジオメトリシェーダ

	int numVertexes;
	ID3D11Buffer* VerBufferDrawFrom = NULL;  // 頂点バッファ(計算前)のインターフェイス
	ID3D11Buffer* VerBufferStreamTo = NULL;  // 頂点バッファ(計算後)のインターフェイス
	ComPtr<ID3D11Buffer>	  CBuffer = NULL;


	ComPtr<ID3D11InputLayout>		VertexLayout;

	Texture* particleTex = nullptr;
	

	//シェーダー単体コンパイル
	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	HRESULT CreateShaderParticleVertex(void);
	HRESULT CreateShaderParticle(void);


	void RenderParticle(void);
	void RenderParticleVertex(void);

	static bool m_first;
	// 追加
	HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, 
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
	HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
	HRESULT create_gs_stream_from_cso(
		ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader,
		D3D11_SO_DECLARATION_ENTRY* Decl, UINT Decl_size, UINT* pBufferStrides, UINT buffer_size);

		HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);


public:
	GeometricParticlesEx(int num = 1000);

	void Update();
	void Render(const XMMATRIX* v, const XMMATRIX* p);

	void Set() { m_first = true; }



};