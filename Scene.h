#pragma once
#include "DxSystem.h"
#include <DirectXMath.h>
#include <queue>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Shader.h"
#include "ShaderEx.h"
#include "CubeMesh.h"
#include "SphereMesh.h"
#include "View.h"
#include "Light.h"
#include "Sprite.h"
#include "PlainMesh.h"
#include "ObjMesh.h"
#include "Sampler.h"
#include "Texture.h"

#include "key_input.h"

#include"ParticleSystem.h"
#include"BasicParticleSystem.h"


#include"GeometricParticles.h"
#include"GeometricParticlesEx.h"


//----------------------------------------------------
//		Scene(管理)クラス
//----------------------------------------------------

class Scene
{
private:
	std::unique_ptr<ShaderEx> shader2D ;
	std::unique_ptr<Sprite> number;

protected:

	std::unique_ptr<Scene> newScene;
	//コンスタントバッファ作成
	bool CreateConstantBuffer(ID3D11Buffer**ppCB, u_int size);
	//ビュー
	std::unique_ptr<View> view;
	XMFLOAT3 cpos;
	float cangle;
	float cdist;

	//ライト
	XMFLOAT3 LightDir = {0,0,0};



	//ビュー・ライト操作
	void Operate();



	//imGui
	virtual void imGuiUpdate();
	void imGuiSceneChanger();
	void imGuiRender();


public:

	Scene() :newScene(nullptr), view(nullptr) {
		cangle = 0.0f; 
		cdist = 15.0f; 
		cpos = XMFLOAT3(0, 4, 20); 

		Light::Init();
	}
	virtual ~Scene()
	{
		//ステートの初期化
		ID3D11DeviceContext* device_context = pSystem->GetDeviceContext();
		device_context->ClearState();
		device_context->Flush();

	}
	virtual bool Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	//アクセサ
	void setScene(std::unique_ptr<Scene> scene) { newScene = std::move(scene); }
	std::unique_ptr<Scene> getScene() { return std::move(newScene); }
};

//----------------------------------------------------
//	SceneTest0XX
//----------------------------------------------------
class SceneTest0XX : public Scene
{
private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> objModelEx = nullptr;

	std::unique_ptr<CubeMesh> bisuko = nullptr;
	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<CubeMesh>ground = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;
	std::unique_ptr<ObjMesh> slime = nullptr;
	std::unique_ptr<ObjMesh> mr_inc = nullptr;



	std::shared_ptr<Sampler> sampler_clamp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> white_texture = nullptr;
	std::shared_ptr<Texture> suika_texture = nullptr;
	std::shared_ptr<Texture> bisuko_texture = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> wood_texture = nullptr;

	std::unique_ptr<PlainMesh>plain = nullptr;
	std::shared_ptr<Texture> screen = nullptr;

	std::shared_ptr<Sprite> sprite = nullptr;


	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;

public:
	SceneTest0XX() {}
	~SceneTest0XX() {}

	bool Initialize();
	void Update();
	void Render();
	//imGui
	void imGuiUpdate();

};


//----------------------------------------------------
//	ガウスブラー
//----------------------------------------------------
class SceneTestGaussianBlur : public Scene
{
	static	constexpr	u_int	KARNEL_SIZE	= 16;
	static	constexpr	float	PI			= 3.141592f;
	static	constexpr	u_int	BUFFER_SIZE	= KARNEL_SIZE * KARNEL_SIZE;

private:
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> gaussianBlur = nullptr;

	std::shared_ptr<Sampler> sampler_cramp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;

	std::shared_ptr<Texture> blur_texture = nullptr;
	std::shared_ptr<Sprite> sprite = nullptr;

	struct ConstantBufferForPerFrameGaussianBlur
	{
		XMFLOAT4	Weight[BUFFER_SIZE];
		float		KarnelSize;
		XMFLOAT2	texcel;
		float		dummy;
	};
	ComPtr<ID3D11Buffer> gaussianBlurCB = nullptr;
	;

public:
	SceneTestGaussianBlur() {}
	~SceneTestGaussianBlur() {}

	bool Initialize();
	void Update();
	void CalcGaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma);
	void RenderGaussBlur();
	void Render();
	//imGui
	void imGuiUpdate();
};




//----------------------------------------------------
//	SceneTestBloom  
//----------------------------------------------------


class SceneTestBloom : public Scene
{
	//public:
	static	constexpr	u_int KARNEL_SIZE = 16;
	static	constexpr	float PI = 3.141592f;
	static	constexpr	u_int BUFFER_SIZE = KARNEL_SIZE* KARNEL_SIZE;
	static  constexpr   u_int SCREEN_WIDTH = 1280;
	static  constexpr   u_int SCREEN_HEIGHT = 720;
	static  constexpr   u_int BLOOM_WIDTH = 1280 / 8;
	static  constexpr   u_int BLOOM_HEIGHT = 720 / 8;


private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> noLightEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> hdrEx = nullptr;
	std::unique_ptr<ShaderEx> bokehEx = nullptr;


	std::unique_ptr<CubeMesh> bisuko = nullptr;
	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<CubeMesh>ground = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;

	std::shared_ptr<Sampler> sampler_clamp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> white_texture = nullptr;
	std::shared_ptr<Texture> suika_texture = nullptr;
	std::shared_ptr<Texture> bisuko_texture = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> wood_texture = nullptr;
	std::shared_ptr<Texture> screen_texture = nullptr;
	std::shared_ptr<Texture> bright_texture = nullptr;
	std::shared_ptr<Texture> bloom_texture = nullptr;

	std::unique_ptr<PlainMesh>plain = nullptr;
	std::unique_ptr<ObjMesh> slime = nullptr;
	std::unique_ptr<ObjMesh> mr_inc = nullptr;
	std::unique_ptr<ObjMesh> sky = nullptr;
	std::shared_ptr<Sprite> sprite = nullptr;


	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;


	struct ConstantBufferForPerFrameGaussianBlur
	{
		XMFLOAT4	Weight[BUFFER_SIZE];
		float		KarnelSize;
		XMFLOAT2	texcel;
		float		dummy;
	};
	ComPtr<ID3D11Buffer> gaussianBlurCB;

	struct ConstantBufferForBloom
	{
		float threshold;
		float dummy01;
		float dummy02;
		float dummy03;
	};
	ComPtr<ID3D11Buffer> bloomCB;



public:
	SceneTestBloom() {}
	~SceneTestBloom() {}

	bool Initialize();
	void Update();
	void Render();

	void RenderScreen();
	void RenderBloom();

	bool GaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma);

	//imGui
	void imGuiUpdate();

};

//----------------------------------------------------
//	SceneTestDOF
//----------------------------------------------------

class SceneTestDOF : public Scene
{
	static	constexpr	u_int BLUR_SAMPLE_COUNT = 15;
	static	constexpr	u_int KARNEL_SIZE = 15;
	static	constexpr	float PI = 3.141592f;
	static	constexpr	u_int BUFFER_SIZE = 256;

	static  constexpr   u_int SCREEN_WIDTH = 1280;
	static  constexpr   u_int SCREEN_HEIGHT = 720;
	static  constexpr   u_int BOKEH_WIDTH = 1280 / 8;
	static  constexpr   u_int BOKEH_HEIGHT = 720 / 8;



private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> phongDepthEx = nullptr;

	std::unique_ptr<ShaderEx> noLightEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> dofEx = nullptr;

	std::unique_ptr<ShaderEx> yiqEx = nullptr;
	std::unique_ptr<ShaderEx> hdrEx = nullptr;
	std::unique_ptr<ShaderEx> bokehEx = nullptr;
	std::unique_ptr<ShaderEx> bokeh2Ex = nullptr;


	std::unique_ptr<CubeMesh> bisuko = nullptr;
	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<CubeMesh>ground = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;

	std::shared_ptr<Sampler> sampler_clamp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> white_texture = nullptr;
	std::shared_ptr<Texture> suika_texture = nullptr;
	std::shared_ptr<Texture> bisuko_texture = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> wood_texture = nullptr;
	std::shared_ptr<Texture> screen_texture = nullptr;
	std::shared_ptr<Texture> bokeh_texture = nullptr;
	std::shared_ptr<Texture> depth_texture = nullptr;

	std::unique_ptr<PlainMesh>plain = nullptr;
	std::unique_ptr<ObjMesh> slime = nullptr;
	std::unique_ptr<ObjMesh> mr_inc = nullptr;
	std::unique_ptr<ObjMesh> sky = nullptr;
	std::shared_ptr<Sprite> sprite = nullptr;


	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;


	struct ConstantBufferForPerFrameGaussianBlur
	{
		XMFLOAT4	Weight[BUFFER_SIZE];
		float		KarnelSize;
		XMFLOAT2	texcel;
		float		dummy;
	};
	ComPtr<ID3D11Buffer> gaussianBlurCB;

	struct ConstantBufferForDOF
	{
		float dist;
		float focus;
		float dummy01;
		float dummy02;
	};
	ComPtr<ID3D11Buffer> depth_of_fieldCB;



public:
	SceneTestDOF() {}
	~SceneTestDOF() {}

	bool Initialize();
	void Update();
	void Render();

	void RenderScreen();
	void RenderBokeh();

	bool GaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma);

	//imGui
	void imGuiUpdate();

};







//----------------------------------------------------
//	SceneTestShadow
//----------------------------------------------------
class SceneTestShadow : public Scene
{
	static	constexpr	u_int	SHADOWBUFFER = 1024;
	static	constexpr	u_int	BUFFER_SIZE = 256;
	float shadowColor[3] = { 0.5f, 0.5f, 0.5f };
	float bias = 0.01f;


private:
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;

	std::shared_ptr<Sampler> sampler_cramp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;

	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;
	std::unique_ptr<PlainMesh> plain = nullptr;
	std::unique_ptr<ObjMesh> objmesh = nullptr;

	std::shared_ptr<Texture> shadowmap = nullptr;
	std::shared_ptr<Texture> shadowmap_depth = nullptr;
	std::unique_ptr<ShaderEx> shadowShader = nullptr;
	std::unique_ptr<ShaderEx> drawShadow = nullptr;

	XMFLOAT4X4 LightViewProjection = {};

	std::shared_ptr<Sampler> sampler_shadow = nullptr;

	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer=nullptr;


	struct ConstantBufferForPerFrameShadowmap
	{
		XMFLOAT4X4	LightViewProjection;
		XMFLOAT4	ShadowParameter;	//	xyz カラー、w バイアス
	};
	ComPtr<ID3D11Buffer> shadowmapCB = nullptr;


public:
	SceneTestShadow() {}
	~SceneTestShadow() {}

	bool Initialize();
	void Update();
	void RenderShadowmap();
	void Render();
	//imGui
	void imGuiUpdate();

	float	ShadowErea = 50;


};



//----------------------------------------------------
//	SceneTestVSM
//----------------------------------------------------
class SceneTestVSM : public Scene
{
	static	constexpr	u_int	SHADOWBUFFER		= 512;
	static	constexpr	u_int	BLUR_SAMPLE_COUNT	= 15;
	static	constexpr	u_int	KARNEL_SIZE			= 15;
	static	constexpr	float	PI					= 3.141592f;
	static	constexpr	u_int	BUFFER_SIZE			= 256;

private:
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;

	std::shared_ptr<Sampler> sampler_cramp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;

	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;
	std::unique_ptr<PlainMesh> plain = nullptr;
	std::unique_ptr<ObjMesh> objmesh = nullptr;

	std::shared_ptr<Texture> shadowmap = nullptr;
	std::shared_ptr<Texture> shadowmapVSM = nullptr;
	std::shared_ptr<Texture> shadowmap_depth = nullptr;
	std::unique_ptr<ShaderEx> varianceShadowmap = nullptr;
	XMFLOAT4X4 LightViewProjection{};

	std::shared_ptr<Sprite> gaussianSprite = nullptr;
	std::unique_ptr<ShaderEx> gaussianBlur = nullptr;

	std::shared_ptr<Sampler> sampler_shadow = nullptr;
	std::unique_ptr<ShaderEx> phongVSM = nullptr;

	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer=nullptr;

	struct ConstantBufferForPerFrameGaussianBlur
	{
		XMFLOAT4	Weight[BUFFER_SIZE];
		float		KarnelSize;
		XMFLOAT2	texcel;
		float		dummy;
	};
	ComPtr<ID3D11Buffer> gaussianBlurCB = nullptr;

	struct ConstantBufferForPerFrameShadowmap
	{
		XMFLOAT4X4	LightViewProjection;
		XMFLOAT4	ShadowParameter;	//	xyz カラー、w バイアス
	};
	ComPtr<ID3D11Buffer> shadowmapCB = nullptr;


public:
	SceneTestVSM() {}
	~SceneTestVSM() {}

	bool Initialize();
	void Update();
	void CalcGaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma);
	void RenderShadow();
	void Render();
	//imGui
	void imGuiUpdate();

};
//----------------------------------------------------
//	フラットシェーディング&ファーシェーディング表現
//----------------------------------------------------
class SceneTestFlatAndFur : public Scene
{
private:
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;

	std::shared_ptr<Sampler> sampler_cramp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;

	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;
	std::unique_ptr<PlainMesh> plain = nullptr;
	std::unique_ptr<ObjMesh> objmesh = nullptr;
	std::unique_ptr<ObjMesh> slime = nullptr;
	std::unique_ptr<ShaderEx> fur_shader = nullptr;
	std::shared_ptr<Texture> fur_texture = nullptr;

	std::unique_ptr<ShaderEx> flat = nullptr;

	float density = 1.0f;
	float distance = 0.02f;



	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//	ライトの色
		XMFLOAT4	LightDir;		//	ライトの方向
		XMFLOAT4	AmbientColor;	//	環境光
		XMFLOAT4	EyePos;			//	カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;

	struct ConstantBufferForFur
	{
		float	Density;
		float	Distance;
		float	dummy1;
		float	dummy2;
		XMFLOAT4	Force;
	};
	ComPtr<ID3D11Buffer>fur_constantbuffer = nullptr;


public:
	SceneTestFlatAndFur() {}
	~SceneTestFlatAndFur() {}

	bool Initialize();
	void Update();
	void Render();
	//imGui
	void imGuiUpdate();

};

//----------------------------------------------------
//	トゥーンレンダリング&破壊表現
//----------------------------------------------------
class SceneTestToonAndDestruction : public Scene
{
private:
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;

	std::shared_ptr<Sampler> sampler_cramp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;

	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;
	std::unique_ptr<PlainMesh> plain = nullptr;
	std::unique_ptr<ObjMesh> objmesh = nullptr;
	std::unique_ptr<ObjMesh> slime = nullptr;

	std::unique_ptr<ShaderEx> toon_rendering = nullptr;
	std::shared_ptr<Texture> toon_diffuse_texture = nullptr;

	std::unique_ptr<ShaderEx> destraction = nullptr;

	float	outLineSize = 0.05f;
	float	destractionRate = 0.0f;
	float	scalingFactor = 1.0f;
	float	rotationFactor = DirectX::XM_PI * 3.0f;
	float	translationFactor = 1.0f;




	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//	ライトの色
		XMFLOAT4	LightDir;		//	ライトの方向
		XMFLOAT4	AmbientColor;	//	環境光
		XMFLOAT4	EyePos;			//	カメラ

	};
	ComPtr<ID3D11Buffer> ConstantBuffer;

	struct ConstantBufferForToon
	{
		XMFLOAT4	Paramter;	//	輪郭線の場合( x = 輪郭線の太さ, yxw = 空き )
	};
	ComPtr<ID3D11Buffer> toon_constantbuffer;

	struct ConstantBufferForDestruction
	{	
		XMFLOAT4	Paramter;	//	破壊表現の場合
								//	x = 破壊レート
								//	y = 拡縮係数
								//	z = 回転係数
								//	w = 法線方向への移動係数
	};
	ComPtr<ID3D11Buffer> destruction_constantbuffer;




public:
	SceneTestToonAndDestruction() {}
	~SceneTestToonAndDestruction() {}

	bool Initialize();
	void Update();
	void Render();
	//imGui
	void imGuiUpdate();

};




//----------------------------------------------------
//	SceneTestBasicPointSprite
//----------------------------------------------------

class SceneTestBasicPointSprite : public Scene
{
private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;

	std::unique_ptr<CubeMesh> bisuko = nullptr;
	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<CubeMesh>ground = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;

	std::shared_ptr<Sampler> sampler_cramp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> white_texture = nullptr;
	std::shared_ptr<Texture> suika_texture = nullptr;
	std::shared_ptr<Texture> bisuko_texture = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> wood_texture = nullptr;

	std::unique_ptr<PlainMesh>plain = nullptr;
	std::shared_ptr<Texture> screen = nullptr;

	std::shared_ptr<Texture> particle_texture = nullptr;
	std::shared_ptr<Texture> particle_texture2 = nullptr;
	std::shared_ptr<cParticleSystem> particles_alpha = nullptr;
	std::shared_ptr<cParticleSystem> particles_add = nullptr;
	std::shared_ptr<cParticleSystem> particles_sub = nullptr;
	std::unique_ptr<ShaderEx> basicPointSprite = nullptr;

	std::shared_ptr<cBasicParticleSystem> basic_particles = nullptr;
	std::unique_ptr<ShaderEx> PointSprite = nullptr;
	ComPtr<ID3D11Buffer> ConstantBuffer;


	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};

public:
	SceneTestBasicPointSprite() {}
	~SceneTestBasicPointSprite() {}

	bool Initialize();
	void Update();
	void Render();
	//imGui
	void imGuiUpdate();

};


//----------------------------------------------------
//	SceneTestPointSprite
//----------------------------------------------------

class SceneTestPointSprite : public Scene
{
private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;

	std::unique_ptr<CubeMesh> bisuko = nullptr;
	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<CubeMesh>ground = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;

	std::shared_ptr<Sampler> sampler_cramp = nullptr;
	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Texture> white_texture = nullptr;
	std::shared_ptr<Texture> suika_texture = nullptr;
	std::shared_ptr<Texture> bisuko_texture = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> wood_texture = nullptr;

	std::unique_ptr<PlainMesh>plain = nullptr;
	std::shared_ptr<Texture> screen = nullptr;

	std::shared_ptr<Texture> particle_texture = nullptr;
	std::shared_ptr<Texture> bomb_texture = nullptr;

	std::shared_ptr<cParticleSystem> particles_bumbs = nullptr;
	std::shared_ptr<cParticleSystem> particles_alpha = nullptr;
	std::shared_ptr<cParticleSystem> particles_add = nullptr;
	std::shared_ptr<cParticleSystem> particles_sub = nullptr;

	std::unique_ptr<ShaderEx> pointSprite = nullptr;



	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;

public:
	SceneTestPointSprite() {}
	~SceneTestPointSprite() {}

	bool Initialize();
	void Update();
	void Render();
	//imGui
	void imGuiUpdate();

};

//----------------------------------------------------
//	SceneTestMRT
//----------------------------------------------------

class SceneTestMRT : public Scene
{
private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> deferredLightEx = nullptr;

	std::unique_ptr<CubeMesh> bisuko = nullptr;
	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<CubeMesh>ground = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;

	std::shared_ptr<Sampler> sampler = nullptr;
	std::shared_ptr<Texture> white = nullptr;
	std::shared_ptr<Texture> yellow = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::unique_ptr<PlainMesh>plain = nullptr;
	std::shared_ptr<Texture> depth_texture = nullptr;
	std::shared_ptr<Texture> earth_texture = nullptr;

	std::shared_ptr<Texture> color_texture = nullptr;
	std::shared_ptr<Texture> normal_texture = nullptr;
	std::shared_ptr<Texture> position_texture = nullptr;

	std::shared_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<ShaderEx> toGbuffer = nullptr;

	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;

public:
	SceneTestMRT() {}
	~SceneTestMRT() {}

	bool Initialize();
	void Update();
	void Render();
	void RenderTexture();

	//imGui
	void imGuiUpdate();


};

//----------------------------------------------------
//	SceneTestMRT2 (normalMapの利用)
//----------------------------------------------------

class SceneTestMRT2 : public Scene
{
private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> deferredLightEx = nullptr;

	std::unique_ptr<CubeMesh> bisuko = nullptr;
	std::unique_ptr<CubeMesh> cube = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<CubeMesh>ground = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;



	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Sampler> sampler_clamp = nullptr;

	std::shared_ptr<Texture> white = nullptr;
	std::shared_ptr<Texture> yellow = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> test_normal_texture = nullptr;
	std::shared_ptr<Texture> bisuko_normal_texture = nullptr;
	std::shared_ptr<Texture> bisuko_specular_texture = nullptr;
	std::shared_ptr<Texture> metal_specular_texture = nullptr;
	std::shared_ptr<Texture> scorpion_specular_texture = nullptr;

	std::unique_ptr<PlainMesh>plain = nullptr;
	std::shared_ptr<Texture> color_texture = nullptr;
	std::shared_ptr<Texture> normal_texture = nullptr;
	std::shared_ptr<Texture> position_texture = nullptr;
	std::shared_ptr<Texture> specular_reflection = nullptr;
	std::shared_ptr<Texture> earth_texture = nullptr;
	std::shared_ptr<Texture> earth_normal_texture = nullptr;


	std::shared_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<ShaderEx> toGbuffer = nullptr;
	std::unique_ptr<ShaderEx> toGbufferNormal = nullptr;
	std::unique_ptr<ShaderEx> toGbufferNormalSpecular = nullptr;


	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;

public:
	SceneTestMRT2() {}
	~SceneTestMRT2() {}

	bool Initialize();
	void Update();
	void Render();
	void RenderTexture();

	//imGui
	void imGuiUpdate();


};

//----------------------------------------------------
//	SceneTestDeferred
//----------------------------------------------------

class SceneTestDeferred : public Scene
{
private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> deferredLightEx = nullptr;
	std::unique_ptr<ShaderEx> deferredNoLight = nullptr;

	std::unique_ptr<ObjMesh> slime = nullptr;
	std::unique_ptr<ObjMesh> mr_inc = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;
	std::unique_ptr<ObjMesh> sky = nullptr;

	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Sampler> sampler_clamp = nullptr;
	std::shared_ptr<Sampler> sampler_border = nullptr;

	std::shared_ptr<Texture> white = nullptr;
	std::shared_ptr<Texture> yellow = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> test_normal_texture = nullptr;
	std::shared_ptr<Texture> field_texture = nullptr;
	std::shared_ptr<Texture> field_normal_texture = nullptr;

	std::unique_ptr<PlainMesh>field = nullptr;
	std::shared_ptr<Texture> color_texture = nullptr;
	std::shared_ptr<Texture> background_texture = nullptr;
	std::shared_ptr<Texture> normal_texture = nullptr;
	std::shared_ptr<Texture> position_texture = nullptr;
	std::shared_ptr<Texture> shadow_texture = nullptr;
	std::shared_ptr<Texture> earth_texture = nullptr;
	std::shared_ptr<Texture> earth_normal_texture = nullptr;


	std::shared_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<ShaderEx> toGbuffer = nullptr;
	std::unique_ptr<ShaderEx> toGbufferNormal = nullptr;
	std::unique_ptr<ShaderEx> toGbufferShadow = nullptr;

	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ位置
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;


public:
	SceneTestDeferred() {}
	~SceneTestDeferred() {}

	bool Initialize();
	void Update();
	void Render();
	void RenderTexture();
	void RenderTexture2();


	//imGui
	void imGuiUpdate();


};

//----------------------------------------------------
//	SceneTestDeferredAll
//----------------------------------------------------

class SceneTestDeferredAll : public Scene
{
private:
	std::unique_ptr<Shader> phong = nullptr;
	std::unique_ptr<ShaderEx> phongEx = nullptr;
	std::unique_ptr<ShaderEx> spriteEx = nullptr;
	std::unique_ptr<ShaderEx> deferredLightEx = nullptr;
	std::unique_ptr<ShaderEx> deferredNoLight = nullptr;

	std::unique_ptr<ObjMesh> slime = nullptr;
	std::unique_ptr<ObjMesh> mr_inc = nullptr;
	std::unique_ptr<CubeMesh> wood = nullptr;
	std::unique_ptr<SphereMesh> sphere = nullptr;
	std::unique_ptr<ObjMesh> sky = nullptr;

	std::shared_ptr<Sampler> sampler_wrap = nullptr;
	std::shared_ptr<Sampler> sampler_clamp = nullptr;
	std::shared_ptr<Sampler> sampler_border = nullptr;

	std::shared_ptr<Texture> white = nullptr;
	std::shared_ptr<Texture> yellow = nullptr;
	std::shared_ptr<Texture> blue_back = nullptr;
	std::shared_ptr<Texture> test_texture = nullptr;
	std::shared_ptr<Texture> test_normal_texture = nullptr;
	std::shared_ptr<Texture> field_texture = nullptr;
	std::shared_ptr<Texture> field_normal_texture = nullptr;

	std::unique_ptr<PlainMesh>field = nullptr;
	std::shared_ptr<Texture> color_texture = nullptr;
	std::shared_ptr<Texture> background_texture = nullptr;
	std::shared_ptr<Texture> normal_texture = nullptr;
	std::shared_ptr<Texture> position_texture = nullptr;
	std::shared_ptr<Texture> shadow_texture = nullptr;
	std::shared_ptr<Texture> earth_texture = nullptr;
	std::shared_ptr<Texture> earth_normal_texture = nullptr;

	std::shared_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<ShaderEx> toGbuffer = nullptr;
	std::unique_ptr<ShaderEx> toGbufferNormal = nullptr;

	std::unique_ptr<ShaderEx> toGbufferShadow = nullptr;


	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		XMFLOAT4	LightColor;		//ライトの色
		XMFLOAT4	LightDir;		//ライトの方向
		XMFLOAT4	AmbientColor;	//環境光
		XMFLOAT4	EyePos;			//カメラ位置
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;

	//ポイントライト構造体
	struct CBforPointLight
	{
		POINTLIGHT  PointLight[Light::POINTMAX];
	};
	ComPtr<ID3D11Buffer> ConstantBufferPointLight;

	//スポットライト構造体
	struct CBforSpotLight
	{
		SPOTLIGHT  SpotLight[Light::SPOTMAX];
	};
	ComPtr<ID3D11Buffer> ConstantBufferSpotLight;

	//影構造体
	struct CBforShadowMap
	{
		XMFLOAT4X4  ShadowViewProjection;
		XMFLOAT4	ShadowColor;
	};
	ComPtr<ID3D11Buffer> ConstantBufferShadow;

	XMFLOAT4X4 ShadowViewProjection = {};





public:
	SceneTestDeferredAll() {}
	~SceneTestDeferredAll() {}

	bool Initialize();
	void Update();
	void Render();
	void RenderTexture();
	void RenderTexture2();

	void RenderShadow();

	//imGui
	void imGuiUpdate();


};



//----------------------------------------------------
//		ScenManager(管理)クラス
//----------------------------------------------------
//	起動時実行シーン指定
//using	SceneTestFirst	= SceneTestVSM;
//using	SceneTestFirst = SceneTestMotionBlur;
using	SceneTestFirst = SceneTest0XX;



class SceneManager
{
private:
	std::unique_ptr<Scene> scene;
public:
	SceneManager() :scene(nullptr){}
	~SceneManager() {}

	void Update();
	void Render();
	void ChangeScene(std::unique_ptr<Scene> newScene);
};
