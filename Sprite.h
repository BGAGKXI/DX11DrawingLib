#pragma once
#include <d3d11.h>
#include "Shader.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture.h"


class Sprite
{
private:
protected:
	ComPtr<ID3D11Buffer> VertexBuffer;
	std::shared_ptr<Texture> texture = nullptr;
	ComPtr<ID3D11DepthStencilState> DepthStencilState ;
	struct VERTEX
	{
		XMFLOAT3 Pos;	//位置
		XMFLOAT3 Normal;//法線
		XMFLOAT2 Tex;	//UV座標
		XMFLOAT4 Color;	//頂点色
	};
public:
	Sprite();
	Sprite(std::shared_ptr<Texture> texture);
	Sprite(const wchar_t* filename);
	virtual ~Sprite();

	void Render(Shader* shader,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle = 0.0f,
		float alpha = 1.0f
	);
	void Render(Shader* shader,
		Texture* tex,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle = 0.0f,
		float alpha = 1.0f
	);


	ID3D11RenderTargetView* GetRenderTargetView() { return texture->GetRenderTargetView(); }
	void SetTexture(u_int slot) { texture->Set(slot); }

};

