﻿#include "stdafx.h"
#include "Display.h"

#include "N64/N64System.h"

namespace N64::Rdp_detail
{
	class Display::Impl
	{
	public:
		// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/frontend/render.c#L118
		static void CheckRecreateTexture(N64System& n64, Display& display)
		{
			// テクスチャの再作成をチェック
			auto&& vi = n64.GetVI();
			const float yScale = static_cast<float>(vi.YScale().Scale()) / 1024.0f;
			const float xScale = static_cast<float>(vi.XScale().Scale()) / 1024.0f;

			// shift操作いるかも?
			const int newHeight = ceilf(static_cast<float>((vi.VVideo().End() - vi.VVideo().Start())) * yScale);
			const int newWidth = ceilf(static_cast<float>((vi.HVideo().End() - vi.HVideo().Start())) * xScale);

			const bool shouldRecreateTexture =
				display.m_pixelBuffer.size() != Size(newWidth, newHeight) ||
				display.m_viTypeBefore != vi.Control().GetType();

			if (shouldRecreateTexture)
			{
				display.m_pixelBuffer = Image(newWidth, newHeight, Palette::Black);
				display.m_texture = DynamicTexture(display.m_pixelBuffer, TexturePixelFormat::R8G8B8A8_Unorm);
				display.m_viTypeBefore = vi.Control().GetType();
			}
		}
	};

	void Display::ScanBlank()
	{
		m_pixelBuffer.fill(Palette::Black);
		m_texture.fill(m_pixelBuffer);
	}

	void Display::ScanR5G5B5A1(N64System& n64)
	{
		Impl::CheckRecreateTexture(n64, *this);

		const uint32 rdramOffset = n64.GetVI().Origin() & RdramSizeMask_0x007FFFFF;
		// TODO
		m_texture.fill(m_pixelBuffer);
	}

	void Display::ScanR8G8B8A8(N64System& n64)
	{
		Impl::CheckRecreateTexture(n64, *this);

		const uint32 rdramOffset = n64.GetVI().Origin() & RdramSizeMask_0x007FFFFF;
		for (int i = 0; i < m_pixelBuffer.width() * m_pixelBuffer.height(); i++)
		{
			m_pixelBuffer.data()[i].r = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 0];
			m_pixelBuffer.data()[i].g = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 1];
			m_pixelBuffer.data()[i].b = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 2];
			// m_pixelBuffer.data()[i].a = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 3];
		}
		// std::copy_n(n64.GetMemory().Rdram().begin() + rdramOffset,
		//             m_pixelBuffer.width() * m_pixelBuffer.height() * 4,
		//             m_pixelBuffer.dataAsUint8());
		m_texture.fill(m_pixelBuffer);
	}

	void Display::Render(const Point& point) const
	{
		(void)m_texture.draw(point);
	}
}