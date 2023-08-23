#include "stdafx.h"
#include "Display.h"

namespace N64::Rdp_detail
{
	void Display::ScanOut16Bit(N64System& n64)
	{
		if (m_pixelBuffer.size() == Size(0, 0))
		{
			m_pixelBuffer.resize(200, 200);
			m_pixelBuffer.fill(Palette::Black);
			m_texture.resized(200, 200);
		}

		for (int x = 0; x < 30; ++x)
		{
			for (int y = 0; y < 30; ++y)
			{
				auto&& pixel = m_pixelBuffer[y][x];
				pixel.r = 255;
				pixel.g = 0;
				pixel.b = 50;
				pixel.a = 255;
			}
		}
		m_texture.fill(m_pixelBuffer);
	}

	void Display::ScanOut32Bit(N64System& n64)
	{
	}

	void Display::Render(const Point& point) const
	{
		(void)m_texture.draw(point);
	}
}
