#include "DxSystem.h"
#include "Sprite.h"
//**********************************************************
//
//		Spriteクラス
//
//**********************************************************

Sprite::Sprite():texture(nullptr)
{
	ID3D11Device* device = pSystem->GetDevice();

	VERTEX v[] = {
		XMFLOAT3(-0.5f, 0.5f,0), XMFLOAT3(0,0,1), XMFLOAT2(0,0), XMFLOAT4(1,1,1,1), //左上
		XMFLOAT3(0.5f, 0.5f,0),  XMFLOAT3(0,0,1), XMFLOAT2(1,0), XMFLOAT4(1,1,1,1), //右上
		XMFLOAT3(-0.5f,-0.5f,0), XMFLOAT3(0,0,1), XMFLOAT2(0,1), XMFLOAT4(1,1,1,1), //左下
		XMFLOAT3(0.5f,-0.5f,0),  XMFLOAT3(0,0,1), XMFLOAT2(1,1), XMFLOAT4(1,1,1,1), //右下
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd)); 
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	device->CreateBuffer(&bd, &res, VertexBuffer.GetAddressOf());


	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState.GetAddressOf());

}

Sprite::Sprite(std::shared_ptr<Texture> texture) :texture(texture)
{
	ID3D11Device* device = pSystem->GetDevice();

	VERTEX v[] = {
		XMFLOAT3(-0.5f, 0.5f,0), XMFLOAT3(0,0,1), XMFLOAT2(0,0), XMFLOAT4(1,1,1,1), //左上
		XMFLOAT3(0.5f, 0.5f,0),  XMFLOAT3(0,0,1), XMFLOAT2(1,0), XMFLOAT4(1,1,1,1), //右上
		XMFLOAT3(-0.5f,-0.5f,0), XMFLOAT3(0,0,1), XMFLOAT2(0,1), XMFLOAT4(1,1,1,1), //左下
		XMFLOAT3(0.5f,-0.5f,0),  XMFLOAT3(0,0,1), XMFLOAT2(1,1), XMFLOAT4(1,1,1,1), //右下
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	device->CreateBuffer(&bd, &res, VertexBuffer.GetAddressOf());
	//テクスチャ
	

	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState.GetAddressOf());

}

Sprite::Sprite(const wchar_t* filename):texture(nullptr)
{	
	ID3D11Device* device = pSystem->GetDevice();

	VERTEX v[] = {
		XMFLOAT3(-0.5f, 0.5f,0), XMFLOAT3(0,0,1), XMFLOAT2(0,0), XMFLOAT4(1,1,1,1), //左上
		XMFLOAT3(0.5f, 0.5f,0),  XMFLOAT3(0,0,1), XMFLOAT2(1,0), XMFLOAT4(1,1,1,1), //右上
		XMFLOAT3(-0.5f,-0.5f,0), XMFLOAT3(0,0,1), XMFLOAT2(0,1), XMFLOAT4(1,1,1,1), //左下
		XMFLOAT3(0.5f,-0.5f,0),  XMFLOAT3(0,0,1), XMFLOAT2(1,1), XMFLOAT4(1,1,1,1), //右下
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd)); // 全０
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory( &res, sizeof(res));
	res.pSysMem = v;

	device->CreateBuffer(&bd, &res, VertexBuffer.GetAddressOf());

	//	テクスチャ読み込み
	if (filename) {
		texture = std::make_shared<Texture>();
		texture->Load(filename);
	}


	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState.GetAddressOf());

}


Sprite::~Sprite()
{

}


void Sprite::Render(Shader* shader,
	float dx, float dy, float dw, float dh,
	float sx, float sy, float sw, float sh,
	float angle,
	float alpha
)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	D3D11_VIEWPORT vp;
	UINT num = 1;
	device_context->RSGetViewports(&num, &vp);

	float screen_width = vp.Width;
	float screen_height = vp.Height;

	shader->Activate();
	//頂点データ設定
	VERTEX data[4];

	data[0].Pos.x = dx;
	data[0].Pos.y = dy;
	data[0].Pos.z = 0.0f;

	data[1].Pos.x = dx + dw;
	data[1].Pos.y = dy;
	data[1].Pos.z = 0.0f;

	data[2].Pos.x = dx;
	data[2].Pos.y = dy + dh;
	data[2].Pos.z = 0.0f;

	data[3].Pos.x = dx + dw;
	data[3].Pos.y = dy + dh;
	data[3].Pos.z = 0.0f;

	//回転の中心
	float workPosX = dx + dw * 0.5f;
	float workPosY = dy + dh * 0.5f;

	//回転処理
	for (int i = 0; i < 4; i++) {

		float workX = data[i].Pos.x - workPosX;
		float workY = data[i].Pos.y - workPosY;
		data[i].Pos.x = workX * cosf(angle) - workY * sinf(angle) + workPosX;
		data[i].Pos.y = workX * sinf(angle) + workY * cosf(angle) + workPosY;
		data[i].Pos.z = 0.0f;
	}

	// 正規化デバイス座標系
	for (int i = 0; i < 4; i++) {
		data[i].Pos.x = 2.0f * data[i].Pos.x / screen_width - 1.0f;
		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / screen_height;
		data[i].Pos.z = 0.0f;
	}


	//テクスチャ座標設定
	data[0].Tex.x = sx;
	data[0].Tex.y = sy;
	data[1].Tex.x = sx + sw;
	data[1].Tex.y = sy;
	data[2].Tex.x = sx;
	data[2].Tex.y = sy + sh;
	data[3].Tex.x = sx + sw;
	data[3].Tex.y = sy + sh;

	//UV座標
	if (texture)
	{
		for (int i = 0; i < 4; i++) {
			data[i].Tex.x = data[i].Tex.x / texture->GetWidth();
			data[i].Tex.y = data[i].Tex.y / texture->GetHeight();
		}
	}
	//頂点カラー
	data[0].Color = XMFLOAT4(1, 1, 1, alpha);
	data[1].Color = XMFLOAT4(1, 1, 1, alpha);
	data[2].Color = XMFLOAT4(1, 1, 1, alpha);
	data[3].Color = XMFLOAT4(1, 1, 1, alpha);
	//法線
	data[0].Normal = XMFLOAT3(0, 0, 1);
	data[1].Normal = XMFLOAT3(0, 0, 1);
	data[2].Normal = XMFLOAT3(0, 0, 1);
	data[3].Normal = XMFLOAT3(0, 0, 1);

	//頂点データ更新
	device_context->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	//	頂点バッファの指定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(
		0, 1, VertexBuffer.GetAddressOf(), // スロット, 数, バッファ
		&stride,		// １頂点のサイズ
		&offset			// 開始位置
	);
	device_context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);
	//テクスチャの設定
	if (texture) texture->Set(0);

	device_context->Draw(4, 0);
	//シェーダ無効か
	shader->Inactivate();


}

void Sprite::Render(Shader* shader,
	Texture* tex,
	float dx, float dy, float dw, float dh,
	float sx, float sy, float sw, float sh,
	float angle,
	float alpha
)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	D3D11_VIEWPORT vp;
	UINT num = 1;

	device_context->RSGetViewports(&num, &vp);


	float screen_width = pSystem->GetScreenWidth();
	float screen_height = pSystem->GetScreenHeight();





	shader->Activate();
	//頂点データ設定
	VERTEX data[4];

	data[0].Pos.x = dx;
	data[0].Pos.y = dy;
	data[0].Pos.z = 0.0f;

	data[1].Pos.x = dx + dw;
	data[1].Pos.y = dy;
	data[1].Pos.z = 0.0f;

	data[2].Pos.x = dx;
	data[2].Pos.y = dy + dh;
	data[2].Pos.z = 0.0f;

	data[3].Pos.x = dx + dw;
	data[3].Pos.y = dy + dh;
	data[3].Pos.z = 0.0f;

	//回転の中心
	float workPosX = dx + dw * 0.5f;
	float workPosY = dy + dh * 0.5f;

	//回転処理
	for (int i = 0; i < 4; i++) {

		float workX = data[i].Pos.x - workPosX;
		float workY = data[i].Pos.y - workPosY;
		data[i].Pos.x = workX * cosf(angle) - workY * sinf(angle) + workPosX;
		data[i].Pos.y = workX * sinf(angle) + workY * cosf(angle) + workPosY;
		data[i].Pos.z = 0.0f;
	}



	// 正規化デバイス座標系

	for (int i = 0; i < 4; i++) {
		data[i].Pos.x = 2.0f * data[i].Pos.x / screen_width - 1.0f;
		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / screen_height;
		data[i].Pos.z = 0.0f;
	}


	//テクスチャ座標設定
	data[0].Tex.x = sx;
	data[0].Tex.y = sy;
	data[1].Tex.x = sx + sw;
	data[1].Tex.y = sy;
	data[2].Tex.x = sx;
	data[2].Tex.y = sy + sh;
	data[3].Tex.x = sx + sw;
	data[3].Tex.y = sy + sh;

	//UV座標
	for (int i = 0; i < 4; i++) {
		data[i].Tex.x = data[i].Tex.x / tex->GetWidth();
		data[i].Tex.y = data[i].Tex.y / tex->GetHeight();
	}
	//頂点カラー
	data[0].Color = XMFLOAT4(1, 1, 1, alpha);
	data[1].Color = XMFLOAT4(1, 1, 1, alpha);
	data[2].Color = XMFLOAT4(1, 1, 1, alpha);
	data[3].Color = XMFLOAT4(1, 1, 1, alpha);
	//法線
	data[0].Normal = XMFLOAT3(0, 0, 1);
	data[1].Normal = XMFLOAT3(0, 0, 1);
	data[2].Normal = XMFLOAT3(0, 0, 1);
	data[3].Normal = XMFLOAT3(0, 0, 1);

	//頂点データ更新
	device_context->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	//	頂点バッファの指定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(
		0, 1, VertexBuffer.GetAddressOf(), // スロット, 数, バッファ
		&stride,		// １頂点のサイズ
		&offset			// 開始位置
	);
	device_context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);
	device_context->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//テクスチャの設定
	if (tex) tex->Set(0);

	device_context->Draw(4, 0);
	//シェーダ無効か
	shader->Inactivate();
}



