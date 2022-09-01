Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


cbuffer CBPerMesh : register(b0)
{
	row_major matrix	World;
	row_major matrix	matWVP;
};
cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;	//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4	AmbientColor;	//����
	float4  EyePos;			//�J�������W
};

struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//���[���h�@��
	float3 wPosition : TEXCOORD2;	//���[���h���W
	float2 Tex : TEXCOORD3;
	float4 D : TEXCOORD4;
};

struct PSOutput
{
	float4 Color : SV_TARGET0;
	float4 Depth : SV_TARGET1;
};
