#include "main.h"
#include "Renderer\Windows\AgApplication.h"

//OGLContext OGLContext; // Our OpenGL Context class
AfterGlow::AgApplication* g_pApplication;

bool running = true; // Whether or not the application is currently running

HINSTANCE hInstance; // The HINSTANCE of this application
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Standard window callback

/**
	WndProc is a standard method used in Win32 programming for handling Window messages. Here we
	handle our window resizing and tell our OGLContext the new window size.
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_SIZE: // If our window is resizing
		{
			g_pApplication->getRenderer().reshapeWindow(LOWORD(lParam), HIWORD(lParam)); // Send the new window size to our OGLContext
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

/**
	createWindow is going to create our window using Windows API calls. It is then going to
	create our OpenGL context on the window and then show our window, making it visible.
*/
bool createWindow(LPCWSTR title, int width, int height) {
	WNDCLASS windowClass;
	HWND hWnd;
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	hInstance = GetModuleHandle(NULL);

	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = (WNDPROC) WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = title;

	if (!RegisterClass(&windowClass)) {
		return false;
	}

	hWnd = CreateWindowEx(dwExStyle, title, title, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, width, height, NULL, NULL, hInstance, NULL);

	//OGLContext.create30Context(hWnd); // Create our OpenGL context on the given window we just created
	g_pApplication->getRenderer().createContext(hWnd);

	ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

	return true;
}

/**
	WinMain is the main entry point for Windows based applications as opposed to 'main' for console
	applications. Here we will make the calls to create our window, setup our scene and then
	perform our 'infinite' loop which processes messages and renders.
*/
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR    lpCmdLine,
                     int       nCmdShow) {
	MSG msg;

	/**
		The following 6 lines of code do conversion between char arrays and LPCWSTR variables
		which are used in the Windows API.
	*/
	char *orig = "OpenGL 3 Project"; // Our windows title
	size_t origsize = strlen(orig) + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);

	createWindow(wcstring, 1366, 768); // Create our OpenGL window

	g_pApplication->getRenderer().setupScene(); // Setup our OpenGL scene
	/**
		This is our main loop, it continues for as long as running is true
	*/
	while (running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // If we have a message to process, process it
			if (msg.message == WM_QUIT) {
				running = false; // Set running to false if we have a message to quit
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else { // If we don't have a message to process
			g_pApplication->Run(); // Render our scene (which also handles swapping of buffers)
		}
	}

	return (int) msg.wParam;
}