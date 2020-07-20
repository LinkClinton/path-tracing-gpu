#include "texture.hpp"

path_tracing::core::textures::texture::texture(const std::shared_ptr<textures::image>& image, const vector3& scale) :
	indexable(mIndexCount++), mImage(image), mScale(scale)
{
}

path_tracing::core::textures::texture_gpu_buffer path_tracing::core::textures::texture::gpu_buffer() const noexcept
{
	texture_gpu_buffer buffer;

	buffer.index = mImage->index();
	buffer.scale = mScale;

	return buffer;
}

std::shared_ptr<path_tracing::core::textures::image> path_tracing::core::textures::texture::image() const noexcept
{
	return mImage;
}

path_tracing::core::uint32 path_tracing::core::textures::texture::count() noexcept
{
	return mIndexCount;
}