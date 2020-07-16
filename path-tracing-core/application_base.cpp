#include "application_base.hpp"
#include "imgui_impl_win32.hpp"

#include <Windows.h>

#include <chrono>

using time_point = std::chrono::high_resolution_clock;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT DefaultWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
		
	switch (message)
	{
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

path_tracing::core::application_base::application_base(const std::string& name, int width, int height) :
	mName(name), mWidth(width), mHeight(height), mHandle(nullptr), mExisted(false)
{
}

path_tracing::core::application_base::~application_base()
{
	mRenderer->release();
	
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void path_tracing::core::application_base::run_loop()
{
	auto current = time_point::now();

	while (mExisted == true) {
		MSG message;

		message.hwnd = static_cast<HWND>(mHandle);

		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) mExisted = false;
		}

		if (mExisted == false) break;

		auto duration = std::chrono::duration_cast<
			std::chrono::duration<float>>(time_point::now() - current);

		ImGui_ImplWin32_NewFrame();
		
		mRenderer->render(mCamera);
		
		current = time_point::now();
	}
}

void path_tracing::core::application_base::initialize_windows()
{
	const auto hInstance = GetModuleHandle(nullptr);
	const auto class_name = mName;

	WNDCLASS appInfo;

	appInfo.style = CS_DBLCLKS;
	appInfo.lpfnWndProc = DefaultWindowProc;
	appInfo.cbClsExtra = 0;
	appInfo.cbWndExtra = 0;
	appInfo.hInstance = hInstance;
	appInfo.hIcon = static_cast<HICON>(LoadImage(nullptr, "", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	appInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
	appInfo.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	appInfo.lpszMenuName = nullptr;
	appInfo.lpszClassName = &class_name[0];

	RegisterClass(&appInfo);

	RECT rect;

	rect.top = 0;
	rect.left = 0;
	rect.right = static_cast<UINT>(mWidth);
	rect.bottom = static_cast<UINT>(mHeight);

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	mHandle = CreateWindow(&class_name[0], &class_name[0], WS_OVERLAPPEDWINDOW ^
		WS_SIZEBOX ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	mExisted = true;

	ShowWindow(static_cast<HWND>(mHandle), SW_SHOW);

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(mHandle);
}

void path_tracing::core::application_base::initialize()
{
	initialize_windows();
	initialize_graphics();
}

void path_tracing::core::application_base::load(const std::shared_ptr<camera>& camera, const std::shared_ptr<scene>& scene)
{
	mCamera = camera;
	mScene = scene;

	mRenderer->build(scene);
}
