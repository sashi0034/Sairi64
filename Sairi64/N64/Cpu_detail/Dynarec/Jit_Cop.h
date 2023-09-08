#pragma once
#include "Jit.h"

class N64::Cpu_detail::Dynarec::Jit::Cop
{
public:
	template <uint8 cop, OpCopSub sub> [[nodiscard]]
	static DecodedToken MFC_template(const AssembleContext& ctx, InstructionCopSub instr)
	{
		JIT_ENTRY;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;
		const uint8 rd = instr.Rd();
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::rdx, rd);
		if constexpr (sub == OpCopSub::MFC)
		{
			x86Asm.call(reinterpret_cast<uint64>(&readCop<cop, uint32>));
			x86Asm.movsxd(x86::rax, x86::eax);
		}
		else if constexpr (sub == OpCopSub::DMFC)
		{
			x86Asm.call(reinterpret_cast<uint64>(&readCop<cop, uint64>));
		}
		else static_assert(AlwaysFalseValue<OpCopSub, sub>);
		x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[rt])), x86::rax);
		return DecodedToken::Continue;
	}

	template <uint8 cop, OpCopSub sub> [[nodiscard]]
	static DecodedToken MTC_template(const AssembleContext& ctx, InstructionCopSub instr)
	{
		JIT_ENTRY;
		const uint8 rt = instr.Rt();
		const uint8 rd = instr.Rd();
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::rdx, rd);
		if (rt != 0)
		{
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[rt])));
			x86Asm.mov(x86::r8, x86::rax);
		}
		else
		{
			x86Asm.xor_(x86::r8, x86::r8);
		}
		if constexpr (sub == OpCopSub::MTC)
			x86Asm.call(reinterpret_cast<uint64>(&writeCop<cop, uint32>));
		else if constexpr (sub == OpCopSub::DMTC)
			x86Asm.call(reinterpret_cast<uint64>(&writeCop<cop, uint64>));
		else static_assert(AlwaysFalseValue<OpCopSub, sub>);
		return DecodedToken::Continue;
	}

	template <OpCop0TlbFunct funct> [[nodiscard]]
	static DecodedToken TLBW_template(const AssembleContext& ctx, InstructionCopSub instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, (uint64)&ctx.cpu->GetCop0());
		x86Asm.call(reinterpret_cast<uint64>(&helperTLBW<funct>));
		return DecodedToken::Continue; // TODO: PC参照先物理アドレスが変わるかもしれないので検証
	}

	template <Opcode op>
	static DecodedToken C1_loadStore(const AssembleContext& ctx, const AssembleState& state, InstructionFi instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& cpu = *ctx.cpu;
		const uint8 base = instr.Base();
		const sint64 offset = (sint64)static_cast<sint16>(instr.Offset());
		const auto resolvedLabel = x86Asm.newLabel();
		x86Asm.mov(x86::rcx, (uint64)ctx.n64); // rcx <- *n64
		x86Asm.mov(x86::rdx, (uint64)&cpu); // rdx <- *cpu
		if (base != 0)
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&cpu.GetGpr().Raw()[base])));
		else
			x86Asm.xor_(x86::rax, x86::rax);
		x86Asm.add(x86::rax, offset);
		x86Asm.mov(x86::r8, x86::rax); // r8 <- vaddr
		x86Asm.mov(x86::r9b, instr.Ft()); // r9b <- ft
		x86Asm.call(reinterpret_cast<uint64>(&helperC1_loadStore<op>));
		x86Asm.cmp(x86::al, 0);
		x86Asm.jne(resolvedLabel);
		// now, error occured
		x86Asm.mov(x86::rax, state.recompiledLength);
		x86Asm.jmp(ctx.endLabel);
		x86Asm.bind(resolvedLabel); // @resolved
		return DecodedToken::Continue;
	}

private:
	template <uint8 cop, typename Wire>
	N64_ABI static uint32 readCop(const Cpu& cpu, uint8 reg)
	{
		if constexpr (cop == 0 && std::same_as<Wire, uint32>)
		{
			return cpu.GetCop0().Read32(reg);
		}
		else if constexpr (cop == 0 && std::same_as<Wire, uint64>)
		{
			return cpu.GetCop0().Read64(reg);
		}
		else if constexpr (cop == 1 && std::same_as<Wire, uint32>)
		{
			return cpu.GetCop1().GetFgr32(cpu.GetCop0(), reg);
		}
		else if constexpr (cop == 1 && std::same_as<Wire, uint64>)
		{
			return cpu.GetCop1().GetFgr64(cpu.GetCop0(), reg);
		}
		else
		{
			static_assert(AlwaysFalse<Wire>);
			return {};
		}
	}

	template <uint8 cop, typename Wire>
	N64_ABI static void writeCop(Cpu& cpu, uint8 reg, uint32 value)
	{
		if constexpr (cop == 0 && std::same_as<Wire, uint32>)
		{
			cpu.GetCop0().Write32(reg, value);
		}
		else if constexpr (cop == 0 && std::same_as<Wire, uint64>)
		{
			cpu.GetCop0().Write64(reg, value);
		}
		else if constexpr (cop == 1 && std::same_as<Wire, uint32>)
		{
			cpu.GetCop1().SetFgr32(cpu.GetCop0(), reg, value);
		}
		else if constexpr (cop == 1 && std::same_as<Wire, uint64>)
		{
			cpu.GetCop1().SetFgr64(cpu.GetCop0(), reg, value);
		}
		else static_assert(AlwaysFalse<Wire>);
	}

	template <OpCop0TlbFunct funct>
	N64_ABI static void helperTLBW(Cop0& cop0)
	{
		if constexpr (funct == OpCop0TlbFunct::TLBWI)
		{
			cop0.GetTlb().WriteEntry(cop0.Reg().index.I());
		}
		else if constexpr (funct == OpCop0TlbFunct::TLBWR)
		{
			cop0.GetTlb().WriteEntry(cop0.WiredRandom());
		}
		else static_assert(AlwaysFalseValue<OpCop0TlbFunct, funct>);
	}

	template <Opcode op>
	N64_ABI static bool helperC1_loadStore(N64System& n64, Cpu& cpu, uint64 vaddr, uint8 ft)
	{
		constexpr BusAccess access = []() consteval
		{
			if constexpr (op == Opcode::LWC1 || op == Opcode::LDC1) return BusAccess::Load;
			else if constexpr (op == Opcode::SWC1 || op == Opcode::SDC1) return BusAccess::Store;
			else static_assert(AlwaysFalseValue<Opcode, op>);
		}();

		auto&& cop0 = cpu.GetCop0();
		if (cop0.Reg().status.Cu1() == false)
		[[unlikely]]
		{
			Process::ThrowException(cpu, ExceptionKinds::CoprocessorUnusable, 1);
			return false;
		}

		if (const auto paddr = Mmu::ResolveVAddr(cpu, vaddr))
		[[likely]]
		{
			if constexpr (op == Opcode::LWC1)
			{
				const uint32 value = Mmu::ReadPaddr32(n64, paddr.value());
				cpu.GetCop1().SetFgr32(cop0, ft, value);
			}
			else if constexpr (op == Opcode::LDC1)
			{
				const uint64 value = Mmu::ReadPaddr64(n64, paddr.value());
				cpu.GetCop1().SetFgr64(cop0, ft, value);
			}
			else if constexpr (op == Opcode::SWC1)
			{
				const uint32 value = cpu.GetCop1().GetFgr32(cop0, ft);
				Mmu::WritePaddr32(n64, paddr.value(), value);
			}
			else if constexpr (op == Opcode::SDC1)
			{
				const uint64 value = cpu.GetCop1().GetFgr64(cop0, ft);
				Mmu::WritePaddr64(n64, paddr.value(), value);
			}
			else static_assert(AlwaysFalseValue<Opcode, op>);
			return true;
		}
		else
		{
			cop0.HandleTlbException(vaddr);
			Process::ThrowException(cpu, cop0.GetTlbExceptionCode<access>(), 0);
			return false;
		}
	}
};
