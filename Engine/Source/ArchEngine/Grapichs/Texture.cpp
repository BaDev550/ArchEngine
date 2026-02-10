#include "ArchPch.h"
#include "Texture.h"
#include "ArchEngine/Core/Application.h"

#include <stb_image.h>

namespace ae::grapichs {
	void Texture2D::LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels)
	{
	}
	void Texture2D::CreateTexture()
	{
	}
	void Texture2D::CreateTextureImageView()
	{
	}
	void Texture2D::CreateTextureSampler()
	{
	}

	Texture2D::Texture2D(const TextureSpecification& specs)
		: _context(Application::Get()->GetWindow().GetRenderContext())
	{
	}
	Texture2D::Texture2D(const TextureSpecification& specs, const std::filesystem::path& path)
		: _context(Application::Get()->GetWindow().GetRenderContext())
	{
	}
	Texture2D::Texture2D(const TextureSpecification& specs, DataBuffer data)
		: _context(Application::Get()->GetWindow().GetRenderContext())
	{
	}
	Texture2D::~Texture2D()
	{
	}
}