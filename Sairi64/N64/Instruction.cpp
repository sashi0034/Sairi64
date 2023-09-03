﻿#include "stdafx.h"
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
		return U"{} ({:08X})"_fmt(OpName(), Raw());
	}

	String InstructionI::Stringify() const
	{
		return U"{} rs:{}, rt:{}, imm:{:04X}"_fmt(OpName(), Rs(), Rt(), Imm());
	}

	String InstructionRegimm::Stringify() const
	{
		return U"{} rs:{}, imm:{:04X}"_fmt(StringifyEnum(Sub()), Rs(), Imm());
	}

	String InstructionR::Stringify() const
	{
		if (Raw() == 0) return U"NOP";

		return U"{} sa:{}, rd:{}, rt:{}, rs:{}"_fmt(StringifyEnum(Funct()), Sa(), Rd(), Rt(), Rs());
	}

	String InstructionCop::Stringify() const
	{
		return U"{} {}"_fmt(StringifyEnum(Op()), StringifyEnum(Sub()));
	}

	String InstructionCopSub::Stringify() const
	{
		return U"{}{} rt:{}, rd:{}"_fmt(StringifyEnum(Sub()), CopNumber(), Rt(), Rd());
	}

	String InstructionCop1Sub::Stringify() const
	{
		return U"{}{} rt:{}, fs:{}"_fmt(StringifyEnum(Sub()), CopNumber(), Rt(), Fs());
	}

	String InstructionCop0Co::Stringify() const
	{
		return U"{} ({:08X})"_fmt(StringifyEnum(Funct()), Raw());
	}

	String InstructionFi::Stringify() const
	{
		return U"{} ft:{}, offset:{:04X}, base:{}"_fmt(OpName(), Ft(), Offset(), Base());
	}
}