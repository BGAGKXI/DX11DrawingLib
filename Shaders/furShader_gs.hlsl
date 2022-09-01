#include "furShader.hlsli"
//--------------------------------------------
//	ジオメトリシェーダー
//--------------------------------------------
[maxvertexcount(LAYER * 3)]
void main(triangle GSInput input[3],	//入力プリミティブタイプ
	inout TriangleStream<PSInput> Stream)//出力データタイプ
{
	for (int j = 0; j < LAYER; j++) {
		for (int i = 0; i < 3; i++) {
			PSInput data = (PSInput)0;
			float4 P = input[i].Position;
			P.xyz += input[i].Normal * Distance * j;
			data.Position = mul(P, matWVP);
			data.wNormal
				= normalize(mul(input[i].Normal, (float3x3)World));
			data.Tex = input[i].Tex;
			data.Color = float4(1, 1, 1, 1);
			data.wPos = mul(P, World).xyz;
			Stream.Append(data);
		}
	}
	Stream.RestartStrip();
}
