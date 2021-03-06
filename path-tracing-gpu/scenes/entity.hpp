#pragma once

#include "../runtime/resources/meshes_system.hpp"

#include "components/submodule_data.hpp"
#include "components/transform.hpp"

#include <optional>

namespace path_tracing::scenes {

	using namespace components;

	using runtime::resources::mesh_info;
	
	struct entity final {
		std::optional<submodule_data> material;
		std::optional<submodule_data> light;

		std::optional<mesh_info> mesh;
		
		transform transform;
		
		entity() = default;
	};

	real compute_mesh_area_with_transform(const runtime::resources::meshes_system& system, 
		const transform& transform, const mesh_info& info);
	
}
