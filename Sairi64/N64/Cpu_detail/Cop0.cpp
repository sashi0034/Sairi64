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

	uint64 Cop0::Read(uint8 number)
	{
		switch (static_cast<Cop0RegKind>(number))
		{
		case Cop0RegKind::Index:
			// TODO
			break;
		case Cop0RegKind::Random:
			break;
		case Cop0RegKind::EntryLo0:
			break;
		case Cop0RegKind::EntryLo1:
			break;
		case Cop0RegKind::Context:
			break;
		case Cop0RegKind::PageMask:
			break;
		case Cop0RegKind::Wired:
			break;
		case Cop0RegKind::Unused_7:
			break;
		case Cop0RegKind::BadVAddr:
			break;
		case Cop0RegKind::Count:
			break;
		case Cop0RegKind::EntryHi:
			break;
		case Cop0RegKind::Compare:
			break;
		case Cop0RegKind::Status:
			break;
		case Cop0RegKind::Cause:
			break;
		case Cop0RegKind::EPC:
			break;
		case Cop0RegKind::PRId:
			break;
		case Cop0RegKind::Config:
			break;
		case Cop0RegKind::LLAddr:
			break;
		case Cop0RegKind::WatchLo:
			break;
		case Cop0RegKind::WatchHi:
			break;
		case Cop0RegKind::XContext:
			break;
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
		case Cop0RegKind::ECC:
			break;
		case Cop0RegKind::CacheErr:
			break;
		case Cop0RegKind::TagLo:
			break;
		case Cop0RegKind::TagHi:
			break;
		case Cop0RegKind::ErrEPC:
			break;
		case Cop0RegKind::Unused_31:
			break;
		default: ;
		}

		N64Logger::Abort(U"read cop0 register: {}"_fmt(number));
		return 0;
	}
}
