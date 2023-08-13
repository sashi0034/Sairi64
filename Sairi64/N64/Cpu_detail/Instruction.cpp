﻿#include "stdafx.h"
#include "Instruction.h"

#include "Gpr.h"

#define RETURN_GPR_NAME(gpr) return GprNames[gpr()];

namespace N64::Cpu_detail
{
	String Instruction::OpName() const
	{
		const auto op = StringifyEnum(Op());
		return op.isEmpty() ? U"op:{:06b}"_fmt(static_cast<uint8>(Op())) : std::move(op);
	}

	StringView InstructionR::RdName() const
	{
		RETURN_GPR_NAME(Rd)
	}

	StringView InstructionR::RtName() const
	{
		RETURN_GPR_NAME(Rt)
	}

	StringView InstructionR::RsName() const
	{
		RETURN_GPR_NAME(Rs)
	}

	String InstructionR::Stringify() const
	{
		if (Raw() == 0) return U"NOP";

		return U"{} sa:{}, rd:{}, rt:{}, rs:{}"_fmt(StringifyEnum(Funct()), Sa(), RdName(), RtName(), RsName());
	}

	String InstructionCop::Stringify() const
	{
		return U"{} {}"_fmt(StringifyEnum(Op()), StringifyEnum(Sub()));
	}

	StringView InstructionCopSub::RdName() const
	{
		RETURN_GPR_NAME(Rd)
	}

	StringView InstructionCopSub::RtName() const
	{
		RETURN_GPR_NAME(Rt)
	}

	String InstructionCopSub::Stringify() const
	{
		return U"{} rt:{}, rd:{}"_fmt(StringifyEnum(Sub()), RtName(), RdName());
	}
}
