#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <cassert>
#include <vector>
#include <chrono>
#include <thread>

#pragma comment (lib,"d3d12.lib")
#pragma comment (lib,"dxgi.lib")

// DirectX基盤
class DirectXBase
{
private:
	HRESULT result;

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

public:
	std::vector<ID3D12Resource*> backBuffers;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;

	D3D12_RESOURCE_BARRIER barrierDesc{};
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

	ComPtr<ID3D12Resource> depthBuff = nullptr;

public:
	ComPtr<ID3D12Device> device = nullptr; // デバイス
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr; // コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr; // コマンドリスト
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr; // コマンドキュー
	ComPtr<IDXGISwapChain4> swapChain = nullptr; // スワップチェーン
	ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr; // レンダーターゲットビュー

	ComPtr<IDXGIFactory7> dxgiFactory = nullptr; // 統合

public:
	// 初期化
	void Init();

	// 描画前処理
	void PreDraw();

	// 描画後処理
	void PostDraw();

private:
	// FPS固定初期化
	void InitializeFixFPS();

	// FPS固定更新
	void UpdateFixFPS();

public:
	// ゲッター
	static DirectXBase* Get();

private:
	static DirectXBase currentDirectX;
};