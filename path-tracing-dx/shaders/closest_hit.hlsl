#ifndef __CLOSEST_HIT_HLSL__
#define __CLOSEST_HIT_HLSL__

#include "resource_scene.hlsl"

StructuredBuffer<float3> positions : register(t0, space100);
StructuredBuffer<float3> normals : register(t1, space100);
StructuredBuffer<float3> uvs : register(t3, space100);

StructuredBuffer<uint3> indices : register(t2, space100);

[shader("closesthit")]
void closest_hit_shader(inout ray_payload payload, HitAttributes attribute) {
	uint3 index = indices[PrimitiveIndex()];

	float3 position0 = positions[index.x];
	float3 position1 = positions[index.y];
	float3 position2 = positions[index.z];

	float3 e1 = position1 - position0;
	float3 e2 = position2 - position0;

	float b0 = 1 - attribute.barycentrics.x - attribute.barycentrics.y;
	float b1 = attribute.barycentrics.x;
	float b2 = attribute.barycentrics.y;

	float3 local_position = position0 * b0 + position1 * b1 + position2 * b2;
	float3 local_normal = cross(e1, e2);

	float3x4 local_to_world = ObjectToWorld3x4();
	float3x3 inv_transpose = float3x3(WorldToObject4x3()[0], WorldToObject4x3()[1], WorldToObject4x3()[2]);

	payload.interaction.position = mul(local_position, local_to_world).xyz;
	payload.interaction.normal = normalize(mul(local_normal, inv_transpose).xyz);
	payload.interaction.uv = (uvs[index.x] * b0 + uvs[index.y] * b1 + uvs[index.z] * b2).xy;
	payload.interaction.wo = -WorldRayDirection();

	float3 shading_normal = normals[index.x] * b0 + normals[index.y] * b1 + normals[index.z] * b2;

	payload.interaction.shading_space = build_coordinate_system(mul(shading_normal, inv_transpose).xyz);
	payload.index = InstanceID();
	payload.missed = false;
}

#endif