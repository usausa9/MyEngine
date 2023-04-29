#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//// DirectInputの初期化
//static ComPtr<IDirectInput8> directInput = nullptr;
//static ComPtr<IDirectInputDevice8> keyboard = nullptr;

using namespace Input;

Microsoft::WRL::ComPtr<IDirectInputDevice8> Key::keyboard = nullptr;

XINPUT_STATE Pad::padState;
XINPUT_STATE Pad::lastPadState;

// 全キーの入力状態を取得する
static BYTE _keys[256] = {};

// 全キーの1F前の入力状態を取得する
static BYTE _prev[256] = {};

void Key::Init(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

	// DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Key::Update()
{
	// 前後更新
	for (int i = 0; i < 256; ++i)
	{
		_prev[i] = _keys[i];
	}

	// キーボード情報の取得開始
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(_keys), _keys);
}

// 押しっぱなし
bool Key::Down(UINT8 keyNum)
{
	return _keys[keyNum];
}

// 押した瞬間
bool Key::Trigger(UINT8 keyNum)
{
	return _keys[keyNum] && !_prev[keyNum];
}

// 離した瞬間
bool Key::Released(UINT8 keyNum)
{
	return !_keys[keyNum] && !_prev[keyNum];
}

void Pad::Init()
{

}

void Pad::Update()
{
	lastPadState = padState;

	XInputGetState(0, &padState);
}

bool Pad::Down(Button button)
{
	return padState.Gamepad.wButtons & (UINT)button;
}

bool Pad::Trigger(Button button)
{
	return padState.Gamepad.wButtons & (UINT)button && !(lastPadState.Gamepad.wButtons & (UINT)button);
}

bool Pad::Released(Button button)
{
	return lastPadState.Gamepad.wButtons & (UINT)button && !(padState.Gamepad.wButtons & (UINT)button);
}

XMFLOAT2 Pad::GetLStick()
{
	float x = padState.Gamepad.sThumbLX;
	float y = padState.Gamepad.sThumbLY;

	if (x * x < 5000 * 5000)
	{
		x = 0;
	}
	if (y * y < 5000 * 5000)
	{
		y = 0;
	}

	x = x / 32767;
	y = y / 32767;

	return { x, y };
}

XMFLOAT2 Pad::GetRStick()
{
	float x = padState.Gamepad.sThumbRX;
	float y = padState.Gamepad.sThumbRY;

	x = x / 32767;
	y = y / 32767;

	return { x, y };
}