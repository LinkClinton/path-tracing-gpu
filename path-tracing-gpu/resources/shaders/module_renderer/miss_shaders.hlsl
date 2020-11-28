#include "module_types.hlsl"

[shader("miss")]
void miss_shader(inout SYSTEM_VALUE(tracing_payload, payload, SV_RayPayload))
{
	payload.missed = 1;
}