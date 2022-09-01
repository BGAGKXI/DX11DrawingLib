#include "DxSystem.h"
#include <d3d11.h>
#include "Texture.h"
#include "Sampler.h"
#if _DEBUG
#pragma comment ( lib, "DirectXTex/DirectXTexD.lib")
#else
#pragma comment ( lib, "DirectXTex/DirectXTex.lib")
#endif
//#include "WICTextureLoader.cpp"


Texture::Texture() :ShaderResourceView(nullptr)
{
}



Texture::~Texture()
{
}


bool Texture::Load(const wchar_t* filename)
{
	ID3D11Device* device = pSystem->GetDevice();


	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = LoadFromWICFile(filename, 0, &metadata, image);

	assert(SUCCEEDED(hr));

	// 画像からシェーダリソースView
	hr = DirectX::CreateShaderResourceView(
		device,
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	//テクスチャデータ取得
	texture2d_desc.Width = metadata.width;
	texture2d_desc.Height = metadata.height;


	return true;
}



bool Texture::LoadMipMap(const wchar_t* filename)
{
	ID3D11Device* device = pSystem->GetDevice();
	HRESULT hr = S_OK;

	size_t wLen = 0;
	errno_t err = 0;


	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	if (_wcsnicmp(&filename[wcslen(filename) - 3], L"TGA",3) == 0)	
	{
		hr = LoadFromTGAFile(filename, &metadata, image);
	}
	else {
		hr = LoadFromWICFile(filename, DirectX::WIC_FLAGS_NONE, &metadata, image);
	}

	if (FAILED(hr)) {
		return false;
	}

	//テクスチャデータ取得
	texture2d_desc.Width = metadata.width;
	texture2d_desc.Height = metadata.height;

	//ミップマップ作成
	DirectX::ScratchImage mipChain;
	DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

	// 画像からシェーダリソースView
	hr = CreateShaderResourceView(device, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), &ShaderResourceView);
	if (FAILED(hr)) {
		return false;
	}
	return true;
}




void Texture::Set(UINT Slot, BOOL flg)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	if (flg == FALSE) {

		ID3D11ShaderResourceView*rtv[1] = { NULL };
		ID3D11SamplerState* ss[1] = { NULL };
		device_context->PSSetShaderResources(Slot, 1, rtv);
		device_context->PSSetSamplers(Slot, 1, ss);
		device_context->DSSetShaderResources(Slot, 1, rtv);
		device_context->DSSetSamplers(Slot, 1, ss);
		return;
	}
	if (ShaderResourceView) {
		device_context->PSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		device_context->DSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
	}
}

bool Texture::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = pSystem->GetDevice();

	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}


bool Texture::CreateMipMap(u_int width, u_int height, DXGI_FORMAT format,float MipLevel)
{
	ID3D11Device* device = pSystem->GetDevice();

	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = static_cast<UINT>(MipLevel);
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));


	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = static_cast<UINT>(MipLevel);
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateDepth(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = pSystem->GetDevice();

	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(Texture2D.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateCube(u_int width, u_int height, DXGI_FORMAT format, int miplevel)
{
	ID3D11Device* device = pSystem->GetDevice();

	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = miplevel;//★
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = format; 
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
//	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS| D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
	rtvd.Texture2DArray.FirstArraySlice = 0;
	rtvd.Texture2DArray.ArraySize = 6;
	rtvd.Texture2DArray.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MostDetailedMip = 0;
	srvd.TextureCube.MipLevels = miplevel;
	//srvd.TextureCube.MipLevels = 0;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;

}
bool Texture::CreateCubeDepthStencil(u_int width, u_int height)
{
	ID3D11Device* device = pSystem->GetDevice();

	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = DXGI_FORMAT_D32_FLOAT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
//	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
	dsvd.Texture2DArray.FirstArraySlice = 0;
	dsvd.Texture2DArray.ArraySize = 6;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateDepthStencilView(Texture2D.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;


}





