#include "toGbuffer_normalMap.hlsli"
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
	output.wPos = wPos;

	//�ڋ�ԍs��
	float3 vy = { 0.0f,1.0f,0.001f };//���x�N�g��
	float3 vz = wN;
	float3 vx;
	vx = cross(vy, vz);
	vx = normalize(vx);
	vy = cross(vz, vx);
	vy = normalize(vy);
	output.vTan = vx;
	output.vBin = vy;
	output.vNor = vz;

	return output;
}
