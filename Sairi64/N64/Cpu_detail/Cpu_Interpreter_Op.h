#pragma once

#include "Cpu_Interpreter.h"
#include "N64/Mmu.h"
#include "N64/N64Logger.h"

#define BEGIN_OP N64_TRACE(instr.Stringify())
#define END_OP return {}

// 命令ドキュメント
// https://hack64.net/docs/VR43XX.pdf

namespace N64::Cpu_detail
{
	struct OperatedUnit
	{
	};

	// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/mips_instructions.h#L6
	template <typename T>
	bool isOverflowSignedAdd(T value1, T value2, T result)
	{
		return (((~(value1 ^ value2) & (value1 ^ result)) >> ((sizeof(T) * 8) - 1)) & 1);
	}

	template <typename T>
	bool isOverflowSignedSub(T value1, T value2, T result)
	{
		return ((((value1 ^ value2) & (value1 ^ result)) >> ((sizeof(T) * 8) - 1)) & 1);
	}

	enum class BranchType
	{
		// 常に遅延スロットの命令は実行される
		Normal,
		// 分岐条件成立時のみ、遅延スロットの命令は実行される
		Likely,
	};

	constexpr uint8 gprRA_31 = 31; // Return Address GPR
}

class N64::Cpu_detail::Cpu::Interpreter::Op
{
public:
	[[nodiscard]]
	static OperatedUnit ADD(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint32 rs = gpr.Read(instr.Rs());
		const uint32 rt = gpr.Read(instr.Rt());
		const uint32 result = rs + rt;

		if (isOverflowSignedAdd(rs, rt, result))
		{
			throwException(cpu, ExceptionKinds::ArithmeticOverflow, 0);
		}
		else
		{
			gpr.Write(instr.Rd(), (sint64)static_cast<sint32>(result));
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit ADDU(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint32 rs = gpr.Read(instr.Rs());
		const uint32 rt = gpr.Read(instr.Rt());
		const sint32 result = rs + rt;

		gpr.Write(instr.Rd(), static_cast<sint64>(result));

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SUB(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const sint32 rs = gpr.Read(instr.Rs());
		const sint32 rt = gpr.Read(instr.Rt());
		const sint32 result = rs - rt;

		if (isOverflowSignedSub(rs, rt, result))
		{
			throwException(cpu, ExceptionKinds::ArithmeticOverflow, 0);
		}
		else
		{
			gpr.Write(instr.Rd(), static_cast<sint64>(result));
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SUBU(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const sint32 rs = gpr.Read(instr.Rs());
		const sint32 rt = gpr.Read(instr.Rt());
		const sint32 result = rs - rt;

		gpr.Write(instr.Rd(), static_cast<sint64>(result));

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit AND(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint64 result = gpr.Read(instr.Rs()) & gpr.Read(instr.Rt());
		gpr.Write(instr.Rd(), result);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit XOR(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint64 result = gpr.Read(instr.Rs()) ^ gpr.Read(instr.Rt());
		gpr.Write(instr.Rd(), result);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit OR(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint64 result = gpr.Read(instr.Rs()) | gpr.Read(instr.Rt());
		gpr.Write(instr.Rd(), result);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit NOR(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint64 result = ~(gpr.Read(instr.Rs()) | gpr.Read(instr.Rt()));
		gpr.Write(instr.Rd(), result);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SLT(Cpu& cpu, InstructionR instr)
	{
		// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/mips_instructions.c#L962
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const sint64 rs = static_cast<sint64>(gpr.Read(instr.Rs()));
		const sint64 rt = static_cast<sint64>(gpr.Read(instr.Rt()));

		gpr.Write(instr.Rd(), rs < rt ? 1 : 0);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SLTU(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint64 rs = gpr.Read(instr.Rs());
		const uint64 rt = gpr.Read(instr.Rt());

		gpr.Write(instr.Rd(), rs < rt ? 1 : 0);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DSLL(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		uint64 rt = gpr.Read(instr.Rt());
		rt <<= instr.Sa();
		gpr.Write(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DSRL(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		uint64 rt = gpr.Read(instr.Rt());
		rt >>= instr.Sa();
		gpr.Write(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DSRA(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		sint64 rt = gpr.Read(instr.Rt());
		rt >>= instr.Sa();
		gpr.Write(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DSLL32(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		uint64 rt = gpr.Read(instr.Rt());
		rt <<= (instr.Sa() + 32);
		gpr.Write(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DSRL32(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		uint64 rt = gpr.Read(instr.Rt());
		rt >>= (instr.Sa() + 32);
		gpr.Write(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DSRA32(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		sint64 rt = gpr.Read(instr.Rt());
		rt >>= (instr.Sa() + 32);
		gpr.Write(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit JR(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint64 rs = gpr.Read(instr.Rs());
		branchVAddr64<BranchType::Normal>(cpu, rs, true);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit JALR(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint64 rs = gpr.Read(instr.Rs());
		branchVAddr64<BranchType::Normal>(cpu, rs, true);
		linkRegister(cpu, instr.Rd());
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MFHI(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		cpu.GetGpr().Write(instr.Rd(), cpu.Hi());
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MFLO(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		cpu.GetGpr().Write(instr.Rd(), cpu.Lo());
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MTHI(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		cpu.SetHi(cpu.GetGpr().Read(instr.Rs()));
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MTLO(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		cpu.SetLo(cpu.GetGpr().Read(instr.Rs()));
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MULT(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const sint64 rs = (sint64)static_cast<sint32>(gpr.Read(instr.Rs()));
		const sint64 rt = (sint64)static_cast<sint32>(gpr.Read(instr.Rt()));

		const sint64 result = rs * rt;

		const sint32 lo = result & 0xFFFF'FFFF;
		const sint32 hi = (result >> 32) & 0xFFFF'FFFF;

		cpu.SetLo(static_cast<sint64>(lo));
		cpu.SetHi(static_cast<sint64>(hi));
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MULTU(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const uint64 rs = gpr.Read(instr.Rs()) & 0xFFFF'FFFF;
		const uint64 rt = gpr.Read(instr.Rt()) & 0xFFFF'FFFF;

		const uint64 result = rs * rt;

		const sint32 lo = result & 0xFFFF'FFFF;
		const sint32 hi = (result >> 32) & 0xFFFF'FFFF;

		cpu.SetLo(static_cast<sint64>(lo));
		cpu.SetHi(static_cast<sint64>(hi));
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit ADDI(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint32 rs = gpr.Read(instr.Rs());
		const uint32 imm = static_cast<sint32>(static_cast<sint16>(instr.Imm()));
		const uint32 result = rs + imm;

		if (isOverflowSignedAdd<uint32>(rs, imm, result))
		{
			throwException(cpu, ExceptionKinds::ArithmeticOverflow, 0);
		}
		else
		{
			gpr.Write(instr.Rt(), (sint64)static_cast<sint32>(result));
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit ADDIU(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const uint32 rs = gpr.Read(instr.Rs());
		const sint16 imm = static_cast<sint16>(instr.Imm());
		const sint32 result = rs + imm;

		gpr.Write(instr.Rt(), (sint64)(result));

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit J(Cpu& cpu, InstructionJ instr)
	{
		BEGIN_OP;
		uint64 target = instr.Target();
		target <<= 2;
		target |= ((cpu.GetPc().Curr() - 4) & 0xFFFFFFFF'F0000000); // PC is now 4 ahead

		branchVAddr64<BranchType::Normal>(cpu, target, true);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit JAL(Cpu& cpu, InstructionJ instr)
	{
		BEGIN_OP;
		linkRegister(cpu, gprRA_31);

		uint64 target = instr.Target();
		target <<= 2;
		target |= ((cpu.GetPc().Curr() - 4) & 0xFFFFFFFF'F0000000); // PC is now 4 ahead

		branchVAddr64<BranchType::Normal>(cpu, target, true);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BLEZ(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const sint64 rs = static_cast<sint64>(cpu.GetGpr().Read(instr.Rs()));
		branchOffset16<BranchType::Normal>(cpu, instr, rs <= 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BLEZL(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const sint64 rs = static_cast<sint64>(cpu.GetGpr().Read(instr.Rs()));
		branchOffset16<BranchType::Likely>(cpu, instr, rs <= 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BGTZ(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const sint64 rs = static_cast<sint64>(cpu.GetGpr().Read(instr.Rs()));
		branchOffset16<BranchType::Normal>(cpu, instr, rs > 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BGTZL(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const sint64 rs = static_cast<sint64>(cpu.GetGpr().Read(instr.Rs()));
		branchOffset16<BranchType::Likely>(cpu, instr, rs > 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BEQ(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const bool condition = cpu.GetGpr().Read(instr.Rs()) == cpu.GetGpr().Read(instr.Rt());
		branchOffset16<BranchType::Normal>(cpu, instr, condition);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BEQL(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const bool condition = cpu.GetGpr().Read(instr.Rs()) == cpu.GetGpr().Read(instr.Rt());
		branchOffset16<BranchType::Likely>(cpu, instr, condition);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BNE(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const bool condition = cpu.GetGpr().Read(instr.Rs()) != cpu.GetGpr().Read(instr.Rt());
		branchOffset16<BranchType::Normal>(cpu, instr, condition);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SLTI(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const sint16 imm = instr.Imm();
		const sint64 rs = gpr.Read(instr.Rs());
		gpr.Write(instr.Rt(), rs < imm ? 1 : 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SLTIU(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const sint16 imm = instr.Imm();
		const uint64 rs = gpr.Read(instr.Rs());
		gpr.Write(instr.Rt(), rs < imm ? 1 : 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit ANDI(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const uint64 imm = instr.Imm();
		gpr.Write(instr.Rt(), gpr.Read(instr.Rs()) & imm);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit ORI(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const uint64 imm = instr.Imm();
		gpr.Write(instr.Rt(), gpr.Read(instr.Rs()) | imm);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit XORI(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const uint64 imm = instr.Imm();
		gpr.Write(instr.Rt(), gpr.Read(instr.Rs()) ^ imm);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BNEL(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		const bool condition = cpu.GetGpr().Read(instr.Rs()) != cpu.GetGpr().Read(instr.Rt());
		branchOffset16<BranchType::Likely>(cpu, instr, condition);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SLL(Cpu& cpu, InstructionR instr) // possibly NOP
	{
		BEGIN_OP;
		const sint32 result = cpu.GetGpr().Read(instr.Rt()) << instr.Sa();
		cpu.GetGpr().Write(instr.Rd(), result);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SLLV(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const uint32 rt = gpr.Read(instr.Rt());
		const sint32 result = rt << (gpr.Read(instr.Rs()) & 0b11111);
		gpr.Write(instr.Rd(), result);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SRL(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const uint32 rt = gpr.Read(instr.Rt());
		const sint32 result = rt >> instr.Sa();
		gpr.Write(instr.Rd(), (sint64)result);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SRA(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const sint64 rt = gpr.Read(instr.Rt());
		const sint32 result = (sint64)(rt >> static_cast<uint64>(instr.Sa()));
		gpr.Write(instr.Rd(), (sint64)result);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SRAV(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const sint64 rt = gpr.Read(instr.Rt());
		const sint32 result = (sint64)(rt >> (gpr.Read(instr.Rs()) & 0b11111));
		gpr.Write(instr.Rd(), (sint64)result);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SRLV(Cpu& cpu, InstructionR instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const uint32 rt = gpr.Read(instr.Rt());
		const sint32 result = rt >> (cpu.GetGpr().Read(instr.Rs()) & 0b11111);
		cpu.GetGpr().Write(instr.Rd(), result);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit LUI(Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		sint64 imm = static_cast<sint16>(instr.Imm());
		imm *= 0x10000; // 負数の左シフトは未定義動作なので乗算で実装
		cpu.GetGpr().Write(instr.Rt(), imm);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit LDL(N64System& n64, Cpu& cpu, InstructionFi instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const sint16 offset = static_cast<sint16>(instr.Offset());
		const uint64 vaddr = gpr.Read(instr.Base()) + offset;
		if (const auto paddr = Mmu::ResolveVAddr(cpu, vaddr))
		{
			const sint32 shift = 8 * ((vaddr ^ 0) & 7);
			const uint64 mask = (uint64)0xFFFFFFFFFFFFFFFF << shift;
			const uint64 data = Mmu::ReadPaddr64(n64, PAddr32(paddr.value() & ~7));
			const uint64 oldRt = gpr.Read(instr.Rt());
			gpr.Write(instr.Rt(), (oldRt & ~mask) | (data << shift));
		}
		else
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Load>(), 0);
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit LDR(N64System& n64, Cpu& cpu, InstructionFi instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();
		const sint16 offset = static_cast<sint16>(instr.Offset());
		const uint64 vaddr = gpr.Read(instr.Base()) + offset;
		if (const auto paddr = Mmu::ResolveVAddr(cpu, vaddr))
		{
			const sint32 shift = 8 * ((vaddr ^ 7) & 7);
			const uint64 mask = (uint64)0xFFFFFFFFFFFFFFFF << shift;
			const uint64 data = Mmu::ReadPaddr64(n64, PAddr32(paddr.value() & ~7));
			const uint64 oldRt = gpr.Read(instr.Rt());
			gpr.Write(instr.Rt(), (oldRt & ~mask) | (data >> shift));
		}
		else
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Load>(), 0);
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit LW(N64System& n64, Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/mips_instructions.c#L317
		auto&& gpr = cpu.GetGpr();

		const sint16 offset = static_cast<sint16>(instr.Imm());
		const uint64 vaddr = gpr.Read(instr.Rs()) + offset;

		if (checkAddressError<0b11>(cpu, vaddr))
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, ExceptionKinds::AddressErrorLoad, 0);
			END_OP;
		}

		if (const Optional<PAddr32> paddr = Mmu::ResolveVAddr(cpu, vaddr))
		{
			const sint32 word = Mmu::ReadPaddr32(n64, paddr.value());
			gpr.Write(instr.Rt(), (sint64)word);
		}
		else
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Load>(), 0);
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit LD(N64System& n64, Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/mips_instructions.c#L317
		auto&& gpr = cpu.GetGpr();

		const sint16 offset = static_cast<sint16>(instr.Imm());
		const uint64 vaddr = gpr.Read(instr.Rs()) + offset;

		if (checkAddressError<0b111>(cpu, vaddr))
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, ExceptionKinds::AddressErrorLoad, 0);
			END_OP;
		}

		if (const Optional<PAddr32> paddr = Mmu::ResolveVAddr(cpu, vaddr))
		{
			const sint64 dword = Mmu::ReadPaddr64(n64, paddr.value());
			gpr.Write(instr.Rt(), dword);
		}
		else
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Load>(), 0);
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SW(N64System& n64, Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/mips_instructions.c#L382
		auto&& gpr = cpu.GetGpr();

		const sint16 offset = static_cast<sint16>(instr.Imm());
		const uint64 vaddr = gpr.Read(instr.Rs()) + offset;

		if (checkAddressError<0b11>(cpu, vaddr))
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, ExceptionKinds::AddressErrorStore, 0);
			END_OP;
		}

		if (const Optional<PAddr32> paddr = Mmu::ResolveVAddr(cpu, vaddr))
		{
			const uint32 word = gpr.Read(instr.Rt());
			Mmu::WritePaddr32(n64, paddr.value(), word);
		}
		else
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Store>(), 0);
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SD(N64System& n64, Cpu& cpu, InstructionI instr)
	{
		BEGIN_OP;
		auto&& gpr = cpu.GetGpr();

		const sint16 offset = static_cast<sint16>(instr.Imm());
		const uint64 vaddr = gpr.Read(instr.Rs()) + offset;

		if (checkAddressError<0b111>(cpu, vaddr))
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, ExceptionKinds::AddressErrorStore, 0);
			END_OP;
		}

		if (const auto paddr = Mmu::ResolveVAddr(cpu, vaddr))
		{
			const uint64 dword = gpr.Read(instr.Rt());
			Mmu::WritePaddr64(n64, paddr.value(), dword);
		}
		else
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			throwException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Store>(), 0);
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BLTZ(Cpu& cpu, InstructionRegimm instr)
	{
		BEGIN_OP;
		const sint64 rs = cpu.GetGpr().Read(instr.Rs());
		branchOffset16<BranchType::Normal>(cpu, instr, rs < 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BLTZL(Cpu& cpu, InstructionRegimm instr)
	{
		BEGIN_OP;
		const sint64 rs = cpu.GetGpr().Read(instr.Rs());
		branchOffset16<BranchType::Likely>(cpu, instr, rs < 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BGEZ(Cpu& cpu, InstructionRegimm instr)
	{
		BEGIN_OP;
		const sint64 rs = cpu.GetGpr().Read(instr.Rs());
		branchOffset16<BranchType::Normal>(cpu, instr, rs >= 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BGEZL(Cpu& cpu, InstructionRegimm instr)
	{
		BEGIN_OP;
		const sint64 rs = cpu.GetGpr().Read(instr.Rs());
		branchOffset16<BranchType::Likely>(cpu, instr, rs >= 0);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BGEZAL(Cpu& cpu, InstructionRegimm instr)
	{
		BEGIN_OP;
		const sint64 rs = cpu.GetGpr().Read(instr.Rs());
		branchOffset16<BranchType::Normal>(cpu, instr, rs >= 0);
		linkRegister(cpu, gprRA_31);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit BGEZALL(Cpu& cpu, InstructionRegimm instr)
	{
		BEGIN_OP;
		const sint64 rs = cpu.GetGpr().Read(instr.Rs());
		branchOffset16<BranchType::Likely>(cpu, instr, rs >= 0);
		linkRegister(cpu, gprRA_31);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MFC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const sint32 value = cpu.GetCop0().Read32(instr.Rd());
		cpu.GetGpr().Write(instr.Rt(), (sint64)value);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DMFC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const uint64 value = cpu.GetCop0().Read64(instr.Rd());
		cpu.GetGpr().Write(instr.Rt(), value);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MTC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const uint32 rt = cpu.GetGpr().Read(instr.Rt());
		cpu.GetCop0().Write32(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DMTC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const uint64 rt = cpu.GetGpr().Read(instr.Rt());
		cpu.GetCop0().Write64(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit CFC1(Cpu& cpu, InstructionCop1Sub instr)
	{
		BEGIN_OP;
		auto&& cop1 = cpu.GetCop1();
		sint32 value;
		switch (uint8 fs = instr.Fs())
		{
		case 0:
			value = cop1.Fcr().fcr0;
			break;
		case 31:
			value = cop1.Fcr().fcr31;
			break;
		default:
			N64Logger::Abort(U"undefined cfc1 fs={}"_fmt(fs));
			value = 0;
			break;
		}
		cpu.GetGpr().Write(instr.Rt(), (sint64)value);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit CTC1(Cpu& cpu, InstructionCop1Sub instr)
	{
		BEGIN_OP;
		uint32 rt = instr.Rt();
		switch (uint8 fs = instr.Fs())
		{
		case 0:
			break; // fcr0 is read-only
		case 31:
			rt &= 0X183FFFF; // mask out bits held 0
			cpu.GetCop1().Fcr().fcr31 = {rt};
			break;
		default:
			N64Logger::Abort(U"undefined ctc1 fs={}"_fmt(fs));
			break;
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit CACHE(Instruction instr)
	{
		BEGIN_OP;
		// for now, do nothing
		END_OP;
	}

private:
	static void throwException(Cpu& cpu, ExceptionCode code, int coprocessorError)
	{
		cpu.handleException(cpu.m_pc.Prev(), code, coprocessorError);
	}

	template <BranchType branch>
	static void branchVAddr64(Cpu& cpu, uint64 vaddr, bool condition)
	{
		cpu.m_delaySlot.Set();

		if (condition)
		{
			cpu.m_pc.SetNext(vaddr);
			N64_TRACE(U"branch accepted vaddr={:016X}"_fmt(vaddr));
		}
		else
		{
			if constexpr (branch == BranchType::Likely)
			{
				// likelyのときは、遅延スロットを実行しないようにする
				cpu.m_pc.Change64(cpu.m_pc.Curr() + 4);
			}
			N64_TRACE(U"branch not accepted (vaddr={:016X})"_fmt(vaddr));
		}
	}

	template <BranchType branch, HasImm16 Instr>
	static void branchOffset16(Cpu& cpu, Instr instr, bool condition)
	{
		sint64 offset = static_cast<sint16>(instr.Imm());
		offset *= 4; // left shift 2

		branchVAddr64<branch>(cpu, cpu.GetPc().Curr() + offset, condition);
	}

	static void linkRegister(Cpu& cpu, uint8 gprNumber)
	{
		cpu.GetGpr().Write(gprNumber, cpu.GetPc().Curr() + 4);
	}

	template <uint64 mask> static bool checkAddressError(Cpu& cpu, uint64 vaddr)
	{
		// TODO
		// https://github.com/SimoneN64/Kaizen/blob/d0bccfc7e7c0d6eaa3662e8286b9d2bf5888b74f/src/backend/core/interpreter/instructions.cpp#L3-L4
		return false;
	}
};
