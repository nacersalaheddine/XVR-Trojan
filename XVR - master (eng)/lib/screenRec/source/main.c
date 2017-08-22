#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int gs_width = 800;
int gs_height = 600;
int gs_port = 2667;
char* gs_ip;

//@NAPRAVI dovurshi

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;

	switch (msg)
	{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void main_loadArguments(char* str)
{
	//@NAPRAVI
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	main_loadArguments()
	WNDCLASSEX wc;
	LPCTSTR MainWndClass = TEXT("Screen Capture");
	HWND hWnd;
	HACCEL hAccelerators;
	HMENU hSysMenu;
	MSG msg;

	wc.cbSize        = sizeof(wc);
	wc.style         = 0;
	wc.lpfnWndProc   = &MainWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = (HICON) LoadImage(hInstance, NULL, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
	wc.hCursor       = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = MainWndClass;
	wc.hIconSm       = (HICON) LoadImage(hInstance, NULL, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);

  // Register our window classes, or error.
	if(!RegisterClassEx(&wc))
	{
    		MessageBox(NULL, TEXT("Error registering window class."), TEXT("Error"), MB_ICONERROR | MB_OK);
		
		    return 0;
	}

  // Create instance of main window.
	hWnd = CreateWindowEx(0, MainWndClass, MainWndClass, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX), CW_USEDEFAULT, CW_USEDEFAULT, gs_height, gs_width, NULL, NULL, hInstance, NULL);

  // Error if window creation failed.
	if(!hWnd)
	{
		MessageBox(NULL, TEXT("Error creating main window."), TEXT("Error"), MB_ICONERROR | MB_OK);
		
		return 0;
	}

	// Show window and force a paint.
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

  // Main message loop.
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if(!TranslateAccelerator(msg.hwnd, hAccelerators, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


/*
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

HDC backBuff;
HDC frontBuff;

int main(int argc, char* argv[])
{
	HWND myconsole = GetConsoleWindow();
	backBuff = GetDC(myconsole);
	frontBuff = CreateCompatibleDC(backBuff);
	
	HBITMAP backBM = CreateCompatibleBitmap(backBuff, 800, 600);
	HBITMAP oldBackBM = (HBITMAP)SelectObject(frontBuff, backBM);

	while(1)
	{
		SetPixelV(frontBuff, 10, 10, RGB(255, 255, 255));
		BitBlt(backBuff, 0, 0, 100, 100, frontBuff, 0, 0, SRCCOPY);
	}


	return 0;
}
*/