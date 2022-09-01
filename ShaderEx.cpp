#define _CRT_SECURE_NO_WARNINGS
#include "DxSystem.h"

#include "ShaderEx.h"

#include<memory>

#include<map>
#include <wrl.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <stdlib.h>
#include <string>

D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },	
	{ "PARAM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

};


HRESULT ShaderEx::create_vs_from_cso(
	ID3D11Device* device,
	const char* cso_name,
	ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout,
	D3D11_INPUT_ELEMENT_DESC* input_element_desc,
	UINT num_elements)
{


	FILE* fp = nullptr;
	errno_t error;
	error = fopen_s(&fp, cso_name, "rb");
	if (error != 0)assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);
	
	HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	assert(SUCCEEDED(hr));

	hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
	assert(SUCCEEDED(hr));


	return hr;
}

HRESULT ShaderEx::create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader)
{

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateHullShader(cso_data.get(), cso_sz, nullptr, hull_shader);
	assert(SUCCEEDED(hr));


	return hr;
}

HRESULT ShaderEx::create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader)
{

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateDomainShader(cso_data.get(), cso_sz, nullptr, domain_shader);
	assert(SUCCEEDED(hr));


	return hr;
}



HRESULT ShaderEx::create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	assert(SUCCEEDED(hr));


	return hr;
}

HRESULT ShaderEx::create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	if (error != 0) assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	assert(SUCCEEDED(hr));


	return hr;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);
	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());


	return false;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* gsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);
	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, gsfilename, wcslen(gsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_gs_from_cso(device, fullname, GS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());


	return false;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* hsfilename, WCHAR* dsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);

	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, hsfilename, wcslen(hsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_hs_from_cso(device, fullname, HS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, dsfilename, wcslen(dsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ds_from_cso(device, fullname, DS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());

	return false;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* hsfilename, WCHAR* dsfilename, WCHAR* gsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);

	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, "sprite_vs.cso", VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, hsfilename, wcslen(hsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_hs_from_cso(device, fullname, HS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, dsfilename, wcslen(dsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ds_from_cso(device, fullname, DS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, gsfilename, wcslen(gsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_gs_from_cso(device, fullname, GS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());

	return false;
}
