#pragma once
#include <dinput.h>
#include <Windows.h>
#include <Xinput.h>

#include <DirectXMath.h>

#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif

#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// ComPtr
#include <wrl.h>

#include "Common.h"

enum class Button
{
	UP = 0x0001,
	DOWN = 0x0002,
	LEFT = 0x0004,
	RIGHT = 0x0008,
	START = 0x0010,
	BACK = 0x0020,
	LSTICKBUTTON = 0x0040,
	RSTICKBUTTON = 0x0080,
	L = 0x0100,
	R = 0x0200,
	A = 0x1000,
	B = 0x2000,
	X = 0x4000,
	Y = 0x8000,
};

namespace Input
{
	class Key
	{
	public:
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

		// 初期化とアップデートの関数 用意
		static void Init(HINSTANCE hInstance, HWND hwnd);
		static void Update();

		// キーボード入力処理用 (返り値0,1)
		static bool Down(UINT8 keyNum);		// 押しっぱなし
		static bool Trigger(UINT8 keyNum);	// 押した瞬間
		static bool Released(UINT8 keyNum);	// 離した瞬間

	private: // メンバ変数
		static ComPtr<IDirectInputDevice8> keyboard;
	};

	class Pad
	{
	public:
		static void Init();
		static void Update();

		static bool Down(Button button);		// 押しっぱなし
		static bool Trigger(Button button);		// 押した瞬間
		static bool Released(Button button);	// 離した瞬間

		static XMFLOAT2 GetLStick();
		static XMFLOAT2 GetRStick();

	private:
		static XINPUT_STATE padState;
		static XINPUT_STATE lastPadState;
	};
}