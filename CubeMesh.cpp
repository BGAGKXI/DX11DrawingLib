#include "DxSystem.h"
#include "CubeMesh.h"


//������
CubeMesh::CubeMesh(const wchar_t* filename)
{
	ID3D11Device* device = pSystem->GetDevice();
	//���_���`
	VERTEX vertices[24] =
	{
		XMFLOAT3(0.5f,	0.5f,  -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f,  0.5f, -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f,  -0.5f, -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f, -0.5f, -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),		

		XMFLOAT3(-0.5f,	0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f,  0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f,-0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f, -0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),		

		XMFLOAT3(0.5f,	0.5f, 0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f,  0.5f, -0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f, -0.5f, 0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f, -0.5f, -0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),		

		XMFLOAT3(-0.5f,	0.5f, -0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f,  0.5f, 0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f, -0.5f, -0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f, -0.5f, 0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),		

		XMFLOAT3(0.5f,	 0.5f,  0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f,  0.5f,  0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f,  0.5f, -0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f,  0.5f, -0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),		

		XMFLOAT3(0.5f,	 -0.5f, -0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f,  -0.5f, -0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(0.5f,  -0.5f, 0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),		
		XMFLOAT3(-0.5f,  -0.5f, 0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),		

	};

	//��̒��_�Ńo�[�e�b�N�X�o�b�t�@�쐬
	Mesh.iNumVertices = sizeof(vertices) / sizeof(vertices[0]);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*Mesh.iNumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		device->CreateBuffer(&bd, &InitData,
			&Mesh.VertexBuffer);
	}


	//�C���f�b�N�X���`
	unsigned int indices[36];
	for (int face = 0; face < 6; face++) {
		indices[face * 6] = face * 4;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;
	}


	//��̒��_�ŃC���f�b�N�X�o�b�t�@�쐬
	Mesh.iNumIndices = sizeof(indices) / sizeof(indices[0]);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*Mesh.iNumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		device->CreateBuffer(&bd, &InitData,
			Mesh.IndexBuffer.GetAddressOf());
	}


	//�e�N�X�`�����[�h
 	if (filename) {
		texture = std::make_unique<Texture>();
		texture->Load(filename);
	}


	// �萔�o�b�t�@����(�s��)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPerMesh);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, ConstantBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert(false && "ID3D11Device::CreateBuffer() Failed.");
		return;
	}

	WorldMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);


}

CubeMesh::CubeMesh(std::shared_ptr<Texture> texture)
{
	ID3D11Device* device = pSystem->GetDevice();
	//���_���`
	VERTEX vertices[24] =
	{
		XMFLOAT3(0.5f,	0.5f,  -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f,  0.5f, -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f,  -0.5f, -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f, -0.5f, -0.5f),	XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),

		XMFLOAT3(-0.5f,	0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f,  0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f,-0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f, -0.5f, 0.5f),	XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),

		XMFLOAT3(0.5f,	0.5f, 0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f,  0.5f, -0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f, -0.5f, 0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f, -0.5f, -0.5f),	XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),

		XMFLOAT3(-0.5f,	0.5f, -0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f,  0.5f, 0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f, -0.5f, -0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f, -0.5f, 0.5f),	XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),

		XMFLOAT3(0.5f,	 0.5f,  0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f,  0.5f,  0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f,  0.5f, -0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f,  0.5f, -0.5f),	XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),

		XMFLOAT3(0.5f,	 -0.5f, -0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f,  -0.5f, -0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0,0),XMFLOAT4(1,1,1,1),
		XMFLOAT3(0.5f,  -0.5f, 0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1,1),XMFLOAT4(1,1,1,1),
		XMFLOAT3(-0.5f,  -0.5f, 0.5f),	XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0,1),XMFLOAT4(1,1,1,1),

	};

	//��̒��_�Ńo�[�e�b�N�X�o�b�t�@�쐬
	Mesh.iNumVertices = sizeof(vertices) / sizeof(vertices[0]);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*Mesh.iNumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		device->CreateBuffer(&bd, &InitData,
			&Mesh.VertexBuffer);
	}


	//�C���f�b�N�X���`
	unsigned int indices[36];
	for (int face = 0; face < 6; face++) {
		indices[face * 6] = face * 4;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;
	}


	//��̒��_�ŃC���f�b�N�X�o�b�t�@�쐬
	Mesh.iNumIndices = sizeof(indices) / sizeof(indices[0]);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*Mesh.iNumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		device->CreateBuffer(&bd, &InitData,
			Mesh.IndexBuffer.GetAddressOf());
	}


	//�e�N�X�`��
	this->texture = texture;

	// �萔�o�b�t�@����(�s��)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPerMesh);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, ConstantBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert(false && "ID3D11Device::CreateBuffer() Failed.");
		return;
	}


}







void CubeMesh::Update()
{
	//�g��
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]
	XMMATRIX a = XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//�ړ�
	XMMATRIX p = XMMatrixTranslation(pos.x, pos.y, pos.z);
	// �s��̍���
	XMMATRIX mW = s*a*p;
	XMStoreFloat4x4(&WorldMatrix, mW);

}

void CubeMesh::Render(Shader* shader, const XMMATRIX& view, const XMMATRIX& projection)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	shader->Activate();

	ConstantBufferForPerMesh cb;
	cb.world = WorldMatrix;
	XMStoreFloat4x4(&cb.matWVP,XMLoadFloat4x4(&WorldMatrix)*(view)*(projection));
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->HSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->DSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());


	//�ύX�@�v���~�e�B�u�E�g�|���W�[���Z�b�g
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, Mesh.VertexBuffer.GetAddressOf(), &stride, &offset);
	//�C���f�b�N�X�o�b�t�@
	device_context->IASetIndexBuffer(Mesh.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//�e�N�X�`���ݒ�
	if (texture) 
		texture->Set(0);


	//�����_�����O(�C���f�b�N�X�t��)
	device_context->DrawIndexed(Mesh.iNumIndices, 0, 0);
	//�V�F�[�_�[������
	shader->Inactivate();

}




