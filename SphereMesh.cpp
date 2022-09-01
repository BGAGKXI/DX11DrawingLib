#include "DxSystem.h"
#include "SphereMesh.h"
#include <vector>


//������
SphereMesh::SphereMesh(std::shared_ptr<Texture>texture, int slices, int stacks) :texture(nullptr)
{
	ID3D11Device* device = pSystem->GetDevice();
	{
		//���_��
		Mesh.iNumVertices = (slices + 1)*(stacks + 1);

		// ���_��`
		VERTEX* vertices = new VERTEX[Mesh.iNumVertices];
		for (int y = 0; y < stacks + 1; y++) {
			for (int x = 0; x < slices + 1; x++) {
				int index = y * (slices + 1) + x;
				float h = 0.5f * cosf(y*XM_PI / stacks);
				float w = 0.5f * sinf(y*XM_PI / stacks);
				float rad_slices = x * XM_PI * 2.0f / slices;

				vertices[index].Pos.x = w*sinf(rad_slices);
				vertices[index].Pos.y = h;
				vertices[index].Pos.z = w*cosf(rad_slices);

				vertices[index].Normal.x = vertices[index].Pos.x * 2.0f;
				vertices[index].Normal.y = vertices[index].Pos.y * 2.0f;
				vertices[index].Normal.z = vertices[index].Pos.z * 2.0f;


				vertices[index].Tex.x = 1.0f - (float)x / slices;
				vertices[index].Tex.y = (float)y / stacks;
				vertices[index].Color = XMFLOAT4(1, 1, 1, 1);
			}
		}
		{
			// ���_�o�b�t�@�m��
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(VERTEX)*Mesh.iNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;//�ύX
			device->CreateBuffer(&bd, &InitData, Mesh.VertexBuffer.GetAddressOf());
		}


		// �C���f�b�N�X��
		Mesh.iNumIndices = stacks * slices * 2 * 3;
		// �C���f�b�N�X�ݒ�
		u_int *indices = new u_int[Mesh.iNumIndices];
		for (int y = 0; y < stacks; y++) {
			for (int x = 0; x < slices; x++) {
				int face = (y*slices + x);
				int vertices_index = y*(slices + 1) + x;
				indices[face * 6] = vertices_index + 1;
				indices[face * 6 + 1] = vertices_index;
				indices[face * 6 + 2] = vertices_index + (slices + 1);

				indices[face * 6 + 3] = vertices_index + 1;
				indices[face * 6 + 4] = vertices_index + (slices + 1);
				indices[face * 6 + 5] = vertices_index + (slices + 1) + 1;

			}
		}
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(u_int)*Mesh.iNumIndices;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = indices;
			device->CreateBuffer(&bd, &InitData, Mesh.IndexBuffer.GetAddressOf());

		}
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


SphereMesh::SphereMesh(const wchar_t* filename, int slices, int stacks) :texture(nullptr)
{
	ID3D11Device* device = pSystem->GetDevice();
	{
		//���_��
		Mesh.iNumVertices = (slices + 1)*(stacks + 1);

		// ���_��`
		VERTEX* vertices = new VERTEX[Mesh.iNumVertices];
		for (int y = 0; y < stacks + 1; y++) {
			for (int x = 0; x < slices + 1; x++) {
				int index = y * (slices + 1) + x;
				float h = 0.5f * cosf(y*XM_PI / stacks);
				float w = 0.5f * sinf(y*XM_PI / stacks);
				float rad_slices = x * XM_PI * 2.0f / slices;

				vertices[index].Pos.x = w*sinf(rad_slices);
				vertices[index].Pos.y = h;
				vertices[index].Pos.z = w*cosf(rad_slices);

				vertices[index].Normal.x = vertices[index].Pos.x * 2.0f;
				vertices[index].Normal.y = vertices[index].Pos.y * 2.0f;
				vertices[index].Normal.z = vertices[index].Pos.z * 2.0f;

				vertices[index].Tex.x = 1.0f - (float)x / slices;
				vertices[index].Tex.y = (float)y / stacks - 1.0f;
				vertices[index].Color = XMFLOAT4(1, 1, 1, 1);
			}
		}
		{
			// ���_�o�b�t�@�m��
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(VERTEX)*Mesh.iNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;//�ύX
			device->CreateBuffer(&bd, &InitData, Mesh.VertexBuffer.GetAddressOf());
		}

		// �C���f�b�N�X��
		Mesh.iNumIndices = stacks * slices * 2 * 3;
		// �C���f�b�N�X�ݒ�
		u_int *indices = new u_int[Mesh.iNumIndices];
		for (int y = 0; y < stacks; y++) {
			for (int x = 0; x < slices; x++) {
				int face = (y*slices + x);
				int vertices_index = y*(slices + 1) + x;
				indices[face * 6] = vertices_index + 1;
				indices[face * 6 + 1] = vertices_index;
				indices[face * 6 + 2] = vertices_index + (slices + 1);

				indices[face * 6 + 3] = vertices_index + 1;
				indices[face * 6 + 4] = vertices_index + (slices + 1);
				indices[face * 6 + 5] = vertices_index + (slices + 1) + 1;
			}
		}
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(u_int)*Mesh.iNumIndices;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = indices;
			device->CreateBuffer(&bd, &InitData, Mesh.IndexBuffer.GetAddressOf());

		}
	}


	//�e�N�X�`�����[�h
	texture = std::make_unique<Texture>();
	texture->Load(filename);

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



void SphereMesh::Update()
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
void SphereMesh::Render(Shader * shader, XMMATRIX & view, XMMATRIX & projection, D3D_PRIMITIVE_TOPOLOGY topology)
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


	//�ύX�@�v���~�e�B�u�E�g�|���W�[���Z�b�g
	device_context->IASetPrimitiveTopology(topology);
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


