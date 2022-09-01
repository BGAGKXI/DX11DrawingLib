#include "flatShader.hlsli"
//--------------------------------------------
//	�W�I���g���V�F�[�_�[
//--------------------------------------------
[maxvertexcount(3)]
void main(
	triangle GSInput input[3],				//����
	inout TriangleStream<PSInput> Stream)	//�o��
{
	//�ʖ@���Z�o
	float3 v1 = input[1].Position.xyz - input[0].Position.xyz;
	float3 v2 = input[2].Position.xyz - input[1].Position.xyz;
	float3 N = normalize(cross(v1, v2));
	for (int i = 0; i < 3; i++) {
		PSInput data = (PSInput)0;
		data.Position = mul(input[i].Position, matWVP);
		data.wNormal = normalize(mul(N, (float3x3)World));
		data.Tex = input[i].Tex;
		data.Color = input[i].Color;
		data.wPos = input[i].wPos;
		Stream.Append(data);
	}
}