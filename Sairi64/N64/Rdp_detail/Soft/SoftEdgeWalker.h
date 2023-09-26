#pragma once
#include "SoftCommon.h"

namespace N64::Rdp_detail::Soft
{
	constexpr uint32 EdgeWalkerBufferSize = 1024;

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
		const HSpan& GetXSpan(uint32 y) const { return m_hSpan; }

	private:
		uint32 m_startY{};
		uint32 m_endY{};
		HSpan m_hSpan{};
	};

	struct PrimitiveAttribute
	{
		FixedPoint32<16, 16> s;
		FixedPoint32<16, 16> t;
		FixedPoint32<16, 16> w;
		FixedPoint32<16, 16> r;
		FixedPoint32<16, 16> g;
		FixedPoint32<16, 16> b;
		FixedPoint32<16, 16> a;
		FixedPoint32<16, 16> z;
	};

	template <CommandId cmd>
	class TriangleEdgeWalker
	{
	public:
		static_assert(
			cmd == CommandId::NonShadedTriangle ||
			cmd == CommandId::FillZBufferTriangle ||
			cmd == CommandId::TextureTriangle ||
			cmd == CommandId::TextureZBufferTriangle ||
			cmd == CommandId::ShadeTriangle ||
			cmd == CommandId::ShadeZBufferTriangle ||
			cmd == CommandId::ShadeTextureTriangle ||
			cmd == CommandId::ShadeTextureZBufferTriangle);
		uint32 GetStartY() const { return m_startY; }
		uint32 GetEndY() const { return m_startY + m_bufferCount - 1; }
		const HSpan& GetXSpan(uint32 y) const { return m_spanBuffer[y - m_startY]; }
		const PrimitiveAttribute& GetAttr(uint32 y) const { return m_attrBuffer[y - m_startY]; }

		void EdgeWalk(
			const EdgeCoefficient& ec,
			const TextureCoefficient& tc,
			uint8 bytesPerPixel)
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

			PrimitiveAttribute attr{};
			if constexpr (hasTexture)
			{
				attr.s = FixedPoint32<16, 16>(tc.SInt(), tc.SFrac());
				attr.t = FixedPoint32<16, 16>(tc.TInt(), tc.TFrac());
				attr.w = FixedPoint32<16, 16>(tc.WInt(), tc.WFrac());
			}

			PrimitiveAttribute deltaAttr{};
			if constexpr (hasTexture)
			{
				deltaAttr.s = FixedPoint32<16, 16>(tc.DsDeInt(), tc.DsDeFrac());
				deltaAttr.t = FixedPoint32<16, 16>(tc.DtDeInt(), tc.DtDeFrac());
				deltaAttr.w = FixedPoint32<16, 16>(tc.DwDeInt(), tc.DwDeFrac());
			}

			const uint32 yh = ec.Yh().Int();
			const uint32 ym = ec.Ym().Int();
			const uint32 yl = ec.Yl().Int();

			m_startY = yh;
			m_bufferCount = 0;

			// YHからYMまで計算
			// FIXME: angrylion-rdpではYhなどを固定小数点のままループを回している。
			for (uint32 y = yh; y < ym; ++y)
			{
				scanLine(startX, endX, attr, startDx, endDx, deltaAttr);
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
				scanLine(startX, endX, attr, startDx, endDx, deltaAttr);
			}
		}

	private:
		static constexpr bool hasTexture =
			cmd == CommandId::TextureTriangle ||
			cmd == CommandId::TextureZBufferTriangle ||
			cmd == CommandId::ShadeTextureTriangle ||
			cmd == CommandId::ShadeTextureZBufferTriangle;

		uint32 m_startY{};
		uint32 m_bufferCount{};
		std::array<HSpan, EdgeWalkerBufferSize> m_spanBuffer{};
		std::array<PrimitiveAttribute, EdgeWalkerBufferSize> m_attrBuffer{};

		void scanLine(
			uint32& startX, uint32& endX, PrimitiveAttribute& attr,
			sint32 startDx, sint32 endDx, const PrimitiveAttribute& deltaAttr)
		{
			m_spanBuffer[m_bufferCount].startX = startX >> 16;
			m_spanBuffer[m_bufferCount].endX = endX >> 16;
			startX += startDx;
			endX += endDx;

			if constexpr (hasTexture)
			{
				m_attrBuffer[m_bufferCount].s = attr.s;
				m_attrBuffer[m_bufferCount].t = attr.t;
				m_attrBuffer[m_bufferCount].w = attr.w;
				attr.s += deltaAttr.s;
				attr.t += deltaAttr.t;
				attr.w += deltaAttr.w;
			}

			m_bufferCount++;
		}
	};
}
