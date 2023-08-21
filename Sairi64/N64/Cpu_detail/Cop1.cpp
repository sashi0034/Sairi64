#include "stdafx.h"
#include "Cop1.h"
#include "Cop0.h"

namespace N64::Cpu_detail
{
	// https://github.com/SimoneN64/Kaizen/blob/d0bccfc7e7c0d6eaa3662e8286b9d2bf5888b74f/src/backend/core/registers/Cop1.hpp#L71
	void Cop1::SetFgr32(const Cop0& cop0, uint8 index, uint32 value)
	{
		auto status = cop0.Reg().status;
		if (status.Fr())
		{
			m_fgr[index].SetLo(value);
		}
		else
		{
			if (index & 0b1)
			{
				// 奇数レジスタにアクセスするときは、下の偶数レジスタのHIに書き込む(?)
				m_fgr[index & ~0b1].SetHi(value);
			}
			else
			{
				// 偶数レジスタにアクセスするときは、偶数レジスタのLOに書き込む
				m_fgr[index].SetLo(value);
			}
		}
	}

	void Cop1::SetFgr64(const Cop0& cop0, uint8 index, uint64 value)
	{
		auto status = cop0.Reg().status;
		if (status.Fr() == false)
		{
			index &= ~0b1;
		}
		m_fgr[index] = {value};
	}

	uint32 Cop1::GetFgr32(const Cop0& cop0, uint8 index) const
	{
		auto status = cop0.Reg().status;
		if (status.Fr())
		{
			return m_fgr[index].Lo();
		}
		else
		{
			if (index & 0b1)
			{
				// 奇数レジスタにアクセスするときは、下の偶数レジスタのHI
				return m_fgr[index & ~0b1].Hi();
			}
			else
			{
				// 偶数レジスタにアクセスするときは、そのレジスタのLO
				return m_fgr[index].Lo();
			}
		}
	}

	uint64 Cop1::GetFgr64(const Cop0& cop0, uint8 index) const
	{
		auto status = cop0.Reg().status;
		if (status.Fr() == false)
		{
			index &= ~0b1;
		}
		return m_fgr[index];
	}
}
