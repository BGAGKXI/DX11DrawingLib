#include "DOF.hlsli"
//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInput input) : SV_TARGET0
{
	float4 color = (float4)0;
	color = DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color;
	float3 bokeh = BokehTexture.Sample(DecalSampler, input.Tex).rgb;
	float depth = DepthTexture.Sample(DecalSampler, input.Tex).r;
	//�u�����h�t�@���t�@�l
		float alpha = abs(depth - dist) / focus;
		alpha = alpha > 1 ? 1 : alpha;
		color.rgb = color.rgb * (1.0 - alpha) + bokeh * (alpha);	//����
		return color;
}
