
//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------

static const float Bias = 0.0008;

cbuffer CBPerFrame : register(b5)
{
	row_major float4x4 ShadowViewProjection;
	float4 ShadowColor;
};


//--------------------------------------------
//	�V���h�[�}�b�v�֐�
//--------------------------------------------


//���[���h���W�����C�g��ԍ��W�ɕϊ�
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
	// ���K���f�o�C�X���W�n
	float4 wvpPos = float4(wPos, 1.0f);

	//wvpPos = mul(vp, wvpPos);
	wvpPos = mul(wvpPos,vp);
	wvpPos /= wvpPos.w;
	// �e�N�X�`�����W�n
	wvpPos.y = -wvpPos.y;
	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
	return wvpPos.xyz;
}

//�V���h�[�}�b�v����V���h�[��ԍ��W�ɕϊ���Z�l��r
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias) {
	// �V���h�E�}�b�v����[�x�����o��
	float d = st.Sample(ss, Tex.xy).r;
	// �V���h�E�}�b�v�̐[�x�l�ƌ����̐[�x�̔�r
	Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
	return Scolor;
}

