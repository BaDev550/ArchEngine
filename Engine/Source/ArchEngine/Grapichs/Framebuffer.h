#pragma once
#include "RenderContext.h"
#include "Texture.h"

#include "ArchEngine/Core/Memory.h"

#include <glm/glm.hpp>

namespace ae::grapichs {
	struct FramebufferAttachmmentSpecification {
		FramebufferAttachmmentSpecification() = default;
		FramebufferAttachmmentSpecification(const std::initializer_list<vk::Format>& attachments) : Attachments(attachments) {}
		std::vector<vk::Format> Attachments;
	};

	struct FramebufferSpecification {
		uint32_t Width = 0;
		uint32_t Height = 0;
		float DepthClearValue = 0.0f;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		FramebufferAttachmmentSpecification Attachments;
	};

	class Framebuffer : public memory::RefCounted {
	public:
		Framebuffer(const FramebufferSpecification& spec);
		~Framebuffer();

		void Resize(uint32_t width, uint32_t height);
		uint32_t GetWidth() const { return _specs.Width; }
		uint32_t GetHeight() const { return _specs.Height; }
		uint32_t GetAttachmentCount() const {};
		memory::Ref<Texture2D> GetAttachmentTexture(uint32_t index = 0) const { return _colorAttachments[index]; }
		memory::Ref<Texture2D> GetDepthTexture() const { return _depthAttachment; }
		bool DoesFramebufferHasDepthAttachment() const { return _depthAttachment; }
		const FramebufferSpecification& GetSpecification() const { return _specs; }
	private:
		std::vector<memory::Ref<Texture2D>> _colorAttachments;
		memory::Ref<Texture2D> _depthAttachment;
		FramebufferSpecification _specs;
	};
}