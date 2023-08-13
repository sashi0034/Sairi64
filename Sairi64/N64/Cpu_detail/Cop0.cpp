#include "stdafx.h"
#include "Cop0.h"

#include "N64/N64Logger.h"

namespace N64::Cpu_detail
{
	Cop0::Cop0()
	{
		m_reg.cause = 0xB000007C;
		// https://github.com/project64/project64/blob/353ef5ed897cb72a8904603feddbdc649dff9eca/Source/Project64-core/N64System/N64System.cpp#L855
		// m_reg.cause.Ip4().Set(1); // TODO: ?

		// https://github.com/SimoneN64/Kaizen/blob/dffd36fc31731a0391a9b90f88ac2e5ed5d3f9ec/src/backend/core/registers/Cop0.cpp#L11
		m_reg.status = 0;
		m_reg.status.Cu0().Set(1);
		m_reg.status.Cu1().Set(1);
		m_reg.status.Fr().Set(1);
		m_reg.prId = 0x00000B22;
		m_reg.config = 0x7006E463;
		m_reg.epc = 0xFFFFFFFFFFFFFFFF;
		m_reg.errorEpc = 0xFFFFFFFFFFFFFFFF;
		m_reg.wired = 0;
		m_reg.index = 63;
		m_reg.badVAddr = 0xFFFFFFFFFFFFFFFF;
	}

	template <typename Wire>
	Wire Cop0::readInternal(uint8 number) const
	{
		constexpr bool wire64 = std::is_same<Wire, uint64_t>::value;
		constexpr bool wire32 = std::is_same<Wire, uint32_t>::value;
		static_assert(wire64 || wire32);

		switch (static_cast<Cop0RegKind>(number))
		{
		case Cop0RegKind::Index:
			if constexpr (wire32) return m_reg.index & 0x8000003F;
			else break;
		case Cop0RegKind::Random:
			if constexpr (wire32) return Random(m_reg.wired, static_cast<uint32>(31));
			else break;
		case Cop0RegKind::EntryLo0:
			return m_reg.entryLo0;
		case Cop0RegKind::EntryLo1:
			return m_reg.entryLo1;
		case Cop0RegKind::Context:
			return m_reg.context;
		case Cop0RegKind::PageMask:
			if constexpr (wire32) return m_reg.pageMask;
			else break;
		case Cop0RegKind::Wired:
			if constexpr (wire32) return m_reg.wired;
			else break;
		case Cop0RegKind::Unused_7:
			break;
		case Cop0RegKind::BadVAddr:
			return m_reg.badVAddr;
		case Cop0RegKind::Count:
			if constexpr (wire32) return m_reg.count;
			else break;
		case Cop0RegKind::EntryHi:
			return m_reg.entryHi;
		case Cop0RegKind::Compare:
			if constexpr (wire32) return m_reg.compare;
			else break;
		case Cop0RegKind::Status:
			return m_reg.status;
		case Cop0RegKind::Cause:
			if constexpr (wire32) return m_reg.cause;
			else break;
		case Cop0RegKind::EPC:
			return m_reg.epc;
		case Cop0RegKind::PRId:
			return m_reg.prId;
		case Cop0RegKind::Config:
			if constexpr (wire32) return m_reg.config;
			else break;
		case Cop0RegKind::LLAddr:
			return m_reg.llAddr;
		case Cop0RegKind::WatchLo:
			if constexpr (wire32) return m_reg.watchLo;
			else break;
		case Cop0RegKind::WatchHi:
			if constexpr (wire32) return m_reg.watchHi;
			break;
		case Cop0RegKind::XContext:
			if constexpr (wire32) return m_reg.xContext;
			else if constexpr (wire64) return m_reg.xContext & 0xFFFFFFFFFFFFFFF0;
			else break;
		case Cop0RegKind::Unused_21:
			break;
		case Cop0RegKind::Unused_22:
			break;
		case Cop0RegKind::Unused_23:
			break;
		case Cop0RegKind::Unused_24:
			break;
		case Cop0RegKind::Unused_25:
			break;
		case Cop0RegKind::ParityError:
			if constexpr (wire32) return m_reg.parityError;
			else break;
		case Cop0RegKind::CacheError:
			if constexpr (wire32) return m_reg.cacheError;
			else break;
		case Cop0RegKind::TagLo:
			if constexpr (wire32) return m_reg.tagLo;
			else break;
		case Cop0RegKind::TagHi:
			if constexpr (wire32) return m_reg.tagHi;
			else break;
		case Cop0RegKind::ErrorEPC:
			return m_reg.errorEpc;
		case Cop0RegKind::Unused_31:
			break;
		default: ;
		}

		N64Logger::Abort(U"cop0 unsupported read {}-bit: {}"_fmt(
			static_cast<int>(std::numeric_limits<Wire>::digits), number));
		return 0;
	}

	constexpr uint64 entryLoMask_0x3FFFFFFF = 0x3FFFFFFF;
	constexpr uint64 entryHiMask_0xC00000FFFFFFE0FF = 0xC00000FFFFFFE0FF;
	constexpr uint64 pageMaskMask_0x1FFE000 = 0x1FFE000;
	constexpr uint64 statusMask_0xFF57FFFF = 0xFF57FFFF;
	constexpr uint64 configMask_0x0F00800F = 0x0F00800F;

	template <typename Wire>
	void Cop0::writeInternal(uint8 number, Wire value)
	{
		constexpr bool wire64 = std::is_same<Wire, uint64_t>::value;
		constexpr bool wire32 = std::is_same<Wire, uint32_t>::value;
		static_assert(wire64 || wire32);

		switch (static_cast<Cop0RegKind>(number))
		{
		case Cop0RegKind::Index:
			if constexpr (wire32)
			{
				m_reg.index = value;
				return;
			}
			else break;
		case Cop0RegKind::Random:
			break;
		case Cop0RegKind::EntryLo0:
			m_reg.entryLo0 = value & entryLoMask_0x3FFFFFFF;
			return;
		case Cop0RegKind::EntryLo1:
			m_reg.entryLo1 = value & entryLoMask_0x3FFFFFFF;
			return;
		case Cop0RegKind::Context:
			if constexpr (wire32)
				m_reg.context = ((sint64)static_cast<sint32>(value) & 0xFFFFFFFFFF800000) | (m_reg.context & 0x7FFFFF);
			else if constexpr (wire64)
				m_reg.context = (value & 0xFFFFFFFFFF800000) | (m_reg.context & 0x7FFFFF);
			return;
		case Cop0RegKind::PageMask:
			if constexpr (wire32)
			{
				m_reg.pageMask = value & pageMaskMask_0x1FFE000;
				return;
			}
			else break;
		case Cop0RegKind::Wired:
			if constexpr (wire32)
			{
				m_reg.wired = value & 63;
				return;
			}
			else break;
		case Cop0RegKind::Unused_7:
			break;
		case Cop0RegKind::BadVAddr:
			break;
		case Cop0RegKind::Count:
			if constexpr (wire32)
			{
				m_reg.count = static_cast<uint64>(value) << 1;
				return;
			}
			else break;
		case Cop0RegKind::EntryHi:
			if constexpr (wire32)
				m_reg.entryHi = ((sint64)static_cast<sint32>(value)) & entryHiMask_0xC00000FFFFFFE0FF;
			if constexpr (wire64)
				m_reg.entryHi = value & entryHiMask_0xC00000FFFFFFE0FF;
			return;
		case Cop0RegKind::Compare:
			if constexpr (wire32)
			{
				m_reg.compare = value;
				m_reg.cause.Ip7().Set(false);
				return;
			}
			else break;
		case Cop0RegKind::Status:
			if constexpr (wire32)
			{
				m_reg.status = static_cast<uint32>(m_reg.status) & (~statusMask_0xFF57FFFF);
				m_reg.status = m_reg.status | (value & statusMask_0xFF57FFFF);
			}
			else if constexpr (wire64)
			{
				m_reg.status = value;
			}
			return;
		case Cop0RegKind::Cause:
			m_reg.cause.Ip0().Set(Cop0Cause32(value).Ip0());
			m_reg.cause.Ip1().Set(Cop0Cause32(value).Ip1());
			return;
		case Cop0RegKind::EPC:
			if constexpr (wire32)
				m_reg.epc = (sint64)static_cast<sint32>(value);
			if constexpr (wire64)
				m_reg.epc = static_cast<sint64>(value);
			return;
		case Cop0RegKind::PRId:
			break;
		case Cop0RegKind::Config:
			if constexpr (wire32)
			{
				m_reg.config &= configMask_0x0F00800F;
				m_reg.config |= (value & configMask_0x0F00800F);
				return;
			}
			else break;
		case Cop0RegKind::LLAddr:
			m_reg.llAddr = value;
			return;
		case Cop0RegKind::WatchLo:
			if constexpr (wire32)
			{
				m_reg.watchLo = value;
				return;
			}
			else break;
		case Cop0RegKind::WatchHi:
			if constexpr (wire32)
			{
				m_reg.watchHi = value;
				return;
			}
			else break;
		case Cop0RegKind::XContext:
			if constexpr (wire32)
				m_reg.xContext =
					(sint64)static_cast<sint32>(value & 0xFFFFFFFE00000000) | (m_reg.xContext & 0x1FFFFFFFF);
			if constexpr (wire64)
				m_reg.xContext = (value & 0xFFFFFFFE00000000) | (m_reg.xContext & 0x1FFFFFFFF);
			return;
		case Cop0RegKind::Unused_21:
			break;
		case Cop0RegKind::Unused_22:
			break;
		case Cop0RegKind::Unused_23:
			break;
		case Cop0RegKind::Unused_24:
			break;
		case Cop0RegKind::Unused_25:
			break;
		case Cop0RegKind::ParityError:
			if constexpr (wire32)
			{
				m_reg.parityError = value & 0xFF;
				return;
			}
			else break;
		case Cop0RegKind::CacheError:
			break;
		case Cop0RegKind::TagLo:
			if constexpr (wire32)
			{
				m_reg.tagLo = value;
				return;
			}
			else break;
		case Cop0RegKind::TagHi:
			if constexpr (wire32)
			{
				m_reg.tagHi = value;
				return;
			}
			else break;
		case Cop0RegKind::ErrorEPC:
			if constexpr (wire32)
				m_reg.errorEpc = (sint64)static_cast<sint32>(value);
			if constexpr (wire64)
				m_reg.errorEpc = static_cast<sint64>(value);
			return;
		case Cop0RegKind::Unused_31:
			break;
		default: ;
		}

		N64Logger::Abort(U"cop0 unsupported write {}-bit: {}"_fmt(
			static_cast<int>(std::numeric_limits<Wire>::digits), number));
	}

	uint64 Cop0::Read64(uint8 number) const
	{
		return readInternal<uint64>(number);
	}

	uint32 Cop0::Read32(uint8 number) const
	{
		return readInternal<uint32>(number);
	}

	void Cop0::Write64(uint8 number, uint64 value)
	{
		writeInternal<uint64>(number, value);
	}

	void Cop0::Write32(uint8 number, uint32 value)
	{
		writeInternal<uint32>(number, value);
	}
}
