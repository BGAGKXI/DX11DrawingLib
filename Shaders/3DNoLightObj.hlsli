//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);
Texture2D SubTexture : register(t1);

SamplerState DecalSampler : register(s0);



//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 matWVP;
};

cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4	AmbientColor;	//����
	float4  EyePos;			//�J�������W
};

static const float3 fogColor = { 0.8,0.8,0.6 };
static const float fogNear = 20.0;
static const float fogFar = 100.0;


//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//���_�J���[����
};



//�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//���[���h�@��
	float3 wPosition : TEXCOORD2;	//���[���h���W
	float2 Tex : TEXCOORD3;
};

//--------------------------------------------
//	�g�U���ˊ֐�
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�(0�`1.0)

//�����o�[�g�V�F�[�f�B���O
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// ���̒l���O�ɂ���
	return K * C * D;
}
//--------------------------------------------
//	���ʔ��ˊ֐�(�u�����E�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//�n�[�t�x�N�g��
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S*K*C;
	return S;
}
