#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

namespace path_tracing::dx::wrapper {

	using Microsoft::WRL::ComPtr;

	size_t size_of(const DXGI_FORMAT& format);

	size_t align_to(size_t value, size_t alignment);

	D3D12_CPU_DESCRIPTOR_HANDLE offset_handle(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, size_t value);

	D3D12_GPU_DESCRIPTOR_HANDLE offset_handle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, size_t value);
	
}
