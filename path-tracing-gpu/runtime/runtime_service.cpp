#include "runtime_service.hpp"

#include "runtime_process.hpp"

path_tracing::runtime::runtime_service::runtime_service(runtime_process& process) :
	resource_system(process.mResourceSystem), meshes_system(process.mMeshesSystem),
	render_device(process.mRenderDevice), render_system(process.mRenderSystem),
	window_system(process.mWindowSystem), scene(process.mScene)
{
}
