// include the basic windows header file
#include <windows.h>
#include <windowsx.h>

#include <Engine.hpp>
#include <sstream>
#include <TimeSystem.hpp>
#include <LibraryLoader.hpp>

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (HWND hwnd, RECT rect);
using CreateGameFunc = Poly::IGame* (void);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	//Open console, good for now
	AllocConsole();
	FILE* s1, *s2, *s3;
	freopen_s(&s1, "CONIN$", "r", stdin);
	freopen_s(&s2, "CONOUT$", "w", stdout);
	freopen_s(&s3, "CONOUT$", "w", stderr);

	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	// RECT viewportRect = { 0, 0, 1413,  600 };	// Cinematic medium
	// RECT viewportRect = { 0, 0, 1920,  804 };	// Cinematic big
	RECT viewportRect = { 0, 0,  960,  540 };	// half HD
	// RECT viewportRect = { 0, 0, 1920, 1080 };	// full HD
	RECT windowRect = viewportRect;    // set the size, but not the position
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

																  // create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		"WindowClass1",    // name of the window class
		" Standalone - OpenGL",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		0,    // x-position of the window
		0,    // y-position of the window
		windowRect.right - windowRect.left,    // width of the window
		windowRect.bottom - windowRect.top,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

				  // display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// enter the main loop:

	// this struct holds Windows event messages
	MSG msg;

	auto loadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libRenderingDevice", "PolyCreateRenderingDevice");
	if (!loadRenderingDevice.FunctionValid()) { return 1; }
	auto loadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
	if (!loadGame.FunctionValid()) { return 1; }

	{
		Poly::Engine Engine;

		std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(loadGame());
		std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(loadRenderingDevice(hWnd, viewportRect));

		Engine.Init(std::move(game), std::move(device));
		Poly::gConsole.LogDebug("Engine loaded successfully");

		// wait for the next message in the queue, store the result in 'msg'
		bool quitRequested = false;
		while (!quitRequested)
		{
			// Check to see if any messages are waiting in the queue
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
			{
				// translate keystroke messages into the right format
				TranslateMessage(&msg);

				// send the message to the WindowProc function
				DispatchMessage(&msg);

				// check to see if it's time to quit
				if (msg.message == WM_QUIT)
					quitRequested = true;
			}

			quitRequested = quitRequested || Engine.IsQuitRequested();
			// Run game code here
			Engine.Update();
		}
	}
	// return this part of the WM_QUIT message to Windows
	return static_cast<int>(msg.wParam);
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{

	case WM_SIZE:
	{
		Poly::ScreenSize screenSize;
		screenSize.Width = GET_X_LPARAM(lParam);
		screenSize.Height = GET_Y_LPARAM(lParam);
		if(Poly::gEngine)
			Poly::gEngine->ResizeScreen(screenSize);
		return 0;
	}

		// input
	// Use separate case's for mouse buttons because otherwise mouse buttons aren't checked
	case WM_LBUTTONDOWN:
		Poly::gEngine->KeyDown(Poly::eKey::MLBUTTON);
		return 0;
	case WM_LBUTTONUP:
		Poly::gEngine->KeyUp(Poly::eKey::MLBUTTON);
		return 0;

	case WM_RBUTTONDOWN:
		Poly::gEngine->KeyDown(Poly::eKey::MRBUTTON);
		return 0;
	case WM_RBUTTONUP:
		Poly::gEngine->KeyUp(Poly::eKey::MRBUTTON);
		return 0;

	case WM_MBUTTONDOWN:
		Poly::gEngine->KeyDown(Poly::eKey::MMBUTTON);
		return 0;
	case WM_MBUTTONUP:
		Poly::gEngine->KeyUp(Poly::eKey::MMBUTTON);
		return 0;

	case WM_XBUTTONDOWN:
		if(GET_Y_LPARAM(wParam) == XBUTTON1) Poly::gEngine->KeyDown(Poly::eKey::MBUTTON1);
		else Poly::gEngine->KeyDown(Poly::eKey::MBUTTON2);
		return 0;
	case WM_XBUTTONUP:
		if(GET_Y_LPARAM(wParam) == XBUTTON1) Poly::gEngine->KeyUp(Poly::eKey::MBUTTON1);
		else Poly::gEngine->KeyUp(Poly::eKey::MBUTTON2);
		return 0;

	case WM_KEYDOWN:
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>((unsigned int)wParam));
		return 0;
	case WM_KEYUP:
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>((unsigned int)wParam));
		return 0;

	case WM_MOUSEMOVE:
	{
		POINT pointPos;
		GetCursorPos(&pointPos);
		ScreenToClient(hWnd, &pointPos);
		Poly::gEngine->UpdateMousePos(Poly::Vector(static_cast<float>(pointPos.x), static_cast<float>(pointPos.y), 0));
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int xPos = GET_WHEEL_DELTA_WPARAM(wParam);
		Poly::gEngine->UpdateWheelPos(Poly::Vector(static_cast<float>(xPos), 0, 0));
		return 0;
	}
	// end of input

		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}