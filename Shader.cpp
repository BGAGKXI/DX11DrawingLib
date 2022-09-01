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
//	�V�F�[�_�[�P�̃R���p�C��
//------------------------------------------------
HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ComPtr<ID3DBlob> BlobError = NULL;
	// �R���p�C��
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

	// �G���[�o��
	if (BlobError != NULL)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
	}

	return hr;
}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc )
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));


	// ���_�V�F�[�_����
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr =device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		VertexLayout.GetAddressOf()
	);
	assert(SUCCEEDED(hr));


	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(VertexLayout.Get());


	// �s�N�Z���V�F�[�_
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = device->CreatePixelShader( PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	return true;
}


//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��2
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		VertexLayout.GetAddressOf()
	);

	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(VertexLayout.Get());

	// �s�N�Z���V�F�[�_
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �s�N�Z���V�F�[�_����
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
	//PSBlob.Get()->Release();
	assert(SUCCEEDED(hr));
	// �W�I���g���V�F�[�_
	ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �W�I���g���V�F�[�_����
	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��3
//------------------------------------------------
//bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
//{
//	ID3D11Device* device = pSystem->GetDevice();
//	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
//
//	HRESULT hr = S_OK;
//
//	ComPtr<ID3DBlob> VSBlob = NULL;
//	// ���_�V�F�[�_
//	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ���_�V�F�[�_����
//	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// ���̓��C�A�E�g
//	D3D11_INPUT_ELEMENT_DESC layout[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	UINT numElements = ARRAYSIZE(layout);
//
//	// ���̓��C�A�E�g����
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
//	// ���̓��C�A�E�g�ݒ�
//	device_context->IASetInputLayout(VertexLayout.Get());
//
//	// �s�N�Z���V�F�[�_
//	ComPtr<ID3DBlob> PSBlob = NULL;
//	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// �s�N�Z���V�F�[�_����
//	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// �W�I���g���V�F�[�_
//	ComPtr<ID3DBlob> GSBlob = NULL;
//	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//	// �W�I���g���V�F�[�_����
//	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// �n���V�F�[�_
//	ComPtr<ID3DBlob> HSBlob = NULL;
//	hr = Compile(filename, HSFunc, "hs_5_0", HSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// �n���V�F�[�_����
//	hr = device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, HS.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// �h���C���V�F�[�_
//	ComPtr<ID3DBlob> DSBlob = NULL;
//	hr = Compile(filename, DSFunc, "ds_5_0", DSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	// �h���C���V�F�[�_����
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
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", &VSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ���_�V�F�[�_����
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &VS);
	if (FAILED(hr))
	{
		VSBlob->Release();
		return false;
	}

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
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

	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(VertexLayout.Get());

	// �s�N�Z���V�F�[�_
	ID3DBlob* PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &PS);
	PSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// �W�I���g���V�F�[�_
	ID3DBlob* GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", &GSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �W�I���g���V�F�[�_����
	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, &GS);
	GSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// �n���V�F�[�_
	ID3DBlob* HSBlob = NULL;
	hr = Compile(filename, HSFunc, "hs_5_0", &HSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �n���V�F�[�_����
	hr = device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, &HS);
	HSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// �h���C���V�F�[�_
	ID3DBlob* DSBlob = NULL;
	hr = Compile(filename, DSFunc, "ds_5_0", &DSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �h���C���V�F�[�_����
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
//	�L����
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
//	�L����
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
