#pragma once

#include "resource_cache.hpp"

#include "../wrapper/raytracing_acceleration.hpp"
#include "../wrapper/descriptor_heap.hpp"
#include "../wrapper/root_signature.hpp"

namespace path_tracing::dx::utilities {

	struct scene_info final {
		matrix4x4 mInvViewProjection = matrix4x4(1);

		scene_info() = default;
	};
	
	class resource_scene final : public noncopyable {
	public:
		explicit resource_scene(const std::shared_ptr<device>& device);

		~resource_scene() = default;

		void set_render_target(const std::shared_ptr<texture2d>& render_target);
		
		void set_instances(const std::vector<raytracing_instance>& instances);
		
		void set_materials(const std::vector<material_gpu_buffer>& materials);

		void set_emitters(const std::vector<emitter_gpu_buffer>& emitters);

		void set_entities(const std::vector<std::shared_ptr<entity>>& entities,
			const std::shared_ptr<resource_cache>& cache);
		
		void set_scene_info(const scene_info& info);

		void execute(const std::shared_ptr<command_queue>& queue);
	private:
		std::shared_ptr<descriptor_table> mDescriptorTable;
		std::shared_ptr<descriptor_heap> mDescriptorHeap;
		std::shared_ptr<root_signature> mRootSignature;

		std::shared_ptr<raytracing_acceleration> mAcceleration;
		
		std::shared_ptr<graphics_command_list> mCommandList;
		std::shared_ptr<command_allocator> mCommandAllocator;

		std::shared_ptr<texture2d> mRenderTarget;
		
		std::shared_ptr<buffer> mSceneInfo;
		std::shared_ptr<buffer> mMaterials;
		std::shared_ptr<buffer> mEmitters;
		
		std::shared_ptr<device> mDevice;

		std::vector<raytracing_instance> mInstancesData;
		std::vector<material_gpu_buffer> mMaterialsData;
		std::vector<emitter_gpu_buffer> mEmittersData;
		scene_info mSceneInfoData;
	};
	
}
