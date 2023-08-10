#include "stdafx.h"
#include "Instruction.h"

#include "Gpr.h"

#define RETURN_GPR_NAME(gpr) return GprNames[gpr()];

namespace N64::Cpu_detail
{
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
		return U"{} sa:{}, rd:{}, rt:{}, rs:{}"_fmt(StringifyEnum(Funct()), RdName(), RtName(), RsName());
	}

	StringView InstructionCopZ1::RdName() const
	{
		RETURN_GPR_NAME(Rd)
	}

	StringView InstructionCopZ1::RtName() const
	{
		RETURN_GPR_NAME(Rt)
	}
}
