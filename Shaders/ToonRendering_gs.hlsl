#include	"ToonRendering.hlsli"
//--------------------------------------------
//	ジオメトリシェーダー
//--------------------------------------------
[maxvertexcount(6)]
void main(triangle GSInput input[3], inout TriangleStream<PSInput> output)
{
	PSInput element;
	int i;
	//通常の面を作成
	for (i = 0; i < 3; ++i)
	{
		element.Position = mul(float4(input[i].Position, 1), matWVP);
		element.wNormal = mul(float4(input[i].Normal, 0), World).xyz;
		element.wPosition = mul(float4(input[i].Position, 1), World).xyz;
		element.Tex = input[i].Tex;
		element.Color = input[i].Color;
		//	ここで頂点を生成している
		output.Append(element);
	}

	//追加した頂点でプリミティブを構成し、新しいプリミティブを生成する
	output.RestartStrip();

	//	裏面で拡大して出力を作成
	for (i = 2; i >= 0; --i)
	{
		element.Position = mul(float4(input[i].Position + input[i].Normal * Paramter.x, 1), matWVP);
		element.wNormal = mul(float4(input[i].Normal, 0), World).xyz;
		element.wPosition = mul(float4(input[i].Position, 1), World).xyz;
		element.Tex = input[i].Tex;
		element.Color = float4(0, 0, 0, 1);
		//	ここで頂点を生成している
		output.Append(element);
	}
	//追加した頂点でプリミティブを構成し、新しいプリミティブを生成する
	output.RestartStrip();
}
