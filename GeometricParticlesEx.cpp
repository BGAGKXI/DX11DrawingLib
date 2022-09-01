

#define _CRT_RAND_S   // rand_sを使う

#include <Windows.h>


#include "DxSystem.h"
#include "GeometricParticlesEx.h"

#include "View.h"

#include<map>
#include <string>


bool GeometricParticlesEx::m_first = true;


// 入力レイアウト
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


	// ストリーム出力するジオメトリ・シェーダを生成
	HRESULT hr = device->CreateGeometryShaderWithStreamOutput(
		cso_data.get(), // バイト・コードへのポインタ
		cso_sz,    // バイト・コードの長さ
		Decl,                        // 出力するデータ定義
		Decl_size,
		pBufferStrides,
		buffer_size,
		D3D11_SO_NO_RASTERIZED_STREAM, // ラスタライザに送信しない(「11.0」以上)
		NULL,
		geometry_shader);      // 作成されたジオメトリ・シェーダを受け取る変数

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
//	シェーダー単体コンパイル
//------------------------------------------------
HRESULT GeometricParticlesEx::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
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
//	シェーダ作成(ParticleVertex)
//------------------------------------------------

HRESULT GeometricParticlesEx::CreateShaderParticleVertex(void)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();


	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// **********************************************************
	// 頂点シェーダ
	//hr = Compile(L"Shaders/GeoParticles2.fx", "VS_VERTEX_Main", "vs_5_0", VSBlob.GetAddressOf());
	//assert(SUCCEEDED(hr));

	//// 頂点シェーダ生成
	//hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(),
	//	NULL, VS[0].GetAddressOf());
	//assert(SUCCEEDED(hr));

	//// 入力レイアウト
	//D3D11_INPUT_ELEMENT_DESC layout[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "ACCELERATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};



	//// 入力レイアウト生成
	//hr = device->CreateInputLayout(
	//	layout,
	//	numElements,
	//	VSBlob->GetBufferPointer(),
	//	VSBlob->GetBufferSize(),
	//	VertexLayout.GetAddressOf()
	//);
	//assert(SUCCEEDED(hr));

	// 入力レイアウトサイズ
	UINT numElements = ARRAYSIZE(layout);

	// 頂点シェーダ/入力レイアウト
	hr = create_vs_from_cso(device, "Shaders/GeoParticles_vs.cso", VS[0].GetAddressOf(), VertexLayout.GetAddressOf(), layout, numElements);

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());


	// **********************************************************
	// ストリーム出力の出力データ定義の宣言
	D3D11_SO_DECLARATION_ENTRY Decl[] =
	{
		{ 0, "POSITION", 0, 0, 3, 0 }, // 「x,y,z」をスロット「0」の「POSITION」に出力
		{ 0, "NORMAL",   0, 0, 3, 0 }, // 「x,y,z」をスロット「0」の「NORMAL」に出力
		{ 0, "TEXCOORD", 0, 0, 3, 0 },  // 「x,y,z」をスロット「0」の「TEXCOORD」に出力
		{ 0, "ACCELERATION", 0, 0, 3, 0 },  // 「x,y,z」をスロット「0」の「ACCELERATION」に出力
	};

	UINT BufferStrides[] = { sizeof(VERTEX) };

	// ジオメトリシェーダ
	//ComPtr<ID3DBlob> GSBlob = NULL;
	//hr = Compile(L"Shaders/GeoParticles2.fx", "GS_VERTEX_Main", "gs_5_0", GSBlob.GetAddressOf());
	//assert(SUCCEEDED(hr));
	//hr = create_gs_from_cso(device, "Shaders/GeoParticles_gs.cso",GS[0].GetAddressOf());


	//// ストリーム出力するジオメトリ・シェーダを生成
	//hr = device->CreateGeometryShaderWithStreamOutput(
	//	GS[0].Get(), // バイト・コードへのポインタ
	//	GSBlob->GetBufferSize(),    // バイト・コードの長さ
	//	Decl,                        // 出力するデータ定義
	//	_countof(Decl),              // 出力する1データの要素数
	//	BufferStrides,               // 出力する1データのサイズ
	//	_countof(BufferStrides),
	//	//		0,                             // 機能レベルが「11.0」より低い場合
	//	D3D11_SO_NO_RASTERIZED_STREAM, // ラスタライザに送信しない(「11.0」以上)
	//	NULL,
	//	GS[0].GetAddressOf());      // 作成されたジオメトリ・シェーダを受け取る変数

	//assert(SUCCEEDED(hr));
	hr = create_gs_stream_from_cso(device, "Shaders/GeoParticles_gs.cso", GS[0].GetAddressOf(),
		Decl, _countof(Decl), BufferStrides, _countof(BufferStrides));


	return hr;
}

/*-------------------------------------------
	シェーダ作成(Particle)
--------------------------------------------*/
HRESULT GeometricParticlesEx::CreateShaderParticle(void)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	// テクスチャ作成
	particleTex = new Texture();
	particleTex->Load(L"Assets/particle01.png");


	HRESULT hr;


	// 頂点シェーダ生成
	// 入力レイアウトサイズ
	UINT numElements = ARRAYSIZE(layout);

	hr = create_vs_from_cso(device, "Shaders/GeoParticleDraw_vs.cso", VS[1].GetAddressOf(), VertexLayout.GetAddressOf(), layout, numElements);


	// ジオメトリシェーダ生成
	hr = create_gs_from_cso(device, "Shaders/GeoParticleDraw_gs.cso", GS[1].GetAddressOf());


	// ピクセル・シェーダのコードをコンパイル
	hr = create_ps_from_cso(device, "Shaders/GeoParticleDraw_ps.cso", PS[1].GetAddressOf());



	return hr;
}


GeometricParticlesEx::GeometricParticlesEx(int num)
{
	ID3D11Device* device = pSystem->GetDevice();

	// 頂点数
	numVertexes = num;
	// 頂点バッファの初期化
	memset(&m_CBData, 0, sizeof(m_CBData));

	// 頂点バッファの定義
	D3D11_BUFFER_DESC vBufferDesc;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	vBufferDesc.ByteWidth = sizeof(VERTEX) * numVertexes; // g_countVBuffer = 頂点数
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT; // 頂点バッファとストリーム出力として使う
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	// 頂点バッファのサブリソースの初期値(頂点座標)
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

	// 頂点バッファのサブリソースの定義
	D3D11_SUBRESOURCE_DATA vSubData;
	vSubData.pSysMem = posVertex;  // バッファ・データの初期値
	vSubData.SysMemPitch = 0;
	vSubData.SysMemSlicePitch = 0;

	// 頂点バッファの作成
	device->CreateBuffer(&vBufferDesc, &vSubData, &VerBufferDrawFrom);
	delete[] posVertex;


	device->CreateBuffer(&vBufferDesc, NULL, &VerBufferStreamTo);

	////////////////////////////////////////////////////////////////////
	// 定数バッファの定義
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;    // 動的(ダイナミック)使用法
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 定数バッファ
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPUから書き込む
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	// 定数バッファの作成
	cBufferDesc.ByteWidth = sizeof(ConstantBufferForPerFrame); // バッファ・サイズ
	device->CreateBuffer(&cBufferDesc, NULL, CBuffer.GetAddressOf());

	///////////////////////////////////////////////////////////////////
	CreateShaderParticleVertex();

	CreateShaderParticle();

}

/*--------------------------------------------
	パーティクル座標の計算処理
--------------------------------------------*/
void GeometricParticlesEx::RenderParticleVertex(void)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();


	// 経過時間
	m_CBData.Time = pSystem->elapsed_time;

	// IAに頂点バッファを設定
	ID3D11Buffer* buffer = VerBufferDrawFrom;
	UINT strides[1] = { sizeof(VERTEX) };
	UINT offsets[1] = { 0 };
	device_context->IASetVertexBuffers(0, 1, &buffer, strides, offsets);
	// IAに入力レイアウト・オブジェクトを設定
	device_context->IASetInputLayout(VertexLayout.Get());
	// IAにプリミティブの種類を設定
	device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// VSに頂点シェーダを設定
	device_context->VSSetShader(VS[0].Get(), NULL, 0);

	// GSにジオメトリ・シェーダを設定
	device_context->GSSetShader(GS[0].Get(), NULL, 0);

	// SOに出力先バッファを設定
	buffer = VerBufferStreamTo;
	device_context->SOSetTargets(1, &VerBufferStreamTo, offsets);

	//------------------------------------------------------------------------------------
	// PSにピクセル・シェーダの停止
	device_context->PSSetShader(NULL, NULL, 0);
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 0);
	//------------------------------------------------------------------------------------

	// ***************************************
	// 定数バッファのマップ取得
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	device_context->Map(
		CBuffer.Get(),              // マップするリソース
		0,							// サブリソースのインデックス番号
		D3D11_MAP_WRITE_DISCARD,	// 書き込みアクセス
		0,							//
		&MappedResource);			// データの書き込み先ポインタ
	// データ書き込み
	CopyMemory(MappedResource.pData, &m_CBData, sizeof(ConstantBufferForPerFrame));
	// マップ解除
	device_context->Unmap(CBuffer.Get(), 0);

	// ***************************************
	// ストリーム出力から出力する
	if (m_first)
		device_context->Draw(numVertexes, 0);
	else
		device_context->DrawAuto();

	// ***************************************
	// SOに出力先バッファを設定
	buffer = NULL;
	device_context->SOSetTargets(1, &buffer, offsets);


	// 新しく計算した頂点データを描画対象にする
	ID3D11Buffer* pB = VerBufferDrawFrom;
	VerBufferDrawFrom = VerBufferStreamTo;
	VerBufferStreamTo = pB;


	m_first = false;
}

void GeometricParticlesEx::Update(void)
{

}


/*--------------------------------------------
	パーティクルの描画処理
--------------------------------------------*/
void GeometricParticlesEx::RenderParticle(void)
{
	ID3D11Device* device = pSystem->GetDevice();
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();

	// ***************************************
	// パーティクルのテクスチャとサンプラを設定
	particleTex->Set(0);
	m_CBData.ParticleSize.x = 0.04f;
	m_CBData.ParticleSize.y = 0.04f;

	// ***************************************
	// IAに頂点バッファを設定
	UINT strides[1] = { sizeof(VERTEX) };
	UINT offsets[1] = { 0 };
	device_context->IASetVertexBuffers(0, 1, &VerBufferDrawFrom, strides, offsets);
	// IAに入力レイアウト・オブジェクトを設定
	device_context->IASetInputLayout(VertexLayout.Get());
	// IAにプリミティブの種類を設定
	device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//ブレンドステート設定
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ADD), nullptr, 0xFFFFFFFF);


	// VSに頂点シェーダを設定
	device_context->VSSetShader(VS[1].Get(), NULL, 0);

	// GSにジオメトリ・シェーダを設定
	device_context->GSSetShader(GS[1].Get(), NULL, 0);

	// RSにビューポートを設定
	pSystem->SetViewPort(pSystem->GetScreenWidth(),pSystem->GetScreenHeight());

	// PSにピクセル・シェーダを設定
	device_context->PSSetShader(PS[1].Get(), NULL, 0);


	// OMにブレンド・ステート・オブジェクトを設定
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	device_context->OMSetBlendState(pSystem->GetBlendState(pSystem->BS_ADD), BlendFactor, 0xffffffff);


	// OMに深度/ステンシル・ステート・オブジェクトを設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_WRITE_FALSE), 0);


	// ***************************************
	// 定数バッファのマップ取得
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	device_context->Map(
		CBuffer.Get(),            // マップするリソース
		0,                       // サブリソースのインデックス番号
		D3D11_MAP_WRITE_DISCARD, // 書き込みアクセス
		0,                       //
		&MappedResource);        // データの書き込み先ポインタ
	// データ書き込み
	CopyMemory(MappedResource.pData, &m_CBData, sizeof(ConstantBufferForPerFrame));
	// マップ解除
	device_context->Unmap(CBuffer.Get(), 0);

	// ***************************************
	// 描画する
	device_context->DrawAuto();
}

void GeometricParticlesEx::Render(const XMMATRIX* v, const XMMATRIX* p)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();


	// ビュー・プロジェクション行列をコンスタントに設定
	XMStoreFloat4x4(&m_CBData.View, XMMatrixTranspose(*v));
	XMStoreFloat4x4(&m_CBData.Projection, XMMatrixTranspose(*p));


	// VSに定数バッファを設定
	device_context->VSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());
	// GSに定数バッファを設定
	device_context->GSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());
	// PSに定数バッファを設定
	device_context->PSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());

	// ***************************************
	// パーティクル座標の計算処理
	RenderParticleVertex();
	// パーティクルの描画処理
	RenderParticle();


}
