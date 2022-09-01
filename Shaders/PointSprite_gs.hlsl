#include "PointSprite.hlsli"
//--------------------------------------------
//	�W�I���g���V�F�[�_�[
//--------------------------------------------
// 1���_ �� �S�p�`
[maxvertexcount(4)]
void main(point GSInput In[1],
	inout TriangleStream<PSInput> ParticleStream)
{
	// ���W�ϊ� (���[���h���W�n �� �r���[���W�n)
	float4 pos = mul(float4(In[0].Position, 1.0), View);
	// �_��ʂɂ���(�S���_�����)
	float rot = In[0].Param.x;
	float s = sin(rot);
	float c = cos(rot);
	float4 right =
		float4(c, -s, 0, 0) * (In[0].Size.x * 0.5);
	float4 up =
		float4(s, c, 0, 0) * (In[0].Size.y * 0.5);

	float4 pos_left_top = pos - right + up;
	float4 pos_left_bottom = pos - right - up;
	float4 pos_right_top = pos + right + up;
	float4 pos_right_bottom = pos + right - up;

	// �^�C�v�ݒ�
	uint type = (uint)In[0].Param.y;
	uint komax = (uint)In[0].Param.z;
	uint komay = (uint)In[0].Param.w;
	float w = 1.0 / komax;
	float h = 1.0 / komay;
	float2 uv = float2((type % komax) * w, (type / komax) * h);

	// ����̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	PSInput Out = (PSInput)0;
	Out.Color = In[0].Color;
	Out.Position = mul(pos_left_top, Projection);
	Out.Tex = uv + float2(0, 0); // �e�N�X�`������
	ParticleStream.Append(Out);
	// �E��̓_�̈ʒu(�ˉe���W�n) �ƃe�N�X�`�����W�̌v�Z�����ďo��
	Out.Color = In[0].Color;
	Out.Position = mul(pos_right_top, Projection);
	Out.Tex = uv + float2(w, 0); // �e�N�X�`��
	ParticleStream.Append(Out);

	// �����̓_�̈ʒu(�ˉe���W�n) �ƃe�N�X�`�����W�̌v�Z�����ďo��
	Out.Color = In[0].Color;
	Out.Position = mul(pos_left_bottom, Projection);
	Out.Tex = uv + float2(0, h); // �e�N�X�`��
	ParticleStream.Append(Out);

	// �E���̓_�̈ʒu(�ˉe���W�n) �ƃe�N�X�`�����W�̌v�Z�����ďo��
	Out.Color = In[0].Color;
	Out.Position = mul(pos_right_bottom, Projection);
	Out.Tex = uv + float2(w, h); // �e�N�X�`��
	ParticleStream.Append(Out);

	ParticleStream.RestartStrip();
}
