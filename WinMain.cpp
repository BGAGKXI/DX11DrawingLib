#include <Windows.h>
#include <tchar.h>
#include "DxSystem.h"

#include "Scene.h"
#include <sstream>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"




//float	elapsed_time = 0; // 経過時間
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;


	switch (message) {
	case WM_CLOSE:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE: PostMessage(hWnd, WM_CLOSE, 0, 0); return 0;
		}
		break;
	}

	return(DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// ウィンドウ生成
	int width = 1280;
	int height = 720;
	WNDCLASS wcex;
	TCHAR szWindowClass[] = TEXT("Shader_Sample");
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	RECT rect;
	DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	::SetRect(&rect, 0, 0, width, height);
	::AdjustWindowRectEx(&rect, style, FALSE, 0);//
	//width = rect.right - rect.left;
	//height = rect.bottom - rect.top;
	int window_width = rect.right - rect.left;
	int window_height = rect.bottom - rect.top;




	HWND hWnd;
	hWnd = CreateWindow(szWindowClass,
		szWindowClass,
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, window_width, window_height,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

														   // Setup Dear ImGui style
	ImGui::StyleColorsClassic();
	//フォントの設定を行う。IPAゴシック 12ポイント
	io.Fonts->AddFontFromFileTTF("Assets\\imGuiFont\\ipag.ttf", 12.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//io.Fonts->AddFontFromFileTTF("resource\\ipag.ttf", 12.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());


	// デバイス初期化
	if (FAILED(pSystem->Initialize(hWnd,(int)width,(int)height)))
	{
		return 0;
	}

	//imGui初期化
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pSystem->GetDevice(),pSystem->GetDeviceContext() );


	SceneManager* scenemanager = new SceneManager();
	scenemanager->ChangeScene(std::make_unique<SceneTestFirst>());

	//メインループ
	MSG hMsg = { 0 };
	float Interval = 1.0f;
	ULONGLONG before = GetTickCount64();
	int fps = 0;

	while (hMsg.message != WM_QUIT) {
		if (PeekMessage(&hMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
		else
		{
			pSystem->elapsed_time =(GetTickCount64() - before) * 0.001f;

			before = GetTickCount64();
			float mspf = 1000.0f / fps;

			Interval -= pSystem->elapsed_time;
			fps++;
			if (Interval < 0) {
				std::ostringstream outs;
				outs.precision(6);
				outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
				SetWindowTextA(hWnd, outs.str().c_str());
				Interval += 1.0f;
				fps = 0;

			}


			// 更新・描画
			scenemanager->Update();
			scenemanager->Render();
		}
	}
	//imGui終了
	ImGui_ImplDX11_Shutdown();
	SAFE_DELETE(scenemanager);
	return 0;
}
