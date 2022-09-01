#include "deferredLight.hlsli"
#include "Func.hlsli"
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
	color *= input.Color * float4(A + D + S, 1.0) * input.Color;
	return color;
}
