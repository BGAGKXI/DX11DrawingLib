//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

Texture2D FurTexture : register(t1);
SamplerState FurSampler : register(s1);

//--------------------------------------------
//	�萔
//--------------------------------------------
static const int LAYER = 8;			//fur�̒���


//--------------------------------------------
//	�R���X�^���g�o�b�t�@
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	row_major float4x4  World;
	row_major float4x4	matWVP;
};
cbuffer CBPerFrame : register(b2)
{
	float4  LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4  AmbientColor;	//����
	float4	EyePos;			//�J�����ʒu
};

cbuffer CBPerFrame2 : register(b3)
{
	float	Density;		//�т̖��x
	float	Distance;		//�т̒���
	float	dummy1;
	float	dummy2;
};


//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
};
struct GSInput
{
	float4 Position : POSITION;
	float3 Normal: NORMAL;
	float2 Tex : TEXCOORD;
};
struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
	float3 wNormal: TEXCOORD1;
	float3 wPos : TEXCOORD2;
};

