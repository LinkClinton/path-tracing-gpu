#include "convert_shape.hpp"

#include "meta-scene/shapes/triangles.hpp"
#include "meta-scene/shapes/sphere.hpp"
#include "meta-scene/shapes/mesh.hpp"

#include "../shapes/sphere.hpp"
#include "../shapes/mesh.hpp"

#include "../resource_manager.hpp"

namespace path_tracing::core::converter {

	std::shared_ptr<shape> create_ply_mesh(const std::shared_ptr<metascene::shapes::mesh>& mesh)
	{
		return resource_manager::read_ply_mesh(mesh);
	}

	std::shared_ptr<shape> create_mesh(const std::shared_ptr<metascene::shapes::mesh>& mesh)
	{
		if (mesh->mesh_type == metascene::shapes::mesh_type::ply)
			return create_ply_mesh(mesh);

		return nullptr;
	}

	std::shared_ptr<shape> create_sphere(const std::shared_ptr<metascene::shapes::sphere>& sphere)
	{
		return std::make_shared<shapes::sphere>(sphere->radius, sphere->reverse_orientation);
	}
	
	std::shared_ptr<shape> create_triangles(const std::shared_ptr<metascene::shapes::triangles>& triangles)
	{
		return std::make_shared<mesh>(
			triangles->positions,
			triangles->normals,
			triangles->uvs,
			triangles->indices,
			triangles->reverse_orientation);
	}
	
	std::shared_ptr<shape> create_shape(const std::shared_ptr<metascene::shapes::shape>& shape)
	{
		if (shape == nullptr) return nullptr;

		if (shape->type == metascene::shapes::type::mesh)
			return create_mesh(std::static_pointer_cast<metascene::shapes::mesh>(shape));

		if (shape->type == metascene::shapes::type::sphere)
			return create_sphere(std::static_pointer_cast<metascene::shapes::sphere>(shape));

		if (shape->type == metascene::shapes::type::triangles)
			return create_triangles(std::static_pointer_cast<metascene::shapes::triangles>(shape));

		return std::make_shared<sphere>(static_cast<real>(1));
	}
}