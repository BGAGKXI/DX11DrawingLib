//--------------------------------------------
//	�g�U���ˊ֐�(�����o�[�g)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// ���̒l���O�ɂ���
	return C * D * K;
}

//--------------------------------------------
//	�n�[�t�����o�[�g
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
float3 HalfLambert(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = D * 0.5 + 0.5;
	//D = D * D;	// ���������Ɍ�������
	return C * D * K;
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
	S = S * K * C;
	return S;
}
//--------------------------------------------
//	���ʔ��ˊ֐�(�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//���˃x�N�g��
	float3 R = normalize(reflect(L,N));
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}
//--------------------------------------------
// �������C�g
//--------------------------------------------
// N:�@��(���K���ς�)
// E:���_�����x�N�g��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// LightColor :���C�g�F
float3 RimLight(
	float3 N, float3 E, float3 L, float3 LightColor)
{
	float rim = 1.0 - max(dot(N, E), 0.0);
	float ratio = max(dot(L, E), 0);
	return  LightColor * rim * ratio;
}


//--------------------------------------------
// �������C�g
//--------------------------------------------
// N:�@��(���K���ς�)
// UP:������i�Е��j
// SkyColor :��(��)�F
// GroundColor :�n��(��)�F
float3 HemiSphereLight(float3 N, float3 UP, float3 SkyColor, float3 GroundColor)
{
	float skyblend = dot(N, UP);
	float groundblend = skyblend < 0.0 ? -skyblend : 0.0;
	skyblend = max(0, skyblend);
	return (SkyColor * skyblend + GroundColor * groundblend);
}

//(dot(normal, invSky) + 1.0) * 0.5;
//--------------------------------------------
//	�t�H�O
//--------------------------------------------
//color		:���݂̐F
//viewPos	:���_
//Pos		:�s�N�Z���̈ʒu
//fogColor	:�t�H�O�̐F
//Near		:�t�H�O�̋ߋ���
//Far		:�t�H�O�̉�����
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, 
float Near, float Far)
{
	float Len = length(viewPos - Pos);
	float fogalpha = saturate((Len - Near) / (Far - Near));
	return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}


//color		:���݂̐F
//viewPos	:���_
//Pos		:�s�N�Z���̈ʒu
//fogColor	:�t�H�O�̐F
//Near		:�t�H�O�̋ߋ���
//Far		:�t�H�O�̉�����
float3 Fog2(float3 color, float3 Pos, float3 fogColor, float Low, float Heigh)
{
	float Len = Heigh - Pos.y;
	float fogalpha = saturate((Len) / (Heigh - Low));
	return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}

