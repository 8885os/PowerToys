#pragma once
#include <robmikh.common/DesktopWindow.h>
#include "CropAndLockWindow.h"

struct ThumbnailCropAndLockWindow : robmikh::common::desktop::DesktopWindow<ThumbnailCropAndLockWindow>, CropAndLockWindow
{
	static const std::wstring ClassName;
	ThumbnailCropAndLockWindow(std::wstring const& titleString, int width, int height);
	~ThumbnailCropAndLockWindow() override;
	LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam);

	HWND Handle() override { return m_window; }
	void CropAndLock(HWND windowToCrop, RECT cropRect) override;
	void OnClosed(std::function<void(HWND)> callback) override { m_closedCallback = callback; }

private:
	static void RegisterWindowClass();

	void Hide();
	void DisconnectTarget();

	// Keep-alive: DWM freezes a thumbnail while its source is minimized, so a source that
	// gets minimized is parked off-screen (not minimized) and moved back when it's activated.
	void Hook();
	void Unhook();
	void ParkTarget();
	void UnparkTarget();
	void OnTargetMinimizeStart();
	void OnTargetForeground();
	static void CALLBACK WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD eventThread, DWORD eventTime);

private:
	HWND m_currentTarget = nullptr;
	POINT m_previousPosition = {};

	unique_hthumbnail m_thumbnail;
	RECT m_destRect = {};
	RECT m_sourceRect = {};

	HWINEVENTHOOK m_minimizeHook = nullptr;
	HWINEVENTHOOK m_foregroundHook = nullptr;
	HWINEVENTHOOK m_destroyHook = nullptr;
	WINDOWPLACEMENT m_savedPlacement = {};
	bool m_targetParked = false;
	bool m_suppressMinimizeHook = false; // Set while we minimize the target ourselves
	bool m_minimized = false;

	bool m_destroyed = false;
	std::function<void(HWND)> m_closedCallback;
};