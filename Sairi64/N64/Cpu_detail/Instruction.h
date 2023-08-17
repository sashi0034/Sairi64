#pragma once
#include "Utils/Util.h"

namespace N64::Cpu_detail
{
	using namespace Utils;

	enum class Opcode : uint8
	{
		SPECIAL = 0b000000,
		REGIMM = 0b000001,
		J = 0b000010,
		JAL = 0b000011,
		BEQ = 0b000100,
		BNE = 0b000101,
		BLEZ = 0b000110,
		BGTZ = 0b000111,
		ADDI = 0b001000,
		ADDIU = 0b001001,
		SLTI = 0b001010,
		SLTIU = 0b001011,
		ANDI = 0b001100,
		ORI = 0b001101,
		XORI = 0b001110,
		LUI = 0b001111,
		CP0 = 0b010000,
		CP1 = 0b010001,
		BEQL = 0b010100,
		BNEL = 0b010101,
		BLEZL = 0b010110,
		BGTZL = 0b010111,
		DADDI = 0b011000,
		DADDIU = 0b011001,
		LDL = 0b011010,
		LDR = 0b011011,
		LB = 0b100000,
		LH = 0b100001,
		LW = 0b100011,
		LBU = 0b100100,
		LHU = 0b100101,
		LWU = 0b100111,
		SB = 0b101000,
		SH = 0b101001,
		SW = 0b101011,
		SDL = 0b101100,
		SDR = 0b101101,
		CACHE = 0b101111,
		LL = 0b110000,
		LLD = 0b110100,
		LD = 0b110111,
		SC = 0b111000,
		SCD = 0b111100,
		SD = 0b111111,
	};

	enum class OpSpecialFunct : uint8
	{
		SLL = 0b000000,
		SRL = 0b000010,
		SRA = 0b000011,
		SLLV = 0b000100,
		SRLV = 0b000110,
		SRAV = 0b000111,
		JR = 0b001000,
		JALR = 0b001001,
		MFHI = 0b010000,
		MFLO = 0b010010,
		MULT = 0b011000,
		MULTU = 0b011001,
		DIV = 0b011010,
		DIVU = 0b011011,
		ADD = 0b100000,
		ADDU = 0b100001,
		SUB = 0b100010,
		SUBU = 0b100011,
		AND = 0b100100,
		OR = 0b100101,
		XOR = 0b100110,
		NOR = 0b100111,
		SLT = 0b101010,
		SLTU = 0b101011,
		DADD = 0b101100,
		DADDU = 0b101101,
		DSUB = 0b101110,
		DSUBU = 0b101111,
		TGE = 0b110000,
		TGEU = 0b110001,
		TLT = 0b110010,
		TLTU = 0b110011,
		TEQ = 0b110100,
		TNE = 0b110110,
		DSLL = 0b111000,
		DSRL = 0b111010,
		DSRA = 0b111011,
		DSLL32 = 0b111100,
		DSRL32 = 0b111110,
		DSRA32 = 0b111111,
	};

	enum class OpRegimm : uint8
	{
		BLTZ = 0b00000,
		BGEZ = 0b00001,
		BLTZL = 0b00010,
		BGEZL = 0b00011,
		BLTZAL = 0b10000,
		BGEZAL = 0b10001,
	};

	enum class OpCopSub : uint8
	{
		MFC = 0b00000,
		DMFC = 0b00001,
		MTC = 0b00100,
		DMTC = 0b00101,
		CFC = 0b00010,
		CTC = 0b00110,
		CO_0x10 = 0x10,
		// 0b10000
		CO_0x11 = 0x11,
		CO_0x12 = 0x12,
		CO_0x13 = 0x13,
		CO_0x14 = 0x14,
		CO_0x15 = 0x15,
		CO_0x16 = 0x16,
		CO_0x17 = 0x17,
		CO_0x18 = 0x18,
		CO_0x19 = 0x19,
		CO_0x1A = 0x1A,
		CO_0x1B = 0x1B,
		CO_0x1C = 0x1C,
		CO_0x1D = 0x1D,
		CO_0x1E = 0x1E,
		CO_0x1F = 0x1F,
		// 0b11111
	};

	class Instruction
	{
	public:
		Instruction(uint32 raw) : m_raw(raw) { return; }
		uint32 Raw() const { return m_raw; }
		Opcode Op() const { return static_cast<Opcode>(GetBits<26, 31>(Raw())); }

		String OpName() const;

		String Stringify() const; // non-virtual
	private:
		uint32 m_raw{};
	};

	static_assert(sizeof(Instruction) == sizeof(uint32));

	class InstructionI : public Instruction
	{
	public:
		uint32 Imm() const { return GetBits<0, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		uint32 Rs() const { return GetBits<21, 25>(Raw()); }

		StringView RdName() const;
		StringView RtName() const;

		String Stringify() const;
	};

	class InstructionRegimm : public Instruction
	{
		OpRegimm Sub() const { return static_cast<OpRegimm>(GetBits<16, 20>(Raw())); }
	};

	class InstructionRegimmB : public InstructionRegimm
	{
	public:
		uint32 Offset() const { return GetBits<0, 15>(Raw()); }
		uint32 Rs() const { return GetBits<21, 25>(Raw()); }
	};

	class InstructionJ : public Instruction
	{
	public:
		uint32 Target() const { return GetBits<0, 25>(Raw()); }
	};

	class InstructionR : public Instruction
	{
	public:
		OpSpecialFunct Funct() const { return static_cast<OpSpecialFunct>(GetBits<0, 5>(Raw())); }
		uint32 Sa() const { return GetBits<6, 10>(Raw()); } // shift amount
		uint32 Rd() const { return GetBits<11, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		uint32 Rs() const { return GetBits<21, 25>(Raw()); }

		StringView RdName() const;
		StringView RtName() const;
		StringView RsName() const;

		String Stringify() const;
	};

	class InstructionCop : public Instruction
	{
	public:
		OpCopSub Sub() const { return static_cast<OpCopSub>(GetBits<21, 25>(Raw())); }
		String Stringify() const;
	};

	class InstructionCopSub : public InstructionCop
	{
	public:
		uint32 ShouldBeZero() const { return GetBits<0, 10>(Raw()); }
		uint32 Rd() const { return GetBits<11, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		// OpCopSub Sub() const { return static_cast<OpCopSub>(GetBits<21, 25>(Raw())); }

		StringView RdName() const;
		StringView RtName() const;

		String Stringify() const;
	};

	class InstructionCopCo : public InstructionCop
	{
	public:
		uint32 Funct() const { return GetBits<0, 5>(Raw()); }
		uint32 ShouldBeZero() const { return GetBits<6, 24>(Raw()); }
	};

	class InstructionFi : public Instruction
	{
	public:
		uint32 Offset() const { return GetBits<0, 15>(Raw()); }
		uint32 Ft() const { return GetBits<16, 20>(Raw()); }
		uint32 Base() const { return GetBits<21, 25>(Raw()); }
	};
}
