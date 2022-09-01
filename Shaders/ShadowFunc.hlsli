
//--------------------------------------------
//	グローバル変数
//--------------------------------------------

static const float Bias = 0.0008;

cbuffer CBPerFrame : register(b5)
{
	row_major float4x4 ShadowViewProjection;
	float4 ShadowColor;
};


//--------------------------------------------
//	シャドーマップ関数
//--------------------------------------------


//ワールド座標をライト空間座標に変換
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
	// 正規化デバイス座標系
	float4 wvpPos = float4(wPos, 1.0f);

	//wvpPos = mul(vp, wvpPos);
	wvpPos = mul(wvpPos,vp);
	wvpPos /= wvpPos.w;
	// テクスチャ座標系
	wvpPos.y = -wvpPos.y;
	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
	return wvpPos.xyz;
}

//シャドーマップからシャドー空間座標に変換とZ値比較
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias) {
	// シャドウマップから深度を取り出す
	float d = st.Sample(ss, Tex.xy).r;
	// シャドウマップの深度値と現実の深度の比較
	Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
	return Scolor;
}

