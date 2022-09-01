#include "DxSystem.h"
#include "PlainMesh.h"


//初期化
PlainMesh::PlainMesh(int row, int col) :texture(nullptr)
{
	ID3D11Device* device = pSystem->GetDevice();
	//頂点を定義
	VERTEX *vertices = new VERTEX[(row + 1)*(col + 1)];
	float w = 1.0f / col;
	float h = 1.0f / row;
	for (int y = 0; y < row + 1; y++) {
		for (int x = 0; x < col + 1; x++) {
			int index = (col + 1)*y + x;
			vertices[index].Pos.x = w*x - 0.5f;
			vertices[index].Pos.y = h*y - 0.5f;
			vertices[index].Pos.z = 0.0f;

			vertices[index].Tex.x = 1.0f - w*x;
			vertices[index].Tex.y = 1.0f - h*y;
			vertices[index].Normal = XMFLOAT3(0, 0, 1);
			vertices[index].Color = XMFLOAT4(1, 1, 1, 1);
		}
	}



	//上の頂点でバーテックスバッファ作成
	iNumVertices = (row + 1)*(col + 1);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*iNumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		device->CreateBuffer(&bd, &InitData,
			VertexBuffer.GetAddressOf());
	}


	UINT16* indices = new UINT16[row*col * 3 * 2];
	int n = 0;
	for (int y = 0; y < row; y++) {
		for (int x = 0; x < col; x++) {
			indices[n + 0] = (col + 1) * y + x;
			indices[n + 1] = (col + 1) * y + x + 1;
			indices[n + 2] = (col + 1) * (y + 1) + x;

			indices[n + 3] = (col + 1) * (y + 1) + x;
			indices[n + 4] = (col + 1) * y + x + 1;
			indices[n + 5] = (col + 1) * (y + 1) + (x + 1);
			n += 6;
		}
	}


	//上の頂点でインデックスバッファ作成
	iNumIndices = row*col * 3 * 2;
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(UINT16)*iNumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		device->CreateBuffer(&bd, &InitData,
			IndexBuffer.GetAddressOf());
	}



	////テクスチャロード
	//if (filename) {
	//	texture = std::make_shared<Texture>();
	//	texture->Load(filename);
	//}


	// 定数バッファ生成(行列)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPerMesh);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	WorldMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);


}

PlainMesh::PlainMesh(const wchar_t* filename,int row, int col):texture(nullptr)
{
	ID3D11Device* device = pSystem->GetDevice();
	//頂点を定義
	VERTEX *vertices = new VERTEX[(row + 1)*(col + 1)];
	float w = 1.0f / col;
	float h = 1.0f / row;
	for (int y = 0; y < row+1; y++) {
		for (int x = 0; x < col+1; x++) {
			int index = (col+1)*y + x;
			vertices[index].Pos.x = w*x - 0.5f;
			vertices[index].Pos.y = h*y - 0.5f;
			vertices[index].Pos.z = 0.0f;

			vertices[index].Tex.x = 1.0f - w*x;
			vertices[index].Tex.y = 1.0f - h*y;
			vertices[index].Normal = XMFLOAT3(0, 0, 1);
			vertices[index].Color = XMFLOAT4(1, 1, 1, 1);
		}
	}



	//上の頂点でバーテックスバッファ作成
	iNumVertices = (row + 1)*(col + 1);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*iNumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		device->CreateBuffer(&bd, &InitData,
			VertexBuffer.GetAddressOf());
	}


	UINT16* indices = new UINT16[row*col * 3 * 2];
	int n = 0;
	for (int y = 0; y < row; y++) {
		for (int x = 0; x < col; x++) {
			indices[n+0] = (col + 1) * y + x ;
			indices[n+1] = (col + 1) * y + x + 1;
			indices[n+2] = (col + 1) * (y + 1) + x;

			indices[n+3] = (col + 1) * (y + 1) + x;
			indices[n+4] = (col + 1) * y + x + 1;
			indices[n+5] = (col + 1) * (y + 1) + (x + 1);
			n += 6;
		}
	}


	//上の頂点でインデックスバッファ作成
	iNumIndices = row*col * 3 * 2;
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(UINT16)*iNumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		device->CreateBuffer(&bd, &InitData,
			IndexBuffer.GetAddressOf());
	}



	//テクスチャロード
	if (filename) {
		texture = std::make_unique<Texture>();
		texture->Load(filename);
	}


	// 定数バッファ生成(行列)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPerMesh);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	WorldMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);


}

PlainMesh::PlainMesh(std::shared_ptr<Texture> texture,int row, int col) :texture(nullptr)
{
	ID3D11Device* device = pSystem->GetDevice();
	//頂点を定義
	VERTEX *vertices = new VERTEX[(row + 1)*(col + 1)];
	float w = 1.0f / col;
	float h = 1.0f / row;
	for (int y = 0; y < row + 1; y++) {
		for (int x = 0; x < col + 1; x++) {
			int index = (col + 1)*y + x;
			vertices[index].Pos.x = w*x - 0.5f;
			vertices[index].Pos.y = h*y - 0.5f;
			vertices[index].Pos.z = 0.0f;

			vertices[index].Tex.x = 1.0f - w*x;
			vertices[index].Tex.y = 1.0f - h*y;
			vertices[index].Normal = XMFLOAT3(0, 0, 1);
			vertices[index].Color = XMFLOAT4(1, 1, 1, 1);
		}
	}



	//上の頂点でバーテックスバッファ作成
	iNumVertices = (row + 1)*(col + 1);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*iNumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		device->CreateBuffer(&bd, &InitData,
			VertexBuffer.GetAddressOf());
	}


	UINT16* indices = new UINT16[row*col * 3 * 2];
	int n = 0;
	for (int y = 0; y < row; y++) {
		for (int x = 0; x < col; x++) {
			indices[n + 0] = (col + 1) * y + x;
			indices[n + 1] = (col + 1) * y + x + 1;
			indices[n + 2] = (col + 1) * (y + 1) + x;

			indices[n + 3] = (col + 1) * (y + 1) + x;
			indices[n + 4] = (col + 1) * y + x + 1;
			indices[n + 5] = (col + 1) * (y + 1) + (x + 1);
			n += 6;
		}
	}


	//上の頂点でインデックスバッファ作成
	iNumIndices = row*col * 3 * 2;
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(UINT16)*iNumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		device->CreateBuffer(&bd, &InitData,
			IndexBuffer.GetAddressOf());
	}



	//テクスチャロード
	this->texture = texture;

	// 定数バッファ生成(行列)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPerMesh);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	WorldMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

}


PlainMesh::~PlainMesh()
{

}







void PlainMesh::Update()
{
	//ワールド変換行列作成
	//XMMATRIX mW = XMMatrixIdentity();
	//拡大
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転
	XMMATRIX a = XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//移動
	XMMATRIX p = XMMatrixTranslation(pos.x, pos.y, pos.z);
	// 行列の合成
	XMMATRIX mW = s*a*p;
	XMStoreFloat4x4(&WorldMatrix, mW);

}

void PlainMesh::Render(Shader* shader, const XMMATRIX& view, const XMMATRIX& projection, D3D_PRIMITIVE_TOPOLOGY topology)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	shader->Activate();

	ConstantBufferForPerMesh cb;
	cb.world = WorldMatrix;
	XMStoreFloat4x4(&cb.matWVP, XMLoadFloat4x4(&WorldMatrix)*(view)*(projection));
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->HSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->DSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());


	//変更　プリミティブ・トポロジーをセット
	device_context->IASetPrimitiveTopology(topology);

	//バーテックスバッファーをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
	device_context->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	//テクスチャ設定
	if (texture) texture->Set(0);

	//レンダリング(インデックス付き)
	device_context->DrawIndexed(iNumIndices,0,0);

	//シェーダー無効可
	shader->Inactivate();

}

void PlainMesh::Render(Shader* shader, Texture* texture, const XMMATRIX& view, const XMMATRIX& projection, D3D_PRIMITIVE_TOPOLOGY topology)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	//シェーダー有効化
	shader->Activate();

	ConstantBufferForPerMesh cb;
	cb.world = WorldMatrix;
	XMStoreFloat4x4(&cb.matWVP, XMLoadFloat4x4(&WorldMatrix)*(view)*(projection));
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->HSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->DSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());


	//変更　プリミティブ・トポロジーをセット
	device_context->IASetPrimitiveTopology(topology);

	//バーテックスバッファーをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
	device_context->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	//テクスチャ設定
	if (texture) texture->Set(0);

	//レンダリング(インデックス付き)
	device_context->DrawIndexed(iNumIndices, 0, 0);

	//シェーダー無効化
	shader->Inactivate();

	//シェーダー無効可
	shader->Inactivate();

}





