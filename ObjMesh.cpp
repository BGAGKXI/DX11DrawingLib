#include "DxSystem.h"

#include "ObjMesh.h"
#include "locale.h"
#include <string>


ObjMesh::ObjMesh( const char* FileName)
{
	HRESULT hr = LoadStaticMesh(FileName);
	assert(SUCCEEDED(hr));
}

//
//
//
ObjMesh::~ObjMesh()
{
	SAFE_DELETE_ARRAY(m_pMaterial);
	for (DWORD i = 0; i < m_dwNumMaterial; i++)
	{
		m_ppIndexBuffer[i]->Release();
	}
	SAFE_DELETE_ARRAY(m_ppIndexBuffer);
}

HRESULT ObjMesh::LoadMaterialFromFile(LPSTR FileName,MY_MATERIAL** ppMaterial)
{
	//�}�e���A���t�@�C�����J���ē��e��ǂݍ���
	FILE* fp=NULL;
	fopen_s(&fp,FileName,"rt");
	char key[110]={0};
	XMFLOAT4 v(0,0,0,1);

	//�}�e���A�����𒲂ׂ�
	m_dwNumMaterial=0;
	while(!feof(fp))
	{
		//�L�[���[�h�ǂݍ���
		fscanf_s(fp,"%s ",key,sizeof(key));
		//�}�e���A����
		if(strcmp(key,"newmtl")==0)
		{
			m_dwNumMaterial++;
		}
	}
	MY_MATERIAL* pMaterial=new MY_MATERIAL[m_dwNumMaterial];
	
	//�{�ǂݍ���	
	fseek(fp,SEEK_SET,0);
	INT iMCount=-1;

	while(!feof(fp))
	{
		//�L�[���[�h�ǂݍ���
		fscanf_s(fp,"%s ",key,sizeof(key));
		//�}�e���A����
		if(strcmp(key,"newmtl")==0)
		{
			iMCount++;
			fscanf_s(fp,"%s ",key,sizeof(key));
			strcpy_s(pMaterial[iMCount].szName,key);			
		}
		//Ka�@�A���r�G���g
		if (strcmp(key, "Ka") == 0)
		{
			fscanf_s(fp, "%f %f %f", &v.x, &v.y, &v.z);
			pMaterial[iMCount].Ka = v;
		}
		//Kd�@�f�B�t���[�Y
		if (strcmp(key, "Kd") == 0)
		{
			fscanf_s(fp, "%f %f %f", &v.x, &v.y, &v.z);
			pMaterial[iMCount].Kd = v;
		}
		//Ks�@�X�y�L�����[
		if (strcmp(key, "Ks") == 0)
		{
			fscanf_s(fp, "%f %f %f", &v.x, &v.y, &v.z);
			pMaterial[iMCount].Ks = v;
		}


		//�e�N�X�`����
		if(strcmp(key,"map_Kd")==0)
		{
			char fullpass[255] = { 0 };
			fscanf_s(fp, "%s", &pMaterial[iMCount].szTextureName, sizeof(pMaterial[iMCount].szTextureName));
			strcpy_s(fullpass, filepass);
			strcat_s(fullpass, pMaterial[iMCount].szTextureName);

			//�e�N�X�`���[���쐬
			pMaterial[iMCount].tex = std::make_unique<Texture>();
			//�}���`�o�C�g����������C�h������ɕϊ�
			WCHAR wfullpass[255] = { 0 };
			size_t wLen = 0;
			errno_t err = 0;
			setlocale(LC_ALL, "japanese");
			err = mbstowcs_s(&wLen, wfullpass, 255, fullpass, _TRUNCATE);
			pMaterial[iMCount].tex->Load(wfullpass);

			memset(wfullpass, 0, sizeof(wfullpass));

		}


	}
	fclose(fp);

	*ppMaterial = pMaterial;
	return S_OK;
}
//
//
//
HRESULT ObjMesh::LoadStaticMesh(const char* FileName)
{
	ID3D11Device* device = pSystem->GetDevice();
	float x,y,z;
	int v1=0,v2=0,v3=0;
	int vn1=0,vn2=0,vn3=0;
	int vt1=0,vt2=0,vt3=0;
	DWORD dwVCount=0;//�ǂݍ��݃J�E���^�[
	DWORD dwVNCount=0;//�ǂݍ��݃J�E���^�[
	DWORD dwVTCount=0;//�ǂݍ��݃J�E���^�[
	DWORD dwFCount=0;//�ǂݍ��݃J�E���^�[

	//char filepass[200] = { 0 };
	char key[200]={0};
	//OBJ�t�@�C�����J���ē��e��ǂݍ���
	FILE* fp=NULL;
	fopen_s(&fp,FileName,"rt");
	int len = strlen(FileName);
	bool flg = false;
	for (int i = len - 1; i >= 0; i--) {
		if (FileName[i] == '/')flg = true;
		if (FileName[i] == '\\')flg = true;
		if (flg) {
			filepass[i] = FileName[i];
		}
		else {
			filepass[i] = '\0';
		}
	}

	//���O�ɒ��_���A�|���S�����𒲂ׂ�
	while(!feof(fp))
	{
		//�L�[���[�h�ǂݍ���
		fscanf_s(fp,"%s ",key,sizeof(key));
		//�}�e���A���ǂݍ���
		if(strcmp(key,"mtllib")==0)
		{
			fscanf_s(fp, "%s\0", key, sizeof(key));
			char fullpass[255] = { 0 };
			strcpy_s(fullpass, filepass);
			strcat_s(fullpass, key);
			LoadMaterialFromFile(fullpass, &m_pMaterial);
			
		}					
		//���_
		if(strcmp(key,"v")==0)
		{
			m_dwNumVert++;
		}
		//�@��
		if(strcmp(key,"vn")==0)
		{
			dwVNCount++;
		}
		//�e�N�X�`���[���W
		if(strcmp(key,"vt")==0)
		{
			dwVTCount++;
		}
		//�t�F�C�X�i�|���S���j
		if(strcmp(key,"f")==0)
		{
			m_dwNumFace++;
		}
	}

	//�ꎞ�I�ȃ������m�ہi���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�j
	std::unique_ptr<MY_VERTEX[]> pvVertexBuffer = std::make_unique<MY_VERTEX[]>( m_dwNumFace*3 );
	std::unique_ptr<XMFLOAT3[]> pvCoord = std::make_unique<XMFLOAT3[]>( m_dwNumVert );
	std::unique_ptr<XMFLOAT3[]> pvNormal= std::make_unique<XMFLOAT3[]>(dwVNCount );
	std::unique_ptr<XMFLOAT2[]> pvTexture= std::make_unique<XMFLOAT2[]>(dwVTCount);

	
	//�{�ǂݍ���	
	fseek(fp,SEEK_SET,0);
	dwVCount=0;
	dwVNCount=0;
	dwVTCount=0;
	dwFCount=0;	

	while(!feof(fp))
	{
		//�L�[���[�h �ǂݍ���
		ZeroMemory(key,sizeof(key));
		fscanf_s(fp,"%s ",key,sizeof(key));

		//���_ �ǂݍ���
		if(strcmp(key,"v")==0)
		{		
			fscanf_s(fp,"%f %f %f",&x,&y,&z);
			pvCoord[dwVCount].x=-x;
			pvCoord[dwVCount].y=y;
			pvCoord[dwVCount].z=z;
			dwVCount++;
		}

		//�@�� �ǂݍ���
		if(strcmp(key,"vn")==0)
		{		
			fscanf_s(fp,"%f %f %f",&x,&y,&z);
			pvNormal[dwVNCount].x=-x;
			pvNormal[dwVNCount].y=y;
			pvNormal[dwVNCount].z=z;
			dwVNCount++;
		}

		//�e�N�X�`���[���W �ǂݍ���
		if(strcmp(key,"vt")==0)
		{		
			fscanf_s(fp,"%f %f",&x,&y);
			pvTexture[dwVTCount].x=x;
			pvTexture[dwVTCount].y=1-y;//OBJ�t�@�C����Y�������t�Ȃ̂ō��킹��
			dwVTCount++;
		}		
	}	

	//�}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
	m_ppIndexBuffer=new ID3D11Buffer*[m_dwNumMaterial];

	//�t�F�C�X�@�ǂݍ��݁@�o���o���Ɏ��^����Ă���\��������̂ŁA�}�e���A�����𗊂�ɂȂ����킹��
	bool boFlag=false;
	int* piFaceBuffer=new int[ m_dwNumFace*3 ];//�R���_�|���S���Ȃ̂ŁA1�t�F�C�X=3���_(3�C���f�b�N�X)
	dwFCount=0;
	DWORD dwPartFCount=0;
	for(DWORD i=0;i<m_dwNumMaterial;i++)
	{		
		dwPartFCount=0;
		fseek(fp,SEEK_SET,0);		

		while(!feof(fp))
		{
			//�L�[���[�h �ǂݍ���
			ZeroMemory(key,sizeof(key));
			fscanf_s(fp,"%s ",key,sizeof(key));

			//�t�F�C�X �ǂݍ��݁����_�C���f�b�N�X��
			if(strcmp(key,"usemtl")==0)
			{
				fscanf_s(fp,"%s ",key,sizeof(key));
				if(strcmp(key,m_pMaterial[i].szName)==0)
				{
					boFlag=true;
				}
				else
				{
					 boFlag=false;
				}
			}
			if(strcmp(key,"f")==0 && boFlag==true)
			{
				if(m_pMaterial[i].tex!=NULL)//�e�N�X�`���[����T�[�t�F�C�X
				{
					fscanf_s(fp,"%d/%d/%d %d/%d/%d %d/%d/%d",&v1,&vt1,&vn1,&v2,&vt2,&vn2,&v3,&vt3,&vn3);
					//���_�\���̂ɑ��
					pvVertexBuffer[dwFCount * 3].vPos = pvCoord[v1 - 1];
					pvVertexBuffer[dwFCount * 3].vNorm = pvNormal[vn1 - 1];
					pvVertexBuffer[dwFCount * 3].vTex = pvTexture[vt1 - 1];
					pvVertexBuffer[dwFCount * 3 + 1].vPos = pvCoord[v2 - 1];
					pvVertexBuffer[dwFCount * 3 + 1].vNorm = pvNormal[vn2 - 1];
					pvVertexBuffer[dwFCount * 3 + 1].vTex = pvTexture[vt2 - 1];
					pvVertexBuffer[dwFCount * 3 + 2].vPos = pvCoord[v3 - 1];
					pvVertexBuffer[dwFCount * 3 + 2].vNorm = pvNormal[vn3 - 1];
					pvVertexBuffer[dwFCount * 3 + 2].vTex = pvTexture[vt3 - 1];

				}
				else//�e�N�X�`���[�����T�[�t�F�C�X
				{
					fscanf_s(fp,"%d//%d %d//%d %d//%d",&v1,&vn1,&v2,&vn2,&v3,&vn3);
					//���_�\���̂ɑ��
					pvVertexBuffer[dwFCount * 3].vPos = pvCoord[v1 - 1];
					pvVertexBuffer[dwFCount * 3].vNorm = pvNormal[vn1 - 1];
					pvVertexBuffer[dwFCount * 3 + 1].vPos = pvCoord[v2 - 1];
					pvVertexBuffer[dwFCount * 3 + 1].vNorm = pvNormal[vn2 - 1];
					pvVertexBuffer[dwFCount * 3 + 2].vPos = pvCoord[v3 - 1];
					pvVertexBuffer[dwFCount * 3 + 2].vNorm = pvNormal[vn3 - 1];

				}

				//�C���f�b�N�X�o�b�t�@�[(�ʂ̌����ύX)
				piFaceBuffer[dwPartFCount*3]=dwFCount*3;
				piFaceBuffer[dwPartFCount*3+1]=dwFCount*3+2;
				piFaceBuffer[dwPartFCount*3+2]=dwFCount*3+1;

				dwPartFCount++;
				dwFCount++;		
			}
		}
		if(dwPartFCount==0)//�g�p����Ă��Ȃ��}�e���A���΍�
		{
			m_ppIndexBuffer[i]= NULL;
			continue;
		}
		if (dwPartFCount == 0)//�g�p����Ă��Ȃ��}�e���A���΍�i�Ȃ�炩�̗��R�Ŗ��O������`����Ă���Ȃǁj
		{
			m_pMaterial[i].dwNumFace = 0;
			continue;
		}


		//�C���f�b�N�X�o�b�t�@�[���쐬
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( int ) * dwPartFCount * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = piFaceBuffer;
		HRESULT hr = device->CreateBuffer(&bd, &InitData, &m_ppIndexBuffer[i]);
		assert(SUCCEEDED(hr));

		m_pMaterial[i].dwNumFace=dwPartFCount;
	}
	delete[] piFaceBuffer;
	fclose(fp);

	HRESULT hr = S_OK;
	//�o�[�e�b�N�X�o�b�t�@�[���쐬
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof( MY_VERTEX ) *m_dwNumFace*3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pvVertexBuffer.get();
	hr =device->CreateBuffer(&bd, &InitData, m_pVertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SIMPLECONSTANT_BUFFER0);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = device->CreateBuffer(&cb, NULL, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�}�e���A���P�ʂŕύX������
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(CBUFF_PER_MATERIAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	hr = device->CreateBuffer(&cb, NULL, ConstantBufferPerMaterial.GetAddressOf());
	assert(SUCCEEDED(hr));




	return S_OK;
}



void ObjMesh::Update()
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

//
//
//
void ObjMesh::Render(Shader* shader, const XMMATRIX& view, const XMMATRIX& projection,
	D3D_PRIMITIVE_TOPOLOGY topology)
{
	ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
	// �V�F�[�_�ݒ�
	shader->Activate();

	SIMPLECONSTANT_BUFFER0 cb;
	cb.world = WorldMatrix;
	XMStoreFloat4x4(&cb.matWVP, XMLoadFloat4x4(&WorldMatrix)*(view)*(projection));
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->DSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());//���ǉ�


	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	device_context->IASetPrimitiveTopology(topology);
	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof( MY_VERTEX );
	UINT offset = 0;
	device_context->IASetVertexBuffers( 0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset );
	//�}�e���A���̐������A���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@�|��`��
	for(DWORD i=0;i<m_dwNumMaterial;i++)
	{
		//�g�p����Ă��Ȃ��}�e���A���΍�
		if(m_pMaterial[i].dwNumFace==0)
		{
			continue;
		}
		//�C���f�b�N�X�o�b�t�@�[���Z�b�g
		stride = sizeof( int );
		offset = 0;
		device_context->IASetIndexBuffer(m_ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0 );
		//�}�e���A���v�f���V�F�[�_�[�ɓn��
		D3D11_MAPPED_SUBRESOURCE pData;

		device_context->Map(ConstantBufferPerMaterial.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
		{
			CBUFF_PER_MATERIAL sg;
			sg.vAmbient = m_pMaterial[i].Ka;//�A���r�G���g�����V�F�[�_�[�ɓn��
			sg.vDiffuse = m_pMaterial[i].Kd;//�f�B�t���[�Y�J���[���V�F�[�_�[�ɓn��
			sg.vSpecular = m_pMaterial[i].Ks;//�X�y�L�����[���V�F�[�_�[�ɓn��
			memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(CBUFF_PER_MATERIAL));
			device_context->Unmap(ConstantBufferPerMaterial.Get(), 0);
		}
		device_context->PSSetConstantBuffers(1, 1, ConstantBufferPerMaterial.GetAddressOf());

		
		//�e�N�X�`���[���V�F�[�_�[�ɓn��

		if(m_pMaterial[i].szTextureName[0] != NULL)
		{
			m_pMaterial[i].tex->Set(0);
		}
		//�v���~�e�B�u�������_�����O
		device_context->DrawIndexed(m_pMaterial[i].dwNumFace*3 , 0 ,0);

		m_pMaterial[i].tex->Set(0,FALSE);

	}
	//�V�F�[�_�[������
	shader->Inactivate();

}

