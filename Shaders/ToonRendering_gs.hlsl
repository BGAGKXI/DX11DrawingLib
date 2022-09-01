#include	"ToonRendering.hlsli"
//--------------------------------------------
//	�W�I���g���V�F�[�_�[
//--------------------------------------------
[maxvertexcount(6)]
void main(triangle GSInput input[3], inout TriangleStream<PSInput> output)
{
	PSInput element;
	int i;
	//�ʏ�̖ʂ��쐬
	for (i = 0; i < 3; ++i)
	{
		element.Position = mul(float4(input[i].Position, 1), matWVP);
		element.wNormal = mul(float4(input[i].Normal, 0), World).xyz;
		element.wPosition = mul(float4(input[i].Position, 1), World).xyz;
		element.Tex = input[i].Tex;
		element.Color = input[i].Color;
		//	�����Œ��_�𐶐����Ă���
		output.Append(element);
	}

	//�ǉ��������_�Ńv���~�e�B�u���\�����A�V�����v���~�e�B�u�𐶐�����
	output.RestartStrip();

	//	���ʂŊg�債�ďo�͂��쐬
	for (i = 2; i >= 0; --i)
	{
		element.Position = mul(float4(input[i].Position + input[i].Normal * Paramter.x, 1), matWVP);
		element.wNormal = mul(float4(input[i].Normal, 0), World).xyz;
		element.wPosition = mul(float4(input[i].Position, 1), World).xyz;
		element.Tex = input[i].Tex;
		element.Color = float4(0, 0, 0, 1);
		//	�����Œ��_�𐶐����Ă���
		output.Append(element);
	}
	//�ǉ��������_�Ńv���~�e�B�u���\�����A�V�����v���~�e�B�u�𐶐�����
	output.RestartStrip();
}
