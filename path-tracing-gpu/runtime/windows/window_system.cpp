#include "window_system.hpp"

#include "../runtime_service.hpp"

void path_tracing::runtime::windows::window_system::resolve(const runtime_service& service)
{
	if (service.scene.output_window.has_value()) {
		mViewWindow = view_window(
			service.render_device.queue(), service.render_device.device(),
			service.scene.output_window->name,
			static_cast<uint32>(service.scene.camera.resolution.x),
			static_cast<uint32>(service.scene.camera.resolution.y));

		mImGuiRenderer.resolve(mViewWindow.mSwapChain, service.render_device, service.scene.output_window->font_size);

		mRenderTargetID = mImGuiRenderer.allocate(service.resource_system.resource<
			wrapper::directx12::texture2d>("RenderSystem.RenderTarget.SDR"), 1);
	}
}

void path_tracing::runtime::windows::window_system::update(const runtime_service& service, real delta)
{
	if (mViewWindow.living() == false) return;
	
	mViewWindow.update(delta);
	
	mImGuiRenderer.update(service, delta);

	ImGui::GetBackgroundDrawList()->AddImage(mRenderTargetID, ImVec2(0, 0),
		ImVec2(
			static_cast<real>(mViewWindow.size_x()),
			static_cast<real>(mViewWindow.size_y())));
}

void path_tracing::runtime::windows::window_system::render(const runtime_service& service, real delta)
{
	if (mViewWindow.living() == false) return;

	mImGuiRenderer.render(service, delta);

	mViewWindow.present(false);
}

bool path_tracing::runtime::windows::window_system::living() const noexcept
{
	return mViewWindow.living();
}