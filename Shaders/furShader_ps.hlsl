#include "furShader.hlsli"
#include "Func.hlsli"
//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInput input) : SV_TARGET
{
	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
	float alpha = FurTexture.Sample(FurSampler, input.Tex * Density).r;
	float3 N = normalize(input.wNormal);
	float3 E = normalize(EyePos.xyz - input.wPos);
	float3 L = normalize(LightDir.rgb);

	// ����
	float3 A = AmbientColor.rgb;

	// �g�U����
	float3 C = LightColor.rgb;
	float3 Kd = float3(1,1,1);
	float3 D = HalfLambert(N, L, C, Kd);

	// ���ʔ���
	float3 Ks = float3(2, 2, 2);
	float3 S = BlinnPhongSpecular(N, L, C, E, Ks,10);

	// �t�@�[�ȊO�𓧉߂���
	color.a = alpha;

	return color * input.Color * float4(A + D + S  , 1);
}
