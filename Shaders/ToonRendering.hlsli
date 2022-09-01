//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

Texture2D RampTexture : register(t1);
SamplerState RampSampler : register(s1);


//--------------------------------------------
//	�R���X�^���g�o�b�t�@
//--------------------------------------------
cbuffer	CBPerMesh	: register(b0)
{
	row_major matrix	World;
	row_major matrix matWVP;
};

cbuffer	CBPerFrame2	: register(b2)
{
	float4	LightColor;		//	���C�g�̐F
	float4	LightDir;		//	���C�g�̕���
	float4	AmbientColor;	//	����
	float4  EyePos;		//	�J�������W
};

cbuffer CBPerFrame3 : register(b3)
{
	float4 Paramter; //	x = �֊s���̑���, yzw = ��
}

//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct	VSInput
{
	float3	Position	: POSITION;
	float3	Normal	: NORMAL;
	float2	Tex		: TEXCOORD;
};

//	�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
struct	GSInput
{
	float3	Position	: POSITION;
	float3	Normal	: TEXCOORD1;
	float2	Tex		: TEXCOORD2;
	float4	Color		: COLOR0;
};

//	�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
struct	PSInput
{
	
	float4	Position	: SV_POSITION;
	float3	wNormal	: TEXCOORD1;	//���[���h�@��
	float3	wPosition	: TEXCOORD2;	//���[���h���W
	float2	Tex		: TEXCOORD3;
	float4	Color		: COLOR0;
	
};

