//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	�R���X�^���g�o�b�t�@
//--------------------------------------------

cbuffer CBPerFrame : register(b0)
{
	row_major float4x4 View; // �r���[�ϊ��s��
	row_major float4x4 Projection; // �����ϊ��s��
};
//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Size     : TEXCOORD;
	float4 Color    : COLOR;
};

struct GSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Size     : TEXCOORD;
	float4 Color    : COLOR;
};

struct PSInput
{
	float4 Position  : SV_POSITION;// ���_���W(�ˉe���W�n)
	float2 Tex  : TEXCOORD;
	float4 Color : COLOR0;
};
