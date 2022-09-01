#include "DxSystem.h"
#include "Shader.h"

Shader::~Shader()
{
}

//****************************************************************
//
//
//
//****************************************************************
//------------------------------------------------
//	シェーダー単体コンパイル
//------------------------------------------------
HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ComPtr<ID3DBlob> BlobError = NULL;
	// コンパイル
	HRESULT hr = D3DCompileFromFile(
		filename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		method,
		shaderModel,
		ShaderFlags,
		0,
		ppBlobOut,
		BlobError.GetAddressOf()
	);

	// エラー出力
	if (BlobError != NULL)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
	}

	return hr;
}

//------------------------------------------------
//	シェーダーセットコンパイル
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc )
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));


	// 頂点シェーダ生成
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// 入力レイアウト生成
	hr =device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		VertexLayout.GetAddressOf()
	);
	assert(SUCCEEDED(hr));


	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());


	// ピクセルシェーダ
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ピクセルシェーダ生成
	hr = device->CreatePixelShader( PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	return true;
}


//------------------------------------------------
//	シェーダーセットコンパイル2
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 頂点シェーダ生成
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// 入力レイアウト生成
	hr = device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		VertexLayout.GetAddressOf()
	);

	assert(SUCCEEDED(hr));

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());

	// ピクセルシェーダ
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ピクセルシェーダ生成
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
	//PSBlob.Get()->Release();
	assert(SUCCEEDED(hr));
	// ジオメトリシェーダ
	ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ジオメトリシェーダ生成
	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

//------------------------------------------------
//	シェーダーセットコンパイル3
//------------------------------------------------
//bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
//{
//	ID3D11Device* device = pSystem->GetDevice();
//	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
//
//	HRESULT hr = S_OK;
//
//	ComPtr<ID3DBlob> VSBlob = NULL;
//	// 頂点シェーダ
//	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// 頂点シェーダ生成
//	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// 入力レイアウト
//	D3D11_INPUT_ELEMENT_DESC layout[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	UINT numElements = ARRAYSIZE(layout);
//
//	// 入力レイアウト生成
//	hr = device->CreateInputLayout(
//		layout,
//		numElements,
//		VSBlob->GetBufferPointer(),
//		VSBlob->GetBufferSize(),
//		VertexLayout.GetAddressOf()
//	);
//
//	assert(SUCCEEDED(hr));
//	VSBlob.Get()->Release();
//
//	// 入力レイアウト設定
//	device_context->IASetInputLayout(VertexLayout.Get());
//
//	// ピクセルシェーダ
//	ComPtr<ID3DBlob> PSBlob = NULL;
//	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ピクセルシェーダ生成
//	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ジオメトリシェーダ
//	ComPtr<ID3DBlob> GSBlob = NULL;
//	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//	// ジオメトリシェーダ生成
//	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ハルシェーダ
//	ComPtr<ID3DBlob> HSBlob = NULL;
//	hr = Compile(filename, HSFunc, "hs_5_0", HSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ハルシェーダ生成
//	hr = device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, HS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ドメインシェーダ
//	ComPtr<ID3DBlob> DSBlob = NULL;
//	hr = Compile(filename, DSFunc, "ds_5_0", DSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ドメインシェーダ生成
//	hr = device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, DS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//
//
//	return true;
//}
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	HRESULT hr = S_OK;

	ID3DBlob* VSBlob = NULL;
	// 頂点シェーダ
	hr = Compile(filename, VSFunc, "vs_5_0", &VSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// 頂点シェーダ生成
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &VS);
	if (FAILED(hr))
	{
		VSBlob->Release();
		return false;
	}

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// 入力レイアウト生成
	hr = device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&VertexLayout
		);

	VSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());

	// ピクセルシェーダ
	ID3DBlob* PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ピクセルシェーダ生成
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &PS);
	PSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// ジオメトリシェーダ
	ID3DBlob* GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", &GSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ジオメトリシェーダ生成
	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, &GS);
	GSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// ハルシェーダ
	ID3DBlob* HSBlob = NULL;
	hr = Compile(filename, HSFunc, "hs_5_0", &HSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ハルシェーダ生成
	hr = device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, &HS);
	HSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// ドメインシェーダ
	ID3DBlob* DSBlob = NULL;
	hr = Compile(filename, DSFunc, "ds_5_0", &DSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ドメインシェーダ生成
	hr = device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, &DS);
	DSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//****************************************************************
//
//
//
//****************************************************************
//------------------------------------------------
//	有効化
//------------------------------------------------
void Shader::Activate()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	device_context->VSSetShader(VS.Get(), NULL, 0);
	device_context->HSSetShader(HS.Get(), NULL, 0);
	device_context->DSSetShader(DS.Get(), NULL, 0);
	device_context->GSSetShader(GS.Get(), NULL, 0);
	device_context->PSSetShader(PS.Get(), NULL, 0);

}
//------------------------------------------------
//	有効化
//------------------------------------------------
void Shader::Inactivate()
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	device_context->VSSetShader(NULL, NULL, 0);
	device_context->HSSetShader(NULL, NULL, 0);
	device_context->DSSetShader(NULL, NULL, 0);
	device_context->GSSetShader(NULL, NULL, 0);
	device_context->PSSetShader(NULL, NULL, 0);

}
