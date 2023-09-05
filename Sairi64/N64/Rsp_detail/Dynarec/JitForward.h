#pragma once
#include "N64/N64Logger.h"
#include "../Rsp_Process.h"
#include "N64/Instruction.h"

#define OFFSET_TO(type, base, target) (offsetof(type, target) - offsetof(type, base))

namespace N64::Rsp_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	using Process = Rsp::Process;

	struct AssembleState
	{
		uint16 recompiledLength;
		uint16 scanPc;
		bool scanningDelaySlot;
	};

	struct AssembleContext
	{
		N64System* n64;
		Rsp* rsp;
		x86::Assembler* x86Asm;
		asmjit::Label endLabel;
	};

	enum class DecodedToken
	{
		End,
		Continue,
		Branch,
	};

	constexpr uint8 ShiftAmount_LBV_SBV = 0;
	constexpr uint8 ShiftAmount_LSV_SSV = 1;
	constexpr uint8 ShiftAmount_LLV_SLV = 2;
	constexpr uint8 ShiftAmount_LDV_SDV = 3;
	constexpr uint8 ShiftAmount_LQV_SQV = 4;
	constexpr uint8 ShiftAmount_LRV_SRV = 4;
	constexpr uint8 ShiftAmount_LPV_SPV = 3;
	constexpr uint8 ShiftAmount_LUV_SUV = 3;
	constexpr uint8 ShiftAmount_LHV_SHV = 4;
	constexpr uint8 ShiftAmount_LFV_SFV = 4;
	constexpr uint8 ShiftAmount_LTV_STV = 4;
	constexpr uint8 ShiftAmount_SWV = 4;

	template <OpLwc2Funct funct>
	consteval uint8 Lwc2FunctShiftAmount()
	{
		switch (funct)
		{
		case OpLwc2Funct::LBV:
			return ShiftAmount_LBV_SBV;
		case OpLwc2Funct::LSV:
			return ShiftAmount_LSV_SSV;
		case OpLwc2Funct::LLV:
			return ShiftAmount_LLV_SLV;
		case OpLwc2Funct::LDV:
			return ShiftAmount_LDV_SDV;
		case OpLwc2Funct::LQV:
			return ShiftAmount_LQV_SQV;
		case OpLwc2Funct::LRV:
			return ShiftAmount_LRV_SRV;
		case OpLwc2Funct::LPV:
			return ShiftAmount_LPV_SPV;
		case OpLwc2Funct::LUV:
			return ShiftAmount_LUV_SUV;
		case OpLwc2Funct::LHV:
			return ShiftAmount_LHV_SHV;
		case OpLwc2Funct::LFV:
			return ShiftAmount_LFV_SFV;
		case OpLwc2Funct::LTV:
			return ShiftAmount_LTV_STV;
		case OpLwc2Funct::Invalid_0xFF: [[fallthrough]];
		default: ;
			throw std::invalid_argument("invalid LWC2");
		}
	}

	template <OpSwc2Funct funct>
	consteval uint8 Swc2FunctShiftAmount()
	{
		switch (funct)
		{
		case OpSwc2Funct::SBV:
			return ShiftAmount_LBV_SBV;
		case OpSwc2Funct::SSV:
			return ShiftAmount_LSV_SSV;
		case OpSwc2Funct::SLV:
			return ShiftAmount_LLV_SLV;
		case OpSwc2Funct::SDV:
			return ShiftAmount_LDV_SDV;
		case OpSwc2Funct::SQV:
			return ShiftAmount_LQV_SQV;
		case OpSwc2Funct::SRV:
			return ShiftAmount_LRV_SRV;
		case OpSwc2Funct::SPV:
			return ShiftAmount_LPV_SPV;
		case OpSwc2Funct::SUV:
			return ShiftAmount_LUV_SUV;
		case OpSwc2Funct::SHV:
			return ShiftAmount_LHV_SHV;
		case OpSwc2Funct::SFV:
			return ShiftAmount_LFV_SFV;
		case OpSwc2Funct::SWV:
			return ShiftAmount_SWV;
		case OpSwc2Funct::STV:
			return ShiftAmount_LTV_STV;
		case OpSwc2Funct::Invalid_0xFF: [[fallthrough]];
		default: ;
			throw std::invalid_argument("invalid LWC2");
		}
	}

	template <typename Instr> [[nodiscard]]
	static DecodedToken AssumeNotImplemented(const AssembleContext& ctx, Instr instr)
	{
		static void (*func)(Instr) = [](Instr instruction)
		{
			N64Logger::Abort(U"not implemented: instruction {}"_fmt(instruction.Stringify()));
		};

		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, instr.Raw());
		x86Asm.mov(x86::rax, func);
		x86Asm.call(x86::rax);
		return DecodedToken::End;
	}

	static void CallBreakPoint(const AssembleContext& ctx, uint64 code0 = 0)
	{
		static void (*func)(N64System& n64, uint64 code) = [](N64System& n64, uint64 code)
		{
			N64_TRACE(U"break point! n64={:016X}, code={}"_fmt(reinterpret_cast<uint64>(&n64), code));
		};

		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, ctx.n64);
		x86Asm.mov(x86::rdx, code0);
		x86Asm.mov(x86::rax, func);
		x86Asm.call(x86::rax);
	}
}
