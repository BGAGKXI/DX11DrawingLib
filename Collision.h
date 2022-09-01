#pragma once

extern int map_data[15][20];


class Collision
{
public:
	static void CheckWallDown(DirectX::XMFLOAT2*p, DirectX::XMFLOAT2*v)
	{
		int l = (int)(p->x - 14) / 32;
		int r = (int)(p->x + 14) / 32;
		int d = (int)(p->y + 16) / 32;

		if (map_data[d][l] >= 64 || map_data[d][r] >= 64) {
			p->y = (float)(d * 32 - 16);
			v->y = -v->y*0.6f;
		}
	}

	static void CheckWallUp(DirectX::XMFLOAT2*p, DirectX::XMFLOAT2*v)
	{
		int l = (int)(p->x - 14) / 32;
		int r = (int)(p->x + 14) / 32;
		int u = (int)(p->y - 16) / 32;


		if (map_data[u][l] >= 64 || map_data[u][r] >= 64) {
			p->y = (float)((u + 1) * 32 + 16);
			v->y = -v->y*0.6f;
		}
	}

	static void CheckWallRight(DirectX::XMFLOAT2*p, DirectX::XMFLOAT2*v)
	{
		int r = (int)(p->x + 16) / 32;
		int u = (int)(p->y - 14) / 32;
		int d = (int)(p->y + 14) / 32;
		if (map_data[d][r] >= 64 || map_data[u][r] >= 64) {
			p->x = (float)(r * 32 - 16);
			v->x = -v->x*0.6f;

		}
	}
	static void CheckWallLeft(DirectX::XMFLOAT2*p, DirectX::XMFLOAT2*v)
	{
		int l = (int)(p->x - 16) / 32;
		int u = (int)(p->y - 14) / 32;
		int d = (int)(p->y + 14) / 32;
		if (map_data[d][l] >= 64 || map_data[u][l] >= 64) {
			p->x = (float)((l + 1) * 32 + 16);
			v->x = -v->x*0.6f;
		}
	}

};