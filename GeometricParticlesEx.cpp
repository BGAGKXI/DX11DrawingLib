

#define _CRT_RAND_S   // rand_s���g��

#include <Windows.h>


#include "DxSystem.h"
#include "GeometricParticlesEx.h"

#include "View.h"

#include<map>
#include <string>


bool GeometricParticlesEx::m_first = true;


// ���̓��C�A�E�g
D3D11_INPUT_ELEMENT_DESC layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "ACCELERATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


HRESULT GeometricParticlesEx::create_vs_from_cso(
	ID3D11Device* device,	const char* cso_name, ID3D11VertexShader** vertex_shader,	
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

HRESULT GeometricParticlesEx::create_gs_stream_from_cso(
	ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader, 
	D3D11_SO_DECLARATION_ENTRY* Decl, UINT Decl_size, UINT* pBufferStrides, UINT buffer_size)
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


	// �X�g���[���o�͂���W�I���g���E�V�F�[�_�𐶐�
	HRESULT hr = device->CreateGeometryShaderWithStreamOutput(
		cso_data.get(), // �o�C�g�E�R�[�h�ւ̃|�C���^
		cso_sz,    // �o�C�g�E�R�[�h�̒���
		Decl,                        // �o�͂���f�[�^��`
		Decl_size,
		pBufferStrides,
		buffer_size,
		D3D11_SO_NO_RASTERIZED_STREAM, // ���X�^���C�U�ɑ��M���Ȃ�(�u11.0�v�ȏ�)
		NULL,
		geometry_shader);      // �쐬���ꂽ�W�I���g���E�V�F�[�_���󂯎��ϐ�

	assert(SUCCEEDED(hr));

	return hr;
}



HRESULT GeometricParticlesEx::create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
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

HRESULT GeometricParticlesEx::create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
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





//------------------------------------------------
//	�V�F�[�_�[�P�̃R���p�C��
//------------------------------------------------
HRESULT GeometricParticlesEx::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
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
//	�V�F�[�_�쐬(ParticleVertex)
//------------------------------------------------

HRESULT GeometricParticlesEx::CreateShaderParticleVertex(void)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();


	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// **********************************************************
	// ���_�V�F�[�_
	//hr = Compile(L"Shaders/GeoParticles2.fx", "VS_VERTEX_Main", "vs_5_0", VSBlob.GetAddressOf());
	//assert(SUCCEEDED(hr));

	//// ���_�V�F�[�_����
	//hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(),
	//	NULL, VS[0].GetAddressOf());
	//assert(SUCCEEDED(hr));

	//// ���̓��C�A�E�g
	//D3D11_INPUT_ELEMENT_DESC layout[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "ACCELERATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};



	//// ���̓��C�A�E�g����
	//hr = device->CreateInputLayout(
	//	layout,
	//	numElements,
	//	VSBlob->GetBufferPointer(),
	//	VSBlob->GetBufferSize(),
	//	VertexLayout.GetAddressOf()
	//);
	//assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g�T�C�Y
	UINT numElements = ARRAYSIZE(layout);

	// ���_�V�F�[�_/���̓��C�A�E�g
	hr = create_vs_from_cso(device, "Shaders/GeoParticles_vs.cso", VS[0].GetAddressOf(), VertexLayout.GetAddressOf(), layout, numElements);

	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(VertexLayout.Get());


	// **********************************************************
	// �X�g���[���o�͂̏o�̓f�[�^��`�̐錾
	D3D11_SO_DECLARATION_ENTRY Decl[] =
	{
		{ 0, "POSITION", 0, 0, 3, 0 }, // �ux,y,z�v���X���b�g�u0�v�́uPOSITION�v�ɏo��
		{ 0, "NORMAL",   0, 0, 3, 0 }, // �ux,y,z�v���X���b�g�u0�v�́uNORMAL�v�ɏo��
		{ 0, "TEXCOORD", 0, 0, 3, 0 },  // �ux,y,z�v���X���b�g�u0�v�́uTEXCOORD�v�ɏo��
		{ 0, "ACCELERATION", 0, 0, 3, 0 },  // �ux,y,z�v���X���b�g�u0�v�́uACCELERATION�v�ɏo��
	};

	UINT BufferStrides[] = { sizeof(VERTEX) };

	// �W�I���g���V�F�[�_
	//ComPtr<ID3DBlob> GSBlob = NULL;
	//hr = Compile(L"Shaders/GeoParticles2.fx", "GS_VERTEX_Main", "gs_5_0", GSBlob.GetAddressOf());
	//assert(SUCCEEDED(hr));
	//hr = create_gs_from_cso(device, "Shaders/GeoParticles_gs.cso",GS[0].GetAddressOf());


	//// �X�g���[���o�͂���W�I���g���E�V�F�[�_�𐶐�
	//hr = device->CreateGeometryShaderWithStreamOutput(
	//	GS[0].Get(), // �o�C�g�E�R�[�h�ւ̃|�C���^
	//	GSBlob->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
	//	Decl,                        // �o�͂���f�[�^��`
	//	_countof(Decl),              // �o�͂���1�f�[�^�̗v�f��
	//	BufferStrides,               // �o�͂���1�f�[�^�̃T�C�Y
	//	_countof(BufferStrides),
	//	//		0,                             // �@�\���x�����u11.0�v���Ⴂ�ꍇ
	//	D3D11_SO_NO_RASTERIZED_STREAM, // ���X�^���C�U�ɑ��M���Ȃ�(�u11.0�v�ȏ�)
	//	NULL,
	//	GS[0].GetAddressOf());      // �쐬���ꂽ�W�I���g���E�V�F�[�_���󂯎��ϐ�

	//assert(SUCCEEDED(hr));
	hr = create_gs_stream_from_cso(device, "Shaders/GeoParticles_gs.cso", GS[0].GetAddressOf(),
		Decl, _countof(Decl), BufferStrides, _countof(BufferStrides));


	return hr;
}

/*-------------------------------------------
	�V�F�[�_�쐬(Particle)
--------------------------------------------*/
HRESULT GeometricParticlesEx::CreateShaderParticle(void)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	// �e�N�X�`���쐬
	particleTex = new Texture();
	particleTex->Load(L"Assets/particle01.png");


	HRESULT hr;


	// ���_�V�F�[�_����
	// ���̓��C�A�E�g�T�C�Y
	UINT numElements = ARRAYSIZE(layout);

	hr = create_vs_from_cso(device, "Shaders/GeoParticleDraw_vs.cso", VS[1].GetAddressOf(), VertexLayout.GetAddressOf(), layout, numElements);


	// �W�I���g���V�F�[�_����
	hr = create_gs_from_cso(device, "Shaders/GeoParticleDraw_gs.cso", GS[1].GetAddressOf());


	// �s�N�Z���E�V�F�[�_�̃R�[�h���R���p�C��
	hr = create_ps_from_cso(device, "Shaders/GeoParticleDraw_ps.cso", PS[1].GetAddressOf());



	return hr;
}


GeometricParticlesEx::GeometricParticlesEx(int num)
{
	ID3D11Device* device = pSystem->GetDevice();

	// ���_��
	numVertexes = num;
	// ���_�o�b�t�@�̏�����
	memset(&m_CBData, 0, sizeof(m_CBData));

	// ���_�o�b�t�@�̒�`
	D3D11_BUFFER_DESC vBufferDesc;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	vBufferDesc.ByteWidth = sizeof(VERTEX) * numVertexes; // g_countVBuffer = ���_��
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT; // ���_�o�b�t�@�ƃX�g���[���o�͂Ƃ��Ďg��
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	// ���_�o�b�t�@�̃T�u���\�[�X�̏����l(���_���W)
	struct VERTEX* posVertex = new VERTEX[numVertexes];
	for (int i = 0; i < numVertexes; ++i) {
		posVertex[i].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		unsigned int x, y, z;
		rand_s(&x); rand_s(&y), rand_s(&z);
		float fx = ((float)x / (float)UINT_MAX) - 0.5f;
		float fy = ((float)y / (float)UINT_MAX) - 0.5f;
		float fz = ((float)z / (float)UINT_MAX) - 0.5f;
		posVertex[i].Velocity = XMFLOAT3(fx * 1.2f, fy * 1.2f, fz * 1.2f);
		posVertex[i].VelocityFirst = XMFLOAT3(fx * 1.2f, fy * 1.2f, fz * 1.2f);
		posVertex[i].Acceleration = XMFLOAT3(0, -0.01f, 0);
	}

	// ���_�o�b�t�@�̃T�u���\�[�X�̒�`
	D3D11_SUBRESOURCE_DATA vSubData;
	vSubData.pSysMem = posVertex;  // �o�b�t�@�E�f�[�^�̏����l
	vSubData.SysMemPitch = 0;
	vSubData.SysMemSlicePitch = 0;

	// ���_�o�b�t�@�̍쐬
	device->CreateBuffer(&vBufferDesc, &vSubData, &VerBufferDrawFrom);
	delete[] posVertex;


	device->CreateBuffer(&vBufferDesc, NULL, &VerBufferStreamTo);

	////////////////////////////////////////////////////////////////////
	// �萔�o�b�t�@�̒�`
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;    // ���I(�_�C�i�~�b�N)�g�p�@
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // �萔�o�b�t�@
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU���珑������
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	// �萔�o�b�t�@�̍쐬
	cBufferDesc.ByteWidth = sizeof(ConstantBufferForPerFrame); // �o�b�t�@�E�T�C�Y
	device->CreateBuffer(&cBufferDesc, NULL, CBuffer.GetAddressOf());

	///////////////////////////////////////////////////////////////////
	CreateShaderParticleVertex();

	CreateShaderParticle();

}

/*--------------------------------------------
	�p�[�e�B�N�����W�̌v�Z����
--------------------------------------------*/
void GeometricParticlesEx::RenderParticleVertex(void)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();


	// �o�ߎ���
	m_CBData.Time = pSystem->elapsed_time;

	// IA�ɒ��_�o�b�t�@��ݒ�
	ID3D11Buffer* buffer = VerBufferDrawFrom;
	UINT strides[1] = { sizeof(VERTEX) };
	UINT offsets[1] = { 0 };
	device_context->IASetVertexBuffers(0, 1, &buffer, strides, offsets);
	// IA�ɓ��̓��C�A�E�g�E�I�u�W�F�N�g��ݒ�
	device_context->IASetInputLayout(VertexLayout.Get());
	// IA�Ƀv���~�e�B�u�̎�ނ�ݒ�
	device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// VS�ɒ��_�V�F�[�_��ݒ�
	device_context->VSSetShader(VS[0].Get(), NULL, 0);

	// GS�ɃW�I���g���E�V�F�[�_��ݒ�
	device_context->GSSetShader(GS[0].Get(), NULL, 0);

	// SO�ɏo�͐�o�b�t�@��ݒ�
	buffer = VerBufferStreamTo;
	device_context->SOSetTargets(1, &VerBufferStreamTo, offsets);

	//------------------------------------------------------------------------------------
	// PS�Ƀs�N�Z���E�V�F�[�_�̒�~
	device_context->PSSetShader(NULL, NULL, 0);
	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 0);
	//------------------------------------------------------------------------------------

	// ***************************************
	// �萔�o�b�t�@�̃}�b�v�擾
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	device_context->Map(
		CBuffer.Get(),              // �}�b�v���郊�\�[�X
		0,							// �T�u���\�[�X�̃C���f�b�N�X�ԍ�
		D3D11_MAP_WRITE_DISCARD,	// �������݃A�N�Z�X
		0,							//
		&MappedResource);			// �f�[�^�̏������ݐ�|�C���^
	// �f�[�^��������
	CopyMemory(MappedResource.pData, &m_CBData, sizeof(ConstantBufferForPerFrame));
	// �}�b�v����
	device_context->Unmap(CBuffer.Get(), 0);

	// ***************************************
	// �X�g���[���o�͂���o�͂���
	if (m_first)
		device_context->Draw(numVertexes, 0);
	else
		device_context->DrawAuto();

	// ***************************************
	// SO�ɏo�͐�o�b�t�@��ݒ�
	buffer = NULL;
	device_context->SOSetTargets(1, &buffer, offsets);


	// �V�����v�Z�������_�f�[�^��`��Ώۂɂ���
	ID3D11Buffer* pB = VerBufferDrawFrom;
	VerBufferDrawFrom = VerBufferStreamTo;
	VerBufferStreamTo = pB;


	m_first = false;
}

void GeometricParticlesEx::Update(void)
{

}


/*--------------------------------------------
	�p�[�e�B�N���̕`�揈��
--------------------------------------------*/
void GeometricParticlesEx::RenderParticle(void)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	// ***************************************
	// �p�[�e�B�N���̃e�N�X�`���ƃT���v����ݒ�
	particleTex->Set(0);
	m_CBData.ParticleSize.x = 0.04f;
	m_CBData.ParticleSize.y = 0.04f;

	// ***************************************
	// IA�ɒ��_�o�b�t�@��ݒ�
	UINT strides[1] = { sizeof(VERTEX) };
	UINT offsets[1] = { 0 };
	device_context->IASetVertexBuffers(0, 1, &VerBufferDrawFrom, strides, offsets);
	// IA�ɓ��̓��C�A�E�g�E�I�u�W�F�N�g��ݒ�
	device_context->IASetInputLayout(VertexLayout.Get());
	// IA�Ƀv���~�e�B�u�̎�ނ�ݒ�
	device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//�u�����h�X�e�[�g�ݒ�
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ADD), nullptr, 0xFFFFFFFF);


	// VS�ɒ��_�V�F�[�_��ݒ�
	device_context->VSSetShader(VS[1].Get(), NULL, 0);

	// GS�ɃW�I���g���E�V�F�[�_��ݒ�
	device_context->GSSetShader(GS[1].Get(), NULL, 0);

	// RS�Ƀr���[�|�[�g��ݒ�
	pSystem->SetViewPort(pSystem->GetScreenWidth(),pSystem->GetScreenHeight());

	// PS�Ƀs�N�Z���E�V�F�[�_��ݒ�
	device_context->PSSetShader(PS[1].Get(), NULL, 0);


	// OM�Ƀu�����h�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ADD), BlendFactor, 0xffffffff);


	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_WRITE_FALSE), 0);


	// ***************************************
	// �萔�o�b�t�@�̃}�b�v�擾
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	device_context->Map(
		CBuffer.Get(),            // �}�b�v���郊�\�[�X
		0,                       // �T�u���\�[�X�̃C���f�b�N�X�ԍ�
		D3D11_MAP_WRITE_DISCARD, // �������݃A�N�Z�X
		0,                       //
		&MappedResource);        // �f�[�^�̏������ݐ�|�C���^
	// �f�[�^��������
	CopyMemory(MappedResource.pData, &m_CBData, sizeof(ConstantBufferForPerFrame));
	// �}�b�v����
	device_context->Unmap(CBuffer.Get(), 0);

	// ***************************************
	// �`�悷��
	device_context->DrawAuto();
}

void GeometricParticlesEx::Render(const XMMATRIX* v, const XMMATRIX* p)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();


	// �r���[�E�v���W�F�N�V�����s����R���X�^���g�ɐݒ�
	XMStoreFloat4x4(&m_CBData.View, XMMatrixTranspose(*v));
	XMStoreFloat4x4(&m_CBData.Projection, XMMatrixTranspose(*p));


	// VS�ɒ萔�o�b�t�@��ݒ�
	device_context->VSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());
	// GS�ɒ萔�o�b�t�@��ݒ�
	device_context->GSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());
	// PS�ɒ萔�o�b�t�@��ݒ�
	device_context->PSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());

	// ***************************************
	// �p�[�e�B�N�����W�̌v�Z����
	RenderParticleVertex();
	// �p�[�e�B�N���̕`�揈��
	RenderParticle();


}
