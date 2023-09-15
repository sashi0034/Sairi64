#pragma once
#include "Jit.h"
#include "../FloatingFmt.h"

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
	static DecodedToken TLBW_template(const AssembleContext& ctx, InstructionCop0Tlb instr)
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

	template <OpCop1BcBr br>
	static DecodedToken BC1_template(const AssembleContext& ctx, const AssembleState& state, InstructionCop1Bc instr)
	{
		JIT_ENTRY;
		constexpr BranchType branch = (br == OpCop1BcBr::BC1F) || (br == OpCop1BcBr::BC1T)
		                              ? BranchType::Normal
		                              : BranchType::Likely;
		constexpr bool trueCompare = (br == OpCop1BcBr::BC1T) || (br == OpCop1BcBr::BC1TL);
		auto&& x86Asm = *ctx.x86Asm;
		const auto validLabel = x86Asm.newLabel();
		const sint32 offset = static_cast<sint32>(static_cast<sint16>(instr.Offset())) * 4;

		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::edx, offset);
		x86Asm.call(reinterpret_cast<uint64>(&helperBC1_template<branch, trueCompare>));
		x86Asm.test(x86::al, x86::al); // if function was succeeded
		x86Asm.jne(validLabel); // then goto @valid
		x86Asm.mov(x86::rax, state.recompiledLength);
		x86Asm.jmp(ctx.endLabel);
		x86Asm.bind(validLabel); // @valid
		return branch == BranchType::Normal ?
		       DecodedToken::Branch :
		       DecodedToken::BranchLikely;
	}

	template <OpCop1FmtFunct funct, FloatingFmt fmt>
	static DecodedToken Fmt_move(const AssembleContext& ctx, InstructionCop1Fmt instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		using before = FloatingFmtType<fmt>::type;
		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::dl, instr.Fd());
		x86Asm.mov(x86::r8b, instr.Fs());
		if constexpr (
			funct == OpCop1FmtFunct::CvtSFmt || funct == OpCop1FmtFunct::CvtDFmt ||
			funct == OpCop1FmtFunct::CvtWFmt || funct == OpCop1FmtFunct::CvtLFmt)
		{
			using after = CvtTarget<funct>::type;
			x86Asm.call(reinterpret_cast<uint64>(&helperFmt_move<after, before, 1>));
		}
		else if constexpr (funct == OpCop1FmtFunct::MovFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperFmt_move<before, before, 1>));
		}
		else if constexpr (funct == OpCop1FmtFunct::NegFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperFmt_move<before, before, -1>));
		}
		else static_assert(AlwaysFalseValue<OpCop1FmtFunct, funct>);
		return DecodedToken::Continue;
	}

	template <OpCop1FmtFunct funct, FloatingFmt fmt>
	static DecodedToken Fmt_mathf(const AssembleContext& ctx, InstructionCop1Fmt instr)
	{
		JIT_ENTRY;
		using fmt_t = FloatingFmtType<fmt>::type;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::dl, instr.Fd());
		x86Asm.mov(x86::r8b, instr.Fs());
		if constexpr (funct == OpCop1FmtFunct::SqrtFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperSqrtFmt<fmt_t>));
		}
		else if constexpr (funct == OpCop1FmtFunct::AbsFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperAbsFmt<fmt_t>));
		}
		else if constexpr (funct == OpCop1FmtFunct::RoundWFmt || funct == OpCop1FmtFunct::RoundLFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperFmt_mathfRounding<funct, fmt_t>));
		}
		else if constexpr (funct == OpCop1FmtFunct::TruncWFmt || funct == OpCop1FmtFunct::TruncLFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperFmt_mathfRounding<funct, fmt_t>));
		}
		else if constexpr (funct == OpCop1FmtFunct::CeilWFmt || funct == OpCop1FmtFunct::CeilLFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperFmt_mathfRounding<funct, fmt_t>));
		}
		else if constexpr (funct == OpCop1FmtFunct::FloorWFmt || funct == OpCop1FmtFunct::FloorLFmt)
		{
			x86Asm.call(reinterpret_cast<uint64>(&helperFmt_mathfRounding<funct, fmt_t>));
		}
		else static_assert(AlwaysFalseValue<OpCop1FmtFunct, funct>);
		return DecodedToken::Continue;
	}

	template <OpCop1FmtFunct funct, FloatingFmt fmt>
	static DecodedToken Fmt_arithmetic(const AssembleContext& ctx, const AssembleState& state, InstructionCop1Fmt instr)
	{
		JIT_ENTRY;
		if constexpr (fmt == FloatingFmt::Word || fmt == FloatingFmt::Long)
		{
			return AssumeNotImplemented(ctx, instr);
		}
		else
		{
			return formatArithmeticInternal<funct, fmt>(ctx, state, instr);
		}
	}

	template <OpCop1FmtFunct funct, FloatingFmt fmt>
	static DecodedToken CondFmt_template(
		const AssembleContext& ctx, const AssembleState& state, InstructionCop1Fmt instr)
	{
		JIT_ENTRY;
		if constexpr (fmt == FloatingFmt::Word || fmt == FloatingFmt::Long)
		{
			return AssumeNotImplemented(ctx, instr);
		}
		auto&& x86Asm = *ctx.x86Asm;
		using floating = FloatingFmtType<fmt>::type;
		const auto validLabel = x86Asm.newLabel();

		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::dl, instr.Fs());
		x86Asm.mov(x86::r8b, instr.Ft());
		if constexpr (fmt == FloatingFmt::Single || fmt == FloatingFmt::Double)
			x86Asm.call(reinterpret_cast<uint64>(&helperCondFmt_template<funct, floating>));
		x86Asm.test(x86::al, x86::al); // if function was succeeded
		x86Asm.jne(validLabel); // then goto @valid
		x86Asm.mov(x86::rax, state.recompiledLength);
		x86Asm.jmp(ctx.endLabel);
		x86Asm.bind(validLabel); // @valid
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

	template <typename After, typename Before, sint8 sign>
	N64_ABI static void helperFmt_move(Cpu& cpu, uint8 fd, uint8 fs)
	{
		static_assert(sign == 1 || sign == -1);
		auto&& cop1 = cpu.GetCop1();
		auto&& cop0 = cpu.GetCop0();

		if constexpr (std::same_as<Before, uint32>)
		{
			cop1.SetFgrBy<After>(cop0, fd, static_cast<sint32>(cop1.GetFgrBy<Before>(cop0, fs)) * sign);
		}
		else if constexpr (std::same_as<Before, uint64>)
		{
			cop1.SetFgrBy<After>(cop0, fd, static_cast<sint64>(cop1.GetFgrBy<Before>(cop0, fs)) * sign);
		}
		else
		{
			cop1.SetFgrBy<After>(cop0, fd, cop1.GetFgrBy<Before>(cop0, fs) * sign);
		}
	}

	template <typename Fmt>
	N64_ABI static void helperSqrtFmt(Cpu& cpu, uint8 fd, uint8 fs)
	{
		if constexpr (std::is_floating_point<Fmt>::value)
		{
			auto&& cop1 = cpu.GetCop1();
			auto&& cop0 = cpu.GetCop0();

			const Fmt fsF = cop1.GetFgrBy<Fmt>(cop0, fs);
			cop1.SetFgrBy<Fmt>(cop0, fd, std::sqrt(fsF));
		}
		else N64Logger::Abort();
	}

	template <typename Fmt>
	N64_ABI static void helperAbsFmt(Cpu& cpu, uint8 fd, uint8 fs)
	{
		auto&& cop1 = cpu.GetCop1();
		auto&& cop0 = cpu.GetCop0();

		if constexpr (std::same_as<Fmt, uint32>)
		{
			const sint32 fsF = cop1.GetFgrBy<sint32>(cop0, fs);
			cop1.SetFgrBy<uint32>(cop0, fd, std::abs(fsF));
		}
		else if constexpr (std::same_as<Fmt, uint64>)
		{
			const sint64 fsF = cop1.GetFgrBy<sint64>(cop0, fs);
			cop1.SetFgrBy<uint64>(cop0, fd, std::abs(fsF));
		}
		else
		{
			const Fmt fsF = cop1.GetFgrBy<Fmt>(cop0, fs);
			cop1.SetFgrBy<Fmt>(cop0, fd, std::abs(fsF));
		}
	}

	template <OpCop1FmtFunct funct, typename Fmt>
	N64_ABI static void helperFmt_mathfRounding(Cpu& cpu, uint8 fd, uint8 fs)
	{
		if constexpr (std::is_floating_point<Fmt>::value)
		{
			auto&& cop1 = cpu.GetCop1();
			auto&& cop0 = cpu.GetCop0();

			const Fmt fsF = cop1.GetFgrBy<Fmt>(cop0, fs);
			if constexpr (funct == OpCop1FmtFunct::RoundWFmt)
				cop1.SetFgrBy<uint32>(cop0, fd, static_cast<sint32>(std::nearbyint(fsF)));
			else if constexpr (funct == OpCop1FmtFunct::RoundLFmt)
				cop1.SetFgrBy<uint64>(cop0, fd, static_cast<sint64>(std::nearbyint(fsF)));
			else if constexpr (funct == OpCop1FmtFunct::TruncWFmt)
				cop1.SetFgrBy<uint32>(cop0, fd, static_cast<sint32>(std::trunc(fsF)));
			else if constexpr (funct == OpCop1FmtFunct::TruncLFmt)
				cop1.SetFgrBy<uint64>(cop0, fd, static_cast<sint64>(std::trunc(fsF)));
			else if constexpr (funct == OpCop1FmtFunct::CeilWFmt)
				cop1.SetFgrBy<uint32>(cop0, fd, static_cast<sint32>(std::ceil(fsF)));
			else if constexpr (funct == OpCop1FmtFunct::CeilLFmt)
				cop1.SetFgrBy<uint64>(cop0, fd, static_cast<sint64>(std::ceil(fsF)));
			else if constexpr (funct == OpCop1FmtFunct::FloorWFmt)
				cop1.SetFgrBy<uint32>(cop0, fd, static_cast<sint32>(std::floor(fsF)));
			else if constexpr (funct == OpCop1FmtFunct::FloorLFmt)
				cop1.SetFgrBy<uint64>(cop0, fd, static_cast<sint64>(std::floor(fsF)));
			else static_assert(AlwaysFalse<Fmt>);
		}
		else N64Logger::Abort();
	}

	template <OpCop1FmtFunct funct, FloatingFmt fmt>
	static DecodedToken formatArithmeticInternal(
		const AssembleContext& ctx, const AssembleState& state, InstructionCop1Fmt instr)
	{
		auto&& x86Asm = *ctx.x86Asm;
		using floating = FloatingFmtType<fmt>::type;
		const auto validLabel = x86Asm.newLabel();

		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::dl, instr.Fd());
		x86Asm.mov(x86::r8b, instr.Fs());
		x86Asm.mov(x86::r9b, instr.Ft());
		x86Asm.call(reinterpret_cast<uint64>(&helperFmt_arithmetic<funct, floating>));
		x86Asm.test(x86::al, x86::al); // if function was succeeded
		x86Asm.jne(validLabel); // then goto @valid
		x86Asm.mov(x86::rax, state.recompiledLength);
		x86Asm.jmp(ctx.endLabel);
		x86Asm.bind(validLabel); // @valid
		return DecodedToken::Continue;
	}

	// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/cpu/r4300i.h#L720
	[[nodiscard]]
	static bool checkCp1(Cpu& cpu)
	{
		if (cpu.GetCop0().Reg().status.Cu1() == false)
		{
			Process::ThrowException(cpu, ExceptionKinds::CoprocessorUnusable, 1);
			return false;
		}
		cpu.GetCop1().Fcr().fcr31.Cause().Set(0);
		return true;
	}

	[[nodiscard]]
	static bool handleQNaN(Cpu& cpu, Cop1& cop1)
	{
		cop1.Fcr().fcr31.CauseInvalidOperation().Set(true);
		if (cop1.Fcr().fcr31.EnableInvalidOperation() == false)
		{
			cop1.Fcr().fcr31.FlagInvalidOperation().Set(true);
			return true;
		}
		else
		{
			Process::ThrowException(cpu, ExceptionKinds::FloatingPoint, 1);
			return false;
		}
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/fpu_instructions.c#L46
	template <typename T> [[nodiscard]]
	static inline bool checkValidFloating(Cpu& cpu, Cop1& cop1, T f)
	{
		if constexpr (std::same_as<T, float> || std::same_as<T, double>)
		{
			// TODO: これではまだ不十分かもしれないので検証
			switch (std::fpclassify(f))
			{
			case FP_NAN:
				if (IsQNaN(f)) return handleQNaN(cpu, cop1);
				[[fallthrough]];
			case FP_SUBNORMAL:
				cop1.Fcr().fcr31.CauseUnimplementedOperation().Set(true);
				Process::ThrowException(cpu, ExceptionKinds::FloatingPoint, 1); // 未実装操作は必ず割り込みが入るらしい
				return false;
			default: ;
			}
		}
		return true;
	}

	template <typename T> [[nodiscard]]
	static inline bool checkValidFloating(Cpu& cpu, Cop1& cop1, T fs, T ft)
	{
		if (checkValidFloating<T>(cpu, cop1, fs) == false) return false;
		if (checkValidFloating<T>(cpu, cop1, ft) == false) return false;
		return true;
	}

	template <OpCop1FmtFunct funct, typename Fmt>
	N64_ABI static bool helperFmt_arithmetic(Cpu& cpu, uint8 fd, uint8 fs, uint8 ft)
	{
		auto&& cop1 = cpu.GetCop1();
		auto&& cop0 = cpu.GetCop0();

		const Fmt fsF = cop1.GetFgrBy<Fmt>(cop0, fs);
		const Fmt ftF = cop1.GetFgrBy<Fmt>(cop0, ft);

		if (checkValidFloating(cpu, cop1, fsF, ftF) == false) return false;

		if constexpr (funct == OpCop1FmtFunct::AddFmt)
		{
			cop1.SetFgrBy<Fmt>(cop0, fd, fsF + ftF);
		}
		else if constexpr (funct == OpCop1FmtFunct::SubFmt)
		{
			cop1.SetFgrBy<Fmt>(cop0, fd, fsF - ftF);
		}
		else if constexpr (funct == OpCop1FmtFunct::MulFmt)
		{
			cop1.SetFgrBy<Fmt>(cop0, fd, fsF * ftF);
		}
		else if constexpr (funct == OpCop1FmtFunct::DivFmt)
		{
			cop1.SetFgrBy<Fmt>(cop0, fd, fsF / ftF);
		}
		else static_assert(AlwaysFalse<Fmt>);

		return true;
	}

	// TODO: いろいろ要るかも
	// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/cpu/fpu_instructions.c#L858
	// https://github.com/SimoneN64/Kaizen/blob/9f14d2421bf3644e0b323eff1db8d012c3a27a73/src/backend/core/registers/cop/cop1instructions.cpp#L254
	template <OpCop1FmtFunct funct, typename Fmt>
	N64_ABI static bool helperCondFmt_template(Cpu& cpu, uint8 fs, uint8 ft)
	{
		auto&& cop1 = cpu.GetCop1();
		auto&& cop0 = cpu.GetCop0();

		Fmt fsF = cop1.GetFgrBy<Fmt>(cop0, fs);
		Fmt ftF = cop1.GetFgrBy<Fmt>(cop0, ft);

		if constexpr (
			funct == OpCop1FmtFunct::CondSfFmt ||
			funct == OpCop1FmtFunct::CondNgleFmt ||
			funct == OpCop1FmtFunct::CondSeqFmt ||
			funct == OpCop1FmtFunct::CondNglFmt ||
			funct == OpCop1FmtFunct::CondLtFmt ||
			funct == OpCop1FmtFunct::CondNgeFmt ||
			funct == OpCop1FmtFunct::CondLeFmt ||
			funct == OpCop1FmtFunct::CondNgtFmt
		)
		{
			if (std::isnan(fsF) || std::isnan(ftF))
			{
				cop1.Fcr().fcr31.FlagInvalidOperation().Set(true);
				cop1.Fcr().fcr31.CauseInvalidOperation().Set(true);
				Process::ThrowException(cpu, ExceptionKinds::FloatingPoint, 1);
				return false;
			}
		}

		const bool condition = [fsF, ftF]()
		{
			if constexpr (funct == OpCop1FmtFunct::CondFFmt || funct == OpCop1FmtFunct::CondSfFmt)
				return false;
			else if constexpr (funct == OpCop1FmtFunct::CondUnFmt || funct == OpCop1FmtFunct::CondNgleFmt)
				return std::isnan(fsF) || std::isnan(ftF);
			else if constexpr (funct == OpCop1FmtFunct::CondEqFmt || funct == OpCop1FmtFunct::CondSeqFmt)
				return fsF == ftF;
			else if constexpr (funct == OpCop1FmtFunct::CondUeqFmt || funct == OpCop1FmtFunct::CondNglFmt)
				return (std::isnan(fsF) || std::isnan(fsF)) || (fsF == fsF);
			else if constexpr (funct == OpCop1FmtFunct::CondOltFmt || funct == OpCop1FmtFunct::CondLtFmt)
				return (!std::isnan(fsF) && !std::isnan(ftF)) && (fsF < ftF);
			else if constexpr (funct == OpCop1FmtFunct::CondUltFmt || funct == OpCop1FmtFunct::CondNgeFmt)
				return (std::isnan(fsF) || std::isnan(ftF)) || (fsF < ftF);
			else if constexpr (funct == OpCop1FmtFunct::CondOleFmt || funct == OpCop1FmtFunct::CondLeFmt)
				return (!std::isnan(fsF) && !std::isnan(ftF)) && (fsF <= ftF);
			else if constexpr (funct == OpCop1FmtFunct::CondUleFmt || funct == OpCop1FmtFunct::CondNgtFmt)
				return (std::isnan(fsF) || std::isnan(ftF)) || (fsF <= ftF);
			else
			{
				static_assert(AlwaysFalseValue<OpCop1FmtFunct, funct>);
				return {};
			}
		}();

		cop1.Fcr().fcr31.Compare().Set(condition);
		return true;
	}

	template <BranchType branch, bool trueCompare>
	N64_ABI static bool helperBC1_template(Cpu& cpu, sint32 offset)
	{
		if (checkCp1(cpu) == false) return false;

		const bool condition = trueCompare
		                       ? cpu.GetCop1().Fcr().fcr31.Compare()
		                       : cpu.GetCop1().Fcr().fcr31.Compare() == false;
		Process::BranchVAddr64<branch>(cpu, cpu.GetPc().Curr() + offset, condition);
		return true;
	}
};
