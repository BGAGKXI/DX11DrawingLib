#pragma once
#include <d3d11.h>
#include <memory>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <assert.h>
#define SAFE_RELEASE(x) if((x)){(x)->Release();(x)=NULL;}
#define SAFE_DELETE(x) if((x)){delete (x);(x)=NULL;}
#define SAFE_DELETE_ARRAY(x) if(x){delete[] x; x=0;}

#define SAMPLE_COUNT 1
#define SAMPLE_QUALITY 0



class DxSystem {
private:
	
	ComPtr<ID3D11Device>			Device;
	ComPtr<ID3D11DeviceContext>	DeviceContext;
	int ScreenWidth{};
	int ScreenHeight{};
	ComPtr<IDXGISwapChain>			SwapChain;
	ComPtr<ID3D11RenderTargetView>	RenderTargetView;
	ComPtr<ID3D11DepthStencilView>	DepthStencilView;
	ComPtr<ID3D11ShaderResourceView>	ShaderResourceView;


	HRESULT CreateDevice(HWND hWnd,int width,int height);
	bool InitializeRenderTarget();

	static const int DEPTH_STENCIL_TYPE = 3;
	ComPtr<ID3D11DepthStencilState>	DepthStencilState[DEPTH_STENCIL_TYPE];
	bool CreateDepthStencilState();

	static const int RASTERIZE_TYPE = 4;
	ComPtr<ID3D11RasterizerState> RasterizerState[RASTERIZE_TYPE];
	bool CreateRasterizerState();

	static const int BLEND_TYPE = 9;
	ComPtr<ID3D11BlendState>		BlendState[BLEND_TYPE];
	bool CreateBlendState();

	DxSystem() {
		elapsed_time = 0; 
	}
public:
	static DxSystem* getInstance()
	{
		static DxSystem instance;
		return&instance;
	}


	float elapsed_time;

	bool Initialize(HWND hWnd,int width, int height);
	void Clear(DWORD color = 0x00000000);
	void Clear(float color[4]);

	void Flip(int n = 0);
	bool CreateDepthStencil(u_int widht, u_int height);


	ID3D11Device*		GetDevice() { return Device.Get(); }
	ID3D11DeviceContext*	GetDeviceContext() { return DeviceContext.Get(); }

	float GetScreenWidth() { return static_cast<float>(ScreenWidth); }
	float GetScreenHeight() { return static_cast<float>(ScreenHeight); }
	ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
	ID3D11ShaderResourceView*	GetDepthStencilSRV() { return	ShaderResourceView.Get(); }
	ID3D11RenderTargetView*	GetRenderTargetView() { return RenderTargetView.Get(); }
	ID3D11DepthStencilState* GetDepthStencilState(int state) {
		return DepthStencilState[state].Get(); }
	ID3D11RasterizerState* GetRasterizerState(int state) { return RasterizerState[state].Get(); }
	ID3D11BlendState* GetBlendState(int state) { return BlendState[state].Get(); }


	void SetViewPort(float width, float height);
	void SetViewPort(float x, float y, float width, float height);
	//DepthStencilState
	enum{ DS_FALSE, DS_TRUE, DS_WRITE_FALSE};
	//RasterizerState
	enum{ RS_CULL_BACK, RS_WIRE, RS_CULL_FRONT,RS_CULL_NONE};
	//BlendState
	enum {BS_NONE,BS_ALPHA,BS_ADD,BS_SUBTRACT,BS_REPLACE,BS_MULTIPLY,BS_LIGHTEN,BS_DARKEN,BS_SCREEN};


};

#define pSystem DxSystem::getInstance()