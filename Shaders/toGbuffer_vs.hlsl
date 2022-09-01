#include "toGbuffer.hlsli"
//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------
PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;
	float4 P = float4(input.Position, 1.0);
	output.Position = mul(P, matWVP);
	float3 wPos = mul(P, World).xyz;
	float3 wN = mul(input.Normal, (float3x3)World).xyz;
	wN = normalize(wN);

	//�e�N�X�`���[���W
	output.Tex = input.Tex;
	output.wNormal = wN;
	output.wPosition = wPos;

	return output;
}
