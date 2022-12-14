#include "DxSystem.h"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

//#define SAMPLE_COUNT 1
//#define SAMPLE_QUALITY 0


//****************************************************************
//
//	初期化
//
//****************************************************************
bool DxSystem::Initialize(HWND hWnd, int width, int height)
{
	ScreenWidth = width;
	ScreenHeight = height;
	CreateDevice(hWnd,width,height);
	InitializeRenderTarget();
	CreateRasterizerState();
	CreateBlendState();
	return false;
}

//****************************************************************
//
//	デバイス生成
//
//****************************************************************
HRESULT DxSystem::CreateDevice( HWND hWnd ,int width, int height)
{
	HRESULT hr = S_OK;

	//デバイスの生成
	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,	//	D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		Device.GetAddressOf(),
		NULL,
		DeviceContext.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}


	//インターフェース取得
	IDXGIDevice1* hpDXGI = NULL;
	if (FAILED(Device.Get()->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI))) {
		MessageBoxW(hWnd, L"QueryInterface", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//アダプター取得
	IDXGIAdapter* hpAdapter = NULL;
	if (FAILED(hpDXGI->GetAdapter(&hpAdapter))) {
		MessageBoxW(hWnd, L"GetAdapter", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//ファクトリー取得
	IDXGIFactory* hpDXGIFactory = NULL;
	hpAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
	if (hpDXGIFactory == NULL) {
		MessageBoxW(hWnd, L"GetParent", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//スワップチェイン作成
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = (int)ScreenWidth;
	scd.BufferDesc.Height = (int)ScreenHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//scd.SampleDesc = MSAA;
	scd.SampleDesc.Quality = SAMPLE_QUALITY;
	scd.SampleDesc.Count = SAMPLE_COUNT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(hpDXGIFactory->CreateSwapChain(Device.Get(), &scd, SwapChain.GetAddressOf()))) {
		MessageBoxW(hWnd, L"CreateSwapChain", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}


	return S_OK;
}


//****************************************************************
//
//	レンダーターゲット関連
//
//****************************************************************
//------------------------------------------------
//	初期化
//------------------------------------------------
bool DxSystem::InitializeRenderTarget()
{
	// バックバッファ取得
	ID3D11Texture2D* BackBuffer = NULL;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);

	if (FAILED(hr))
	{
		assert("バックバッファ取得失敗");
		return false;
	}

	// レンダーターゲットビュー生成
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, RenderTargetView.GetAddressOf());
	BackBuffer->Release();
	BackBuffer = NULL;

	if (FAILED(hr))
	{
		assert("レンダーターゲットビュー生成失敗");
		return false;
	}
	//  深度ステンシルバッファ生成
	CreateDepthStencil((u_int)ScreenWidth, (u_int)ScreenHeight);
	//  デプスステンシルステート生成
	CreateDepthStencilState();

	// レンダーターゲットビュー設定
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	// ビューポートの設定
	SetViewPort(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));


	return true;
}

//------------------------------------------------
//	ビューポートの設定
//------------------------------------------------
void DxSystem::SetViewPort(float width, float height)
{
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);



}
void DxSystem::SetViewPort(float x, float y, float width, float height)
{
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = (float)x;
	vp.TopLeftY = (float)y;
	DeviceContext->RSSetViewports(1, &vp);

}


//------------------------------------------------
//	深度ステンシルバッファ生成
//------------------------------------------------
bool DxSystem::CreateDepthStencil(u_int widht, u_int height)
{
	ComPtr<ID3D11Texture2D> DepthStencilTexture;

	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = widht;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R24G8_TYPELESS;
	td.SampleDesc.Count = SAMPLE_COUNT;
	td.SampleDesc.Quality = SAMPLE_QUALITY;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = Device->CreateTexture2D(&td, NULL, &DepthStencilTexture);

	if (FAILED(hr))
	{
		assert("深度ステンシルテクスチャ生成失敗");
		return false;
	}

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = Device->CreateDepthStencilView(DepthStencilTexture.Get(), &dsvd, DepthStencilView.GetAddressOf());

	if (FAILED(hr))
	{
		assert("深度ステンシルビューの失敗");
		return false;
	}

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	//srvd.Format = DXGI_FORMAT_D32_FLOAT;
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = Device->CreateShaderResourceView(DepthStencilTexture.Get(), &srvd, ShaderResourceView.GetAddressOf());

	if (FAILED(hr))
	{
		assert("シェーダリソースビュー生成失敗");
		return false;
	}


	return true;
}

//------------------------------------------------
//	デプスステンシルステートの生成
//------------------------------------------------

bool DxSystem::CreateDepthStencilState()
{
	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	HRESULT hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[DS_FALSE].GetAddressOf());

	if (FAILED(hr))
	{
		assert("デプスステンシルステート失敗");
		return false;
	}
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[DS_TRUE].GetAddressOf());


	if (FAILED(hr))
	{
		assert("デプスステンシルステート失敗");
		return false;
	}

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[DS_WRITE_FALSE].GetAddressOf());

	if (FAILED(hr))
	{
		assert("デプスステンシルステート失敗");
		return false;
	}

	return true;

}




//------------------------------------------------
//	ラスタライザステートの生成
//------------------------------------------------

bool DxSystem::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	for (int state = 0; state < RASTERIZE_TYPE; state++) {
		switch (state) {
		case RS_CULL_BACK:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;

		case RS_WIRE:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;
			break;

		case RS_CULL_FRONT:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;

		case RS_CULL_NONE:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;
		}
		HRESULT hr = Device->CreateRasterizerState(&rd, RasterizerState[state].GetAddressOf());

		if (FAILED(hr))
		{
			assert("ラスタライザステートの生成");
			return false;
		}
	}

	return true;

}

//ブレンドステートの作成
bool DxSystem::CreateBlendState()
{
	D3D11_BLEND_DESC bd;

	for (int state = 0; state < BLEND_TYPE; state++) {
		switch (state) {
		case BS_NONE:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = false;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_ALPHA:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_ADD:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_SUBTRACT:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			//bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_REPLACE:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS_MULTIPLY:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS_LIGHTEN:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_DARKEN:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS_SCREEN:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		}
		//ブレンドステートの作成
		HRESULT hr = Device->CreateBlendState(&bd, BlendState[state].GetAddressOf());
		//assert(FAILED(hr));

		if (FAILED(hr))
		{
			assert("ブレンドステートの作成失敗");
			return false;
		}
	}
	return true;
}

//------------------------------------------------
//	クリア
//------------------------------------------------
void DxSystem::Clear(DWORD color)
{

	// デフォルトレンダーターゲットビュー設定
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	float clearColor[4];
	for (int i = 3; i>=0; i--){
		clearColor[i] = ((color >> 8*(3-i)) & 0x000000FF)/255.0f;
	}
 	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetDepthStencilState(DepthStencilState[DS_TRUE].Get(), 1);

}

void DxSystem::Clear(float color[4])
{

	// デフォルトレンダーターゲットビュー設定
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), color);
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetDepthStencilState(DepthStencilState[DS_TRUE].Get(), 1);

}

//------------------------------------------------
//	フリップ
//------------------------------------------------
void DxSystem::Flip(int n)
{
	// フリップ処理
	SwapChain->Present(n, 0);


}
