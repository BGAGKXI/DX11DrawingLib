//--------------------------------------------
//	�e�N�X�`��/�T���v���[
//--------------------------------------------

Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D SpecularTexture : register(t3);
Texture2D ShadowTexture : register(t4);
SamplerState DecalSampler : register(s0);
SamplerState ShadowSampler : register(s1);


//--------------------------------------------
//	�R���X�^���g�o�b�t�@
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	row_major float4x4 World;
	row_major float4x4 matWVP;
};

cbuffer CBPerFrame : register(b2)
{
	float4	LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4	AmbientColor;	//����
	float4  EyePos;			//�J�������W
};

//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};

//�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//���[���h�@��
	float3 wPos : TEXCOORD2;	//���[���h���W
	float2 Tex : TEXCOORD3;
};

struct PSOutput
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Position : SV_TARGET2;
};

