#include "stdafx.h"
#include "Instruction.h"

namespace N64
{
	String Instruction::OpName() const
	{
		const auto op = StringifyEnum(Op());
		return op.isEmpty() ? U"op:{:06b}"_fmt(static_cast<uint8>(Op())) : std::move(op);
	}

	String Instruction::Stringify() const
	{
		return U"{} ({:08X}h)"_fmt(OpName(), Raw());
	}

	String InstructionI::Stringify() const
	{
		return U"{} rs:{}, rt:{}, imm:{:04X}h"_fmt(OpName(), Rs(), Rt(), Imm());
	}

	String InstructionRegimm::Stringify() const
	{
		return U"{} rs:{}, imm:{:04X}h"_fmt(StringifyEnum(Sub()), Rs(), Imm());
	}

	String InstructionJ::Stringify() const
	{
		return U"{} target:{:08X}"_fmt(OpName(), Target());
	}

	String InstructionR::Stringify() const
	{
		if (Raw() == 0) return U"NOP";

		return U"{} rs:{}, rt:{}, rd:{}, sa:{}"_fmt(StringifyEnum(Funct()), Rs(), Rt(), Rd(), Sa());
	}

	String InstructionCop::Stringify() const
	{
		return U"{} ({})"_fmt(StringifyEnum(Op()), StringifyEnum(Sub()));
	}

	String InstructionCopSub::Stringify() const
	{
		return U"{}{} rt:{}, rd:{}"_fmt(StringifyEnum(Sub()), CopNumber(), Rt(), Rd());
	}

	String InstructionCop1Sub::Stringify() const
	{
		return U"{}1 rt:{}, fs:{}"_fmt(StringifyEnum(Sub()), Rt(), Fs());
	}

	String InstructionCop0Tlb::Stringify() const
	{
		return U"{} ({:08X}h)"_fmt(StringifyEnum(Funct()), Raw());
	}

	String InstructionCop1Fmt::Stringify() const
	{
		return U"{} fmt:{}, ft:{}, fs:{}, fd:{}"_fmt(StringifyEnum(Funct()), StringifyEnum(Fmt()), Ft(), Fs(), Fd());
	}

	String InstructionCop2VecFunct::Stringify() const
	{
		return U"{} vt:{}, vs:{}, vd:{}, element:{}"_fmt(StringifyEnum(Funct()), Vt(), Vs(), Vd(), Element());
	}

	String InstructionCop2VecSub::Stringify() const
	{
		return U"{}2  vt:{}, vs:{}, element:{}"_fmt(StringifyEnum(Sub()), Vt(), Vs(), Element());
	}

	String InstructionFi::Stringify() const
	{
		return U"{} base:{}, ft:{}, offset:{:04X}h"_fmt(OpName(), Base(), Ft(), Offset());
	}

	template <typename InstrV>
	String stringifyV(InstrV instr)
	{
		return U"{} base:{}, vt:{}, element:{}, offset:{:02X}h"_fmt(
			StringifyEnum(instr.Funct()), instr.Base(), instr.Vt(), instr.Element(), instr.Offset());
	}

	String InstructionLv::Stringify() const
	{
		return stringifyV(*this);
	}

	String InstructionSv::Stringify() const
	{
		return stringifyV(*this);
	}
}
