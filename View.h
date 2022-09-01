#pragma once
#include <DirectXMath.h>
class View
{
private:
protected:
	DirectX::XMFLOAT3  Pos,Target,Up;
	DirectX::XMFLOAT4X4 ViewMatrix;
	DirectX::XMFLOAT4X4 ProjectionMatrix;
	FLOAT Fov;
	FLOAT Aspect;
	FLOAT Near;
	FLOAT Far;
	FLOAT Width;
	FLOAT Height;
	bool bView;
	
public:
	View();
	virtual ~View();
	void Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);
	void SetProjection(FLOAT fov, FLOAT aspect, FLOAT min, FLOAT max);
	void SetOrtho(FLOAT width, FLOAT height, FLOAT min, FLOAT max);

	void Activate();
	DirectX::XMMATRIX GetView() { return XMLoadFloat4x4(&ViewMatrix); }
	DirectX::XMMATRIX GetProjection() { return XMLoadFloat4x4(&ProjectionMatrix); }
	DirectX::XMFLOAT3 GetPos() { return Pos; }

};