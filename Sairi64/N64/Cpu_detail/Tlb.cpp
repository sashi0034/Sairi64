﻿#include "stdafx.h"
#include "Tlb.h"

#include "N64/N64Logger.h"

namespace N64::Cpu_detail
{
	void Tlb::WriteEntry(uint32 index)
	{
		PageMask32 pageMask = m_pageMask;

		const uint32 top = pageMask.Mask() & 0xAAA;
		pageMask.Mask().Set(top | (top >> 1));

		if (index >= 32)
		{
			N64Logger::Abort(U"invalid write tlb at {}"_fmt(index));
		}

		m_tlb[index].entryHi = m_entryHi;
		m_tlb[index].entryHi.Vpn2().Set(m_tlb[index].entryHi.Vpn2() & (~pageMask.Mask()));

		m_tlb[index].entryLo0 = m_entryLo0 & 0x03FFFFFE;
		m_tlb[index].entryLo1 = m_entryLo1 & 0x03FFFFFE;
		m_tlb[index].pageMask = pageMask;

		m_tlb[index].global = m_entryLo0.G() && m_entryLo1.G();
		m_tlb[index].initialized = true;
	}
}
