#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct CIRCLESHADOW {
	float index;
	float range;//光の届く範囲
	float type; //有効か無効か
	float inner;
	XMFLOAT4 pos;
};

class CircleShadow
{
public:
	static const int CIRCLEMAX = 32;
	static CIRCLESHADOW circleShadow[CIRCLEMAX];
	static void SetCircleShadow(int index, XMFLOAT3 pos, float inner, float range);
};
