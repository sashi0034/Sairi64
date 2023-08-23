#pragma once
#include "N64/Forward.h"

namespace N64::Rdp_detail
{
	class Display
	{
	public:
		void ScanOut16Bit(N64System& n64);
		void ScanOut32Bit(N64System& n64);
		void Render(const Point& point) const;
	private:
		Image m_pixelBuffer{};
		DynamicTexture m_texture{};
	};
}
