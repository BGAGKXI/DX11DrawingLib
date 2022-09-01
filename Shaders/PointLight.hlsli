
//--------------------------------------------
//	�|�C���g���C�g�\����
//--------------------------------------------

struct POINTLIGHT {
	float index;
	float range;//���̓͂��͈�
	float type; //�L����������
	float dummy;
	float4 pos;
	float4 color;
};

//--------------------------------------------
//	�R���X�^���g�o�b�t�@
//--------------------------------------------
static const int POINTMAX = 32;

cbuffer CBPerFrame : register(b3)
{
	POINTLIGHT PointLight[POINTMAX];
};
//--------------------------------------------
//	�|�C���g���C�g�֐�
//--------------------------------------------

// P: �s�N�Z�����[���h�ʒu
// N: ���[���h�@��(���K���ς�)
// E: ���������x�N�g��(���K���ς�)
// Kd: �g�U���˗�
// Ks: ���ʔ��˗� 

float3 AddPointLight(float3 P, float3 N, float3 E, float3 Kd, float3 Ks)
{
	float3 PL;				//�|�C���g���C�g�x�N�g��
	float3 PC;				//�|�C���g���C�g�F
	float influence;		//�|�C���g���C�g�̉e��
	float3 PD = (float3)0;	//�g�U����
	float3 PS = (float3)0;	//���ʔ���
	for (int i = 0; i < POINTMAX; i++) {
		//�_�����Ă��Ȃ����C�g���O
		if (PointLight[i].type == 0) continue;
		PL = P - PointLight[i].pos.xyz;
		float d = length(PL);
		float r = PointLight[i].range;
		//�͂��Ȃ����C�g���O
		if (d > r) continue;
		influence = saturate(1.0f - d / r);
		PL = normalize(PL);
		PC = PointLight[i].color.rgb;
		//�g�U���̉��Z		
		PD += Diffuse(N, PL, PC, Kd)*influence;
		//���ʌ��̉��Z
		PS += PhongSpecular(N, PL, PC, E, Ks, 20)*influence;
	}
	return (PD + PS);
}

