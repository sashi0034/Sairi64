#pragma once

#include "../SoftCommander.h"

namespace N64::Rdp_detail::Soft
{
	static BlendSource From1a(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::PixelAlpha;
		case 1: return BlendSource::MemoryColor;
		case 2: return BlendSource::BlendColor;
		case 3: return BlendSource::FogColor;
		default: return {};
		}
	}

	static BlendSource From1b(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::PixelAlpha;
		case 1: return BlendSource::PrimitiveAlpha;
		case 2: return BlendSource::ShadeAlpha;
		case 3: return BlendSource::Zero;
		default: return {};
		}
	}

	static BlendSource From2a(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::PixelAlpha;
		case 1: return BlendSource::MemoryColor;
		case 2: return BlendSource::BlendColor;
		case 3: return BlendSource::FogColor;
		default: return {};
		}
	}

	static BlendSource From2b(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::OneMinusAlpha;
		case 1: return BlendSource::MemoryAlpha;
		case 2: return BlendSource::One;
		case 3: return BlendSource::Zero;
		default: return {};
		}
	}

	struct HSpan
	{
		int32 startX;
		int32 endX;
	};

	inline uint8 GetBytesPerPixel(const CommanderState& state)
	{
		switch (state.colorImage.size)
		{
		// case 1: return 1;
		case 2: return 2;
		case 3: return 4;
		default: throw std::range_error("size of pixel color element is invalid");
		}
	}

	class TriangleEdgeWalker
	{
	public:
		uint32 GetStartY() const { return m_startY; }
		std::span<HSpan> GetSpan() { return {m_buffer.data(), m_bufferCount}; }

		// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.cpp#L302
		template <uint8 offset>
		void EdgeWalk(const EdgeCoefficient<offset>& ec, uint8 bytesPerPixel)
		{
			const bool rightMajor = ec.RightMajor();
			uint32 startX;
			uint32 endX;
			sint32 startDx;
			sint32 endDx;
			if (rightMajor)
			{
				startX = (ec.Xm() << 16) | ec.XmFrac();
				endX = (ec.Xh() << 16) | ec.XhFrac();
				startDx = (ec.DxMDy() << 16) | ec.DxMDyFrac();
				endDx = (ec.DxHDy() << 16) | ec.DxHDyFrac();
			}
			else
			{
				startX = (ec.Xh() << 16) | ec.XhFrac();
				endX = (ec.Xm() << 16) | ec.XmFrac();
				startDx = (ec.DxHDy() << 16) | ec.DxHDyFrac();
				endDx = (ec.DxMDy() << 16) | ec.DxMDyFrac();
			}
			const uint32 yh = ec.Yh() / bytesPerPixel;
			const uint32 ym = ec.Ym() / bytesPerPixel;
			const uint32 yl = ec.Yl() / bytesPerPixel;

			m_startY = yh;
			m_bufferCount = 0;

			// YHからYMまで計算
			for (uint32 y = yh; yh < ym; ++y)
			{
				m_buffer[m_bufferCount].startX = startX >> 16;
				m_buffer[m_bufferCount].endX = endX >> 16;
				m_bufferCount++;
				startX += startDx;
				endX += endDx;
			}

			if (rightMajor)
			{
				startX = ec.Xl() << 16 | ec.XlFrac();
				startDx = ec.DxLDy() << 16 | ec.DxLDyFrac();
			}
			else
			{
				startX = ec.Xl() << 16 | ec.XlFrac();
				startDx = ec.DxLDy() << 16 | ec.DxLDyFrac();
			}

			// YMからYLまで計算
			for (uint32 y = ym; y < yl; ++y)
			{
				m_buffer[m_bufferCount].startX = startX >> 16;
				m_buffer[m_bufferCount].endX = endX >> 16;
				m_bufferCount++;
				startX += startDx;
				endX += endDx;
			}
		}

	private:
		uint32 m_startY{};
		uint32 m_bufferCount{};
		std::array<HSpan, 1024> m_buffer{};
	};
}
