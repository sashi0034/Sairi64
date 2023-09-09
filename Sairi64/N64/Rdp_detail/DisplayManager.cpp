#include "stdafx.h"
#include "DisplayManager.h"

#include "N64/N64System.h"

namespace N64::Rdp_detail
{
	class DisplayManager::Impl
	{
	public:
		// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/frontend/render.c#L118
		static void CheckRecreateTexture(N64System& n64, DisplayManager& display)
		{
			// テクスチャの再作成をチェック
			auto&& vi = n64.GetVI();

			const float yScale = static_cast<float>(vi.YScale().Scale()) / 1024.0f;
			const float xScale = static_cast<float>(vi.XScale().Scale()) / 1024.0f;

			const auto vVideo = vi.VVideo().End() - vi.VVideo().Start();
			const auto hVideo = vi.HVideo().End() - vi.HVideo().Start();

			const int newHeight = ceilf(static_cast<float>(vVideo >> 1) * yScale);
			const int newWidth = ceilf(static_cast<float>(hVideo >> 0) * xScale);

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

	void DisplayManager::ScanBlank()
	{
		m_pixelBuffer.fill(Palette::Black);
		m_texture.fill(m_pixelBuffer);
	}

	void DisplayManager::ScanR5G5B5A1(N64System& n64)
	{
		Impl::CheckRecreateTexture(n64, *this);

		const uint32 rdramOffset = n64.GetVI().Origin() & RdramSizeMask_0x007FFFFF;
		for (int i = 0; i < m_pixelBuffer.width() * m_pixelBuffer.height(); i++)
		{
			const uint16 pixel =
				(n64.GetMemory().Rdram()[rdramOffset + i * 2 + 0] << 8) |
				(n64.GetMemory().Rdram()[rdramOffset + i * 2 + 1]);
			m_pixelBuffer.data()[i].r = GetBits<11, 15>(pixel) << 3;
			m_pixelBuffer.data()[i].g = GetBits<6, 10>(pixel) << 3;
			m_pixelBuffer.data()[i].b = GetBits<1, 5>(pixel) << 3;
		}
	}

	void DisplayManager::ScanR8G8B8A8(N64System& n64)
	{
		Impl::CheckRecreateTexture(n64, *this);

		const uint32 rdramOffset = n64.GetVI().Origin() & RdramSizeMask_0x007FFFFF;
		for (int i = 0; i < m_pixelBuffer.width() * m_pixelBuffer.height(); i++)
		{
			m_pixelBuffer.data()[i].r = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 3];
			m_pixelBuffer.data()[i].g = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 2];
			m_pixelBuffer.data()[i].b = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 1];
			// m_pixelBuffer.data()[i].a = n64.GetMemory().Rdram()[rdramOffset + i * 4 + 0];
		}
		// std::copy_n(n64.GetMemory().Rdram().begin() + rdramOffset,
		//             m_pixelBuffer.width() * m_pixelBuffer.height() * 4,
		//             m_pixelBuffer.dataAsUint8());
	}

	void DisplayManager::Render(const RenderConfig& config) const
	{
		m_texture.fill(m_pixelBuffer);
		(void)m_texture.scaled(config.scale * m_videoScale).draw(config.startPoint);
	}
}
