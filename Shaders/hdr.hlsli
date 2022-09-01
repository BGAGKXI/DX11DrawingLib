//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
static float3 screen_color = { 0.2,1.0,0.8 };

cbuffer CBPerFrame2 : register(b2)
{
	float4		LightColor;	//���C�g�̐F
	float4		LightDir;		//���C�g�̕���
	float4		AmbientColor;	//����
	float4		EyePos;		//�J����
};

cbuffer CBPerFrame4 : register(b4)
{
	float		Threshold;		//�u���[���p臒l
	float		dummy0;
	float		dummy1;
	float		dummy2;
}

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

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
};
//--------------------------------------------
//	�֐�
//--------------------------------------------
//YIQ�ϊ�
float getBrightness(float3 color) {
	float3 y = { 0.299,0.587,0.114 };
	float bright = dot(y, color);
	return bright;
}

