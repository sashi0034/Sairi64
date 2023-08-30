#include "stdafx.h"
#include "GprMapper.h"

#include "N64/Cpu_detail/Gpr.h"

namespace N64::Cpu_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	const x86::Gpq& indexToGpq(uint8 index)
	{
		// x86_64の不揮発性レジスタに対応させる
		// https://learn.microsoft.com/ja-jp/windows-hardware/drivers/debugger/x64-architecture#calling-conventions
		static constexpr std::array<x86::Gpq, GpAvailableSize_8> map{
			x86::rbx,
			x86::rbp,
			x86::rdi,
			x86::rsi,
			x86::r12,
			x86::r13,
			x86::r14,
			x86::r15,
		};
		return map[index];
	}

	GprMapper::GprMapper()
	{
		m_gpMap.fill(-1);
	}

	void GprMapper::PushNonVolatiles(x86::Assembler& x86Asm)
	{
		for (int i = 0; i < GpAvailableSize_8; ++i)
		{
			x86Asm.push(indexToGpq(i));
		}
	}

	void GprMapper::PopNonVolatiles(x86::Assembler& x86Asm)
	{
		for (int i = GpAvailableSize_8 - 1; i >= 0; --i)
		{
			x86Asm.pop(indexToGpq(i));
		}
	}

	const x86::Gpq& GprMapper::assignNew(x86::Assembler& x86Asm, Gpr& gpr, uint8 rd)
	{
		auto&& nextGp = indexToGpq(m_gpStackCount);
		m_gpStack[m_gpStackCount] = rd;
		m_gpStackCount++;
		m_gpMap[rd] = nextGp.id();

		x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64_t>(&gpr.Raw()[rd])));
		x86Asm.mov(nextGp, x86::rax);

		return nextGp;
	}

	x86::Gpq GprMapper::AssignMap(x86::Assembler& x86Asm, Gpr& gpr, uint8 rd)
	{
		// 既に登場したレジスタなので以前のものを利用
		if (m_gpMap[rd] != -1) return x86::Gpq(m_gpMap[rd]);

		// 手元のレジスタが枯渇したので一旦クリア
		if (m_gpStackCount + 1 >= GpAvailableSize_8) FlushClear(x86Asm, gpr);

		// 新規割り当て
		return assignNew(x86Asm, gpr, rd);
	}

	std::pair<x86::Gpq, x86::Gpq> GprMapper::AssignMap(x86::Assembler& x86Asm, Gpr& gpr, uint8 rd, uint8 rs)
	{
		// 新しく割り当てるレジスタの数を計算し、枯渇するなら一旦クリア
		const int appendCount = (m_gpMap[rd] != -1 ? 0 : 1) + (m_gpMap[rs] != -1 ? 0 : 1);
		if (m_gpStackCount + appendCount >= GpAvailableSize_8) FlushClear(x86Asm, gpr);

		// 割り当て
		std::pair<x86::Gpq, x86::Gpq> result;
		result.first = m_gpMap[rd] != -1
			               ? x86::Gpq(m_gpMap[rd])
			               : assignNew(x86Asm, gpr, rd);
		result.second = m_gpMap[rs] != -1
			                ? x86::Gpq(m_gpMap[rs])
			                : assignNew(x86Asm, gpr, rs);
		return result;
	}

	std::tuple<x86::Gpq, x86::Gpq, x86::Gpq> GprMapper::AssignMap(
		x86::Assembler& x86Asm, Gpr& gpr, uint8 rd, uint8 rs, uint8 rt)
	{
		// 新しく割り当てるレジスタの数を計算し、枯渇するなら一旦クリア
		const int appendCount = (m_gpMap[rd] != -1 ? 0 : 1) + (m_gpMap[rs] != -1 ? 0 : 1) + (m_gpMap[rt] != -1 ? 0 : 1);
		if (m_gpStackCount + appendCount >= GpAvailableSize_8) FlushClear(x86Asm, gpr);

		// 割り当て
		std::tuple<x86::Gpq, x86::Gpq, x86::Gpq> result;
		std::get<0>(result) = m_gpMap[rd] != -1
			                      ? x86::Gpq(m_gpMap[rd])
			                      : assignNew(x86Asm, gpr, rd);
		std::get<1>(result) = m_gpMap[rs] != -1
			                      ? x86::Gpq(m_gpMap[rs])
			                      : assignNew(x86Asm, gpr, rs);
		std::get<2>(result) = m_gpMap[rt] != -1
			                      ? x86::Gpq(m_gpMap[rt])
			                      : assignNew(x86Asm, gpr, rt);
		return result;
	}

	void GprMapper::FlushClear(x86::Assembler& x86Asm, Gpr& gpr)
	{
		for (int i = 0; i < m_gpStackCount; ++i)
		{
			const uint8 gprId = m_gpStack[i];
			x86Asm.mov(x86::rax, (uint64)(&gpr.Raw()[gprId]));
			x86Asm.mov(x86::qword_ptr(x86::rax), x86::gpq(m_gpMap[gprId]));
			m_gpMap[gprId] = -1;
		}
		m_gpStackCount = 0;
	}
}
