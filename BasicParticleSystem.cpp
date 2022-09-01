#include "DxSystem.h"
#include "BasicParticleSystem.h"

cBasicParticleSystem::cBasicParticleSystem(int num)
{
	ID3D11Device* device = pSystem->GetDevice();

	vertices.resize(num);
	//パーティクル作成と画像ロード
	texture = std::make_unique<Texture>();
	texture->Load(L"Assets/particle1.png");

	//頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//頂点数分のバッファ
	bd.ByteWidth = sizeof(VERTEX)*num;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 定数バッファ生成
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}

cBasicParticleSystem::cBasicParticleSystem(std::shared_ptr<Texture> texture, int num)
{
	//頂点データ作成
	vertices.resize(num);
	//パーティクル作成と画像設定
	this->texture = texture;

	ID3D11Device* device = pSystem->GetDevice();
	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//頂点数分のバッファ
	bd.ByteWidth = sizeof(VERTEX)*num;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 定数バッファ生成
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}


void cBasicParticleSystem::Render(
	Shader* shader, const XMMATRIX& view, const XMMATRIX& projection,
	int blend)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	//シェーダ有効化
	shader->Activate();

	//定数バッファの更新
	ConstantBufferForPerFrame cb;
	XMFLOAT4X4 fv, fp;
	XMStoreFloat4x4(&fv, view);
	cb.view = fv;
	XMStoreFloat4x4(&fp, projection);
	cb.projection = fp;
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());

	//ブレンドステート設定
	device_context->OMSetBlendState(pSystem->GetBlendState(blend), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	device_context->RSSetState(pSystem->GetRasterizerState(pSystem->RS_CULL_BACK));
	//デプスステンシルステート設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_WRITE_FALSE), 1);
	//プリミティブ・トポロジーをセット
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//テクスチャ設定
	if (texture) texture->Set(0);


	//バーテックスバッファーをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
	//レンダリング(インデックス付き)
	device_context->Draw(vertices.size(), 0);
	//デプスステンシルステート設定
	device_context->OMSetDepthStencilState(pSystem->GetDepthStencilState(pSystem->DS_FALSE), 1);
	//シェーダ無効化
	shader->Inactivate();
}

void cBasicParticleSystem::Set( DirectX::XMFLOAT3 p )
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	for (auto& v :vertices) 
	{
		v.Pos.x = (rand() % 1001 - 500) * 0.01f;
		v.Pos.y = (rand() % 1001 - 500) * 0.01f;
		v.Pos.z = (rand() % 1001 - 500) * 0.01f;
		v.Color = {1,1,1,1};
	}
	//頂点データ更新
	device_context->UpdateSubresource(VertexBuffer.Get(), 0, nullptr, vertices.data(), 0, 0);
}


