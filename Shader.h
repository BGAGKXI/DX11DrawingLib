#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

class Shader
{
public:
protected:
	ComPtr<ID3D11VertexShader>		VS = nullptr; // 頂点シェーダ
	ComPtr<ID3D11PixelShader>		PS = nullptr; // ピクセルシェーダ
	ComPtr<ID3D11GeometryShader>	GS = nullptr; // ジオメトリシェーダ
	ComPtr<ID3D11HullShader>		HS = nullptr; //　ハルシェーダ
	ComPtr<ID3D11DomainShader>		DS = nullptr;//　ドメインネームシェーダ


	ComPtr<ID3D11InputLayout>		VertexLayout = nullptr;

	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
	

public:
	Shader() {}
	virtual ~Shader();

	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);

	void Activate();
	void Inactivate();
};


//----------------------------------------------
//
//	頂点データ構造体定義
//
//----------------------------------------------

//struct VERTEX
//{
//	XMFLOAT3 Pos;	//位置
//	XMFLOAT3 Normal;//法線
//	XMFLOAT2 Tex;	//UV座標
//	XMFLOAT4 Color;	//頂点色
//};