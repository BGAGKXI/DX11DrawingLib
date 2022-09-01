#pragma once
#include "DirectXTex\DirectXTex.h"


class Texture
{
protected:
	ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	Texture();
	virtual ~Texture();
	bool Load(const wchar_t* filename = nullptr);
	bool LoadMipMap(const wchar_t* filename = nullptr);

	void Set(UINT Slot = 0, BOOL flg = TRUE);

	float GetWidth() { return static_cast<float>(texture2d_desc.Width); }
	float GetHeight() { return static_cast<float>(texture2d_desc.Height); }
	bool Create(u_int width, u_int height, DXGI_FORMAT format);
	bool CreateMipMap(u_int width, u_int height, DXGI_FORMAT format,float MipLevel = 1);

	// 深度ステンシルバッファ作成
	bool CreateDepth(u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);


	ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
	ID3D11ShaderResourceView* GetShaderResourceView() { return ShaderResourceView.Get(); }
	//CubeMap用
	bool CreateCube(u_int width, u_int height, DXGI_FORMAT format,int miplevel = 1);
	bool CreateCubeDepthStencil(u_int width, u_int height);


};
