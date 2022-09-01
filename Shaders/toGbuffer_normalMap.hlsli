//--------------------------------------------
//	�e�N�X�`���E�T���v��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
Texture2D NormalTexture : register(t1);
SamplerState DecalSampler : register(s0);
//--------------------------------------------
//	�R���X�^���g�o�b�t�@
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	row_major matrix  World;
	row_major matrix  matWVP;
};
cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4	AmbientColor;	//����
	float4  EyePos;		//�J�������W
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
	float4 Position  	:SV_POSITION;
	float4 Color		:COLOR0;
	float3 wNormal		:TEXCOORD1;
	float3 wPos			:TEXCOORD2;
	float2 Tex			:TEXCOORD3;
	float3 vTan			:TEXCOORD4;
	float3 vBin			:TEXCOORD5;
	float3 vNor			:TEXCOORD6;
};

struct PSOutput
{
	float4 Color : SV_TARGET0;
	float4 wNormal : SV_TARGET1;
	float4 wPosition : SV_TARGET2;
};
