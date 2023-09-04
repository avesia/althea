/*
* 
* Althea dynamic desktop wallpaper
* 
* (c) 2023 Raku Hasegawa
* 
*/

#include "althea.h"

const wchar_t CLASS_NAME[] = L"AltDynWallpaper";

const wchar_t SHELLDLL_DEFVIEW[] = L"SHELLDLL_DefView";
const wchar_t WORKER_W[] = L"WorkerW";
const wchar_t PROGMAN[] = L"Progman";

void show_error_dialog(wchar_t * message)
{
	MessageBoxW(NULL, message, L"Error", MB_OK);
}

BOOL CALLBACK find_wallpaper_window_enum_windows_proc(
	HWND hWndParent,
	LPARAM lParam
)
{
	HWND shell = FindWindowExW(hWndParent, NULL, SHELLDLL_DEFVIEW, NULL);

	if (shell != NULL)
	{
		Application * application = (Application*)lParam;

		application->desktop_workerw_handle = FindWindowExW(NULL, NULL, WORKER_W, NULL);
	}

	return true;
}

bool find_wallpaper_window(Application * application)
{
	HWND progman_handle = FindWindowW(PROGMAN, NULL);

	if (progman_handle == NULL)
	{
		show_error_dialog(L"cannot find Progman");
		return false;
	}

	ULONG_PTR progman_result = NULL;

	SendMessageTimeoutW(
		progman_handle,
		0x052C,
		NULL,
		NULL,
		SMTO_NORMAL,
		1000,
		&progman_result
	);

	/* Find the window that has `SHELLDLL_DefView` as a child.  */
	BOOL result = EnumWindows(find_wallpaper_window_enum_windows_proc, (LPARAM)application);

	if (!result)
	{
		return false;
	}

	if (application->desktop_workerw_handle == NULL)
	{
		show_error_dialog(L"cannot find `WorkerW`");
		return false;
	}

	return true;
}

bool create_app(Application * application, HINSTANCE hInstance)
{
	bool result = false;

	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = wndproc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	result = find_wallpaper_window(&application);

	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT wndproc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool create_window(WallpaperWindow * window, HINSTANCE hInstance)
{
	HWND hWnd = CreateWindowExW(
		0,
		CLASS_NAME,
		CLASS_NAME,
		WS_OVERLAPPEDWINDOW,
		1920,
		1080,
		0,
		0,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hWnd == NULL)
	{
		return false;
	}

	return true;
}

void set_wallpaper(Application * application, WallpaperWindow * window)
{
	SetParent(application->desktop_workerw_handle, window->window_handle);
}

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow
)
{
	bool result = false;

	Application * application = (Application *)malloc(sizeof(Application));

	if (application == NULL)
	{
		return EXIT_FAILURE;
	}

	result = create_app(&application, hInstance);
	
	if (!result)
	{
		return EXIT_FAILURE;
	}

	/* Create a window for wallpaper */
	WallpaperWindow * window = (WallpaperWindow *)malloc(sizeof(WallpaperWindow));

	result = create_window(&window, hInstance);

	if (!result)
	{
		return EXIT_FAILURE;
	}

	free(application);

	return EXIT_SUCCESS;
}
