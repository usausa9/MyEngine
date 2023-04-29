#pragma once
#include <windows.h>
#include <string>

class WinAPI
{
public:
	// ウィンドウサイズ
	int width = 1280;
	int height = 720;
	
	// ウィンドウクラスの設定
	WNDCLASSEX w{};
	// ウィンドウハンドルの生成
	HWND hwnd;
	// ウィンドウサイズ用の長方形の生成
	RECT wrc;
	// メッセージ情報構造体の生成
	MSG msg = {};

public:
	// ウィンドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// Windowクラスの設定
	void Init();

	// 更新処理
	bool ProcessMessage();

	// コンソールへの文字出力
	void DebugText(LPCSTR text);

	// ウィンドウオブジェクトの生成
	void CreateWindowObj(LPCWSTR title, int windowWidth, int windowHeight);

	// ウィンドウ表示
	void Show();

public:
	// ゲッター
	static WinAPI* Get();

private:
	static WinAPI currentWindow;
};