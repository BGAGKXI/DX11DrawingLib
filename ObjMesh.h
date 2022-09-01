#pragma once
#include <stdio.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"
#include "Shader.h"

//--------------------------------------------------------------------
//	Meshクラス
//--------------------------------------------------------------------


//頂点の構造体
struct MY_VERTEX
{
	XMFLOAT3 vPos;
	XMFLOAT3 vNorm;
	XMFLOAT2 vTex;
};

struct VERTEX
{
	XMFLOAT3 vPos;
	XMFLOAT3 vNorm;
	XMFLOAT2 vTex;
};

struct SIMPLECONSTANT_BUFFER0
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 matWVP;


};

struct CBUFF_PER_MATERIAL//マテリアル単位で渡す情報
{
	XMFLOAT4 vAmbient;//アンビエント光
	XMFLOAT4 vDiffuse;//ディフューズ色
	XMFLOAT4 vSpecular;//鏡面反射
};
//オリジナル　マテリアル構造体
struct MY_MATERIAL
{
	CHAR szName[110];
	CHAR szTextureName[110];//テクスチャーファイル名
	CHAR szBumpTextureName[110];//Bumpテクスチャーファイル名
	XMFLOAT4 Ka;//アンビエント
	XMFLOAT4 Kd;//ディフューズ
	XMFLOAT4 Ks;//スペキュラー

	std::unique_ptr<Texture> tex = nullptr;

	DWORD dwNumFace;//そのマテリアルであるポリゴン数
	MY_MATERIAL()
	{
		ZeroMemory(this,sizeof(MY_MATERIAL));
	}
	~MY_MATERIAL()
	{
	}
};

//
//
//
class ObjMesh
{
private:
	DWORD m_dwNumVert = 0;
	DWORD m_dwNumFace = 0;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ID3D11Buffer** m_ppIndexBuffer;

	ComPtr<ID3D11Buffer> ConstantBuffer;
	ComPtr<ID3D11Buffer> ConstantBufferPerMaterial;

	DWORD m_dwNumMaterial;

	char filepass[200] = { 0 };//OBJ/MTLパス名

	//std::unique_ptr<MY_MATERIAL>m_pMaterial;
		
	//情報
	XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	XMFLOAT3 scale = { 5.0f, 5.0f, 5.0f };
	XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 WorldMatrix;
	HRESULT LoadMaterialFromFile(LPSTR FileName, MY_MATERIAL** ppMaterial);
	HRESULT LoadStaticMesh(const char* FileName);

public:
	MY_MATERIAL* m_pMaterial;

	ObjMesh() {}
	ObjMesh(const char* FileName);
	~ObjMesh();
	void Render(Shader* shader, const XMMATRIX& view, const XMMATRIX& projection, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//行列更新
	void Update();
	void SetPos(const XMFLOAT3& pos) { this->pos = pos; }
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	void SetAngle(const XMFLOAT3& angle) { this->angle = angle; }
	XMFLOAT3 GetPos() { return pos; }

	inline XMFLOAT4X4 GetWorldMatrix(){ return WorldMatrix; }

};