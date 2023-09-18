#pragma once
#include "SoftCommon.h"

namespace N64::Rdp_detail::Soft
{
	class RectangleEdgeWalker
	{
	public:
		RectangleEdgeWalker(uint32 startY, uint32 vLength, const HSpan& hSpan):
			m_startY(startY),
			m_endY(vLength),
			m_hSpan(hSpan)
		{
		}

		uint32 GetStartY() const { return m_startY; }
		uint32 GetEndY() const { return m_endY; }
		const HSpan& GetHSpan(uint32 y) const { return m_hSpan; }

	private:
		uint32 m_startY{};
		uint32 m_endY{};
		HSpan m_hSpan{};
	};

	class TriangleEdgeWalker
	{
	public:
		uint32 GetStartY() const { return m_startY; }
		uint32 GetEndY() const { return m_startY + m_bufferCount - 1; }
		const HSpan& GetHSpan(uint32 y) const { return m_buffer[y - m_startY]; }

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
			for (uint32 y = yh; y < ym; ++y)
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
