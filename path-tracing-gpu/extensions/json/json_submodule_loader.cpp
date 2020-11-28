#include "json_submodule_loader.hpp"

namespace path_tracing::extensions::json {

	submodule_data load_diffuse_material_from_json(const nlohmann::json& material)
	{
		submodule_data submodule;

		submodule.float3.insert({ "diffuse", material["diffuse"] });

		submodule.submodule = "diffuse_material";

		return submodule;
	}
	
	submodule_data load_material_from_json(const nlohmann::json& material)
	{
		if (material["type"] == "diffuse") return load_diffuse_material_from_json(material);

		return {};
	}

	submodule_data load_surface_light_from_json(const nlohmann::json& light, uint32 index)
	{	
		submodule_data submodule;

		submodule.float3.insert({ "intensity", light["radiance"] });

		submodule.uint.insert({ "entity", index });
		submodule.uint.insert({ "delta", 0 });
		
		submodule.submodule = "surface_light";
		
		return submodule;
	}
	
	submodule_data load_light_from_json(const nlohmann::json& light, uint32 index)
	{
		if (light["type"] == "surface") return load_surface_light_from_json(light, index);

		return {};
	}

}
