#pragma once
#include "N64/Forward.h"

namespace N64::Rdp_detail
{
	struct RenderConfig;

	class DisplayManager
	{
	public:
		void ScanBlank();
		void ScanR5G5B5A1(N64System& n64);
		void ScanR8G8B8A8(N64System& n64);
		void Render(const RenderConfig& config) const;

	private:
		class Impl;

		Vec2 m_videoScale{1.0, 1.0};
		Image m_pixelBuffer{};
		DynamicTexture m_texture{};

		ViType4 m_viTypeBefore{};
	};
}
