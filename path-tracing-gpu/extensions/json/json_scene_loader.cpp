#include "json_scene_loader.hpp"

#include "../models/tiny_obj_loader.hpp"
#include "../models/tiny_ply_loader.hpp"

#include "json_submodule_loader.hpp"

#include <filesystem>
#include <fstream>

using namespace path_tracing::runtime::resources;
using namespace path_tracing::scenes::components;

namespace path_tracing::extensions::json {

	transform load_transform_from_matrix_property(const nlohmann::json& json)
	{
		matrix4x4 matrix;

		matrix[0][0] = json[0];  matrix[0][1] = json[1];  matrix[0][2] = json[2];  matrix[0][3] = json[3];
		matrix[1][0] = json[4];  matrix[1][1] = json[5];  matrix[1][2] = json[6];  matrix[1][3] = json[7];
		matrix[2][0] = json[8];  matrix[2][1] = json[9];  matrix[2][2] = json[10]; matrix[2][3] = json[11];
		matrix[3][0] = json[12]; matrix[3][1] = json[13]; matrix[3][2] = json[14]; matrix[3][3] = json[15];

		return transform(matrix);
	}

	transform load_transform_from_look_at_property(const coordinate_system& system, const nlohmann::json& json)
	{
		const vector3 eye = json["eye"];
		const vector3 at = json["at"];
		const vector3 up = json["up"];

		return transform(look_at(system, eye, at, up)).inverse();
	}

	transform load_transform_from_property(const coordinate_system& system, const nlohmann::json& json)
	{
		if (json.contains("matrix")) return load_transform_from_matrix_property(json["matrix"]);
		if (json.contains("eye")) return load_transform_from_look_at_property(system, json);

		throw "not implementation";
	}
	
	perspective_camera load_perspective_camera(const coordinate_system& system, const nlohmann::json& json)
	{
		return perspective_camera{
			load_transform_from_property(system, json["transform"]),
			json["perspective"]["fov"].get<real>()
		};
	}

	film load_film(const nlohmann::json& json)
	{
		return film{
			json["resolution"][0], json["resolution"][1],
			json["scale"]
		};
	}

	mesh_info load_mesh_from_data_property(meshes_system& system, const nlohmann::json& json)
	{
		mesh_cpu_buffer instance;

		instance.positions = json["positions"].get<std::vector<vector3>>();
		instance.indices = json["indices"].get<std::vector<uint32>>();

		if (json.contains("normals")) instance.normals = json["normals"].get<std::vector<vector3>>();
		if (json.contains("uvs")) instance.uvs = json["uvs"].get<std::vector<vector3>>();

		return system.allocate("unknown" + std::to_string(system.count()), std::move(instance));
	}

	mesh_info load_mesh_from_obj_property(meshes_system& system, const nlohmann::json& json, const std::string& directory)
	{
		const auto fullpath = directory + json["filename"].get<std::string>();

		if (system.has(fullpath)) return system.info(fullpath);

		auto buffer = models::load_obj_mesh(fullpath);

		return system.allocate(fullpath, std::move(buffer));
	}

	mesh_info load_mesh_from_ply_property(meshes_system& system, const nlohmann::json& json, const std::string& directory)
	{
		const auto fullpath = directory + json["filename"].get<std::string>();

		if (system.has(fullpath)) return system.info(fullpath);

		auto buffer = models::load_ply_mesh(fullpath);

		return system.allocate(fullpath, std::move(buffer));
	}
	
	mesh_info load_mesh_from_file_property(meshes_system& system, const nlohmann::json& json, const std::string& directory)
	{
		if (json["type"] == "obj") return load_mesh_from_obj_property(system, json, directory);
		if (json["type"] == "ply") return load_mesh_from_ply_property(system, json, directory);
		
		throw "not implementation";
	}
	
	mesh_info load_mesh_from_property(meshes_system& system, const nlohmann::json& json, const std::string& directory)
	{
		if (json["type"] == "triangles") return load_mesh_from_data_property(system, json["triangles"]);
		if (json["type"] == "mesh") return load_mesh_from_file_property(system, json["mesh"], directory);
		
		throw "not implementation";
	}
}

void path_tracing::extensions::json::json_scene_loader::load(const runtime_service& service, 
	const nlohmann::json& scene, const std::string& directory)
{
	if (scene.contains("config") == true) {
		const auto config = scene["config"];

		if (config.contains("coordinate_system")) {
			service.scene.camera_system = config["coordinate_system"]["camera"] == "right_hand" ? 
				coordinate_system::right_hand : coordinate_system::left_hand;
			service.scene.texture_system = config["coordinate_system"]["texture"] == "right_hand" ?
				coordinate_system::right_hand : coordinate_system::left_hand;
		}

		if (config.contains("output_window")) {
			const auto output_window = config["output_window"];
			
			service.scene.output_window = scenes::output_window_property{
				output_window["name"], output_window["font"],
				output_window["size"][0], output_window["size"][1],
				output_window["enable"]
			};
		}

		if (config.contains("output_images")) {
			service.scene.output_images = scenes::output_images_property{
				config["output_images"]["sdr"],
				config["output_images"]["hdr"]
			};
		}
		
		if (config.contains("sample_count"))
			service.scene.sample_count = config["sample_count"];

		if (config.contains("max_depth"))
			service.scene.max_depth = config["max_depth"];
	}

	if (scene.contains("camera")) 
		service.scene.camera = load_perspective_camera(service.scene.camera_system, scene["camera"]);
	
	if (scene.contains("film")) 
		service.scene.film = load_film(scene["film"]);

	uint32 index = 0;
	
	for (const auto& entity : scene["entities"]) {
		const auto transform = load_transform_from_property(service.scene.camera_system, entity["transform"]);

		std::optional<submodule_data> material = std::nullopt;
		std::optional<submodule_data> light = std::nullopt;
		std::optional<mesh_info> mesh = std::nullopt;

		if (entity.contains("material")) material = load_material_from_json(entity["material"]);
		if (entity.contains("light")) light = load_light_from_json(service.images_system, entity["light"], directory, index);
		if (entity.contains("shape")) mesh = load_mesh_from_property(service.meshes_system, entity["shape"], directory);

		service.scene.entities.push_back({ material, light, mesh, transform });

		index++;
	}
	
	service.meshes_system.upload_cached_buffers(service.render_device);
	service.images_system.upload_cached_buffers(service.render_device);
	
	service.render_device.wait();
}

void path_tracing::extensions::json::json_scene_loader::load(const runtime_service& service, const std::string& filename)
{
	std::ifstream stream(filename);

	nlohmann::json scene; stream >> scene;

	load(service, scene, std::filesystem::path(filename).parent_path().generic_string() + "/");

	stream.close();
}
