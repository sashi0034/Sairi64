#pragma once
#include "N64/Forward.h"

namespace N64::Rdp_detail
{
	class Display
	{
	public:
		void ScanBlank();
		void ScanR5G5B5A1(N64System& n64);
		void ScanR8G8B8A8(N64System& n64);
		void Render(const Point& point) const;

	private:
		class Impl;

		Image m_pixelBuffer{};
		DynamicTexture m_texture{};

		ViType4 m_viTypeBefore{};
	};
}
