#include "deferredPointSpotLightShadow.hlsli"
#include "Func.hlsli"
#include "PointLight.hlsli"
#include "SpotLight.hlsli"
#include "ShadowFunc.hlsli"

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInput input) : SV_TARGET0
{
	float4 color = ColorTexture.Sample(DecalSampler,input.Tex);
	float3 P = PositionTexture.Sample(DecalSampler, input.Tex).xyz;
	float3 N = NormalTexture.Sample(DecalSampler, input.Tex).xyz;

	float3 E = normalize(EyePos.xyz - P);
	float3 L = normalize(LightDir.xyz);
	// ����
	float3 A = AmbientColor.rgb;

	//�g�U����
	float3 C = LightColor.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = Diffuse(N, L, C, Kd);
	//���ʔ���
	float3 Ks = float3(1, 1, 1);
	float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 20);
	//�|�C���g���C�g
	float3 PL = 0;
	PL = AddPointLight(P, N, E, Kd, Ks);

	//�X�|�b�g���C�g
	float3 SL = 0;
	SL = AddSpotLight(P, N, E, Kd, Ks);

	//-----------------------------------------------
	//				�V���h�[�v�Z
	//-----------------------------------------------
	//���[���h��ԍ��W����V���h�[�}�b�v���
	float3 ShadowPos = 0;
	ShadowPos = GetShadowTex(ShadowViewProjection, P);
	//�V���h�[�}�b�v����[�x�擾�Ɣ�r
	D *= GetShadow(ShadowTexture, ShadowSampler, ShadowPos, ShadowColor.rgb, Bias);
	color *= input.Color * float4(A + D + S + PL + SL, 1.0);

	return color;
}
