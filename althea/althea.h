#ifndef _ALTHEA_ALTHEA_H_
#define _ALTHEA_ALTHEA_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

void show_error_dialog(wchar_t * message);

struct _Application
{
	HWND desktop_workerw_handle;
};

typedef struct _Application Application;

BOOL CALLBACK find_wallpaper_window_enum_windows_proc(HWND hWndParent, LPARAM lParam);

bool find_wallpaper_window(Application * application);

bool create_app(Application * application, HINSTANCE hInstance);

struct _WallpaperWindow
{
	int width;
	int height;

	HWND window_handle;
};

typedef struct _WallpaperWindow WallpaperWindow;

LRESULT wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool create_window(WallpaperWindow * window, HINSTANCE hInstance);

void set_wallpaper(Application * application, WallpaperWindow * window);

#endif
