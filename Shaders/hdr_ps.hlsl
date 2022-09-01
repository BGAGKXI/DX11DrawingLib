#include "hdr.hlsli"
//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------

float4 main(PSInput input) : SV_TARGET0
{
	float4 color = (float4)0;
	color = DiffuseTexture.Sample(DecalSampler, input.Tex)
								* input.Color;
	float brightness = getBrightness(color.rgb);
	float contribution = max(0, brightness - Threshold.x);
	contribution /= brightness;

	color.rgb *= contribution;
	return color;
}
