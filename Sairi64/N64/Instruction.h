#pragma once
#include "Utils/Util.h"

namespace N64
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
		CP2 = 0b010010,
		CP3 = 0b010011,
		BEQL = 0b010100,
		BNEL = 0b010101,
		BLEZL = 0b010110,
		BGTZL = 0b010111,
		DADDI = 0b011000,
		DADDIU = 0b011001,
		LDL = 0b011010,
		LDR = 0b011011,
		RDHWR = 0b011111,
		LB = 0b100000,
		LH = 0b100001,
		LWL = 0b100010,
		LW = 0b100011,
		LBU = 0b100100,
		LHU = 0b100101,
		LWR = 0b100110,
		LWU = 0b100111,
		SB = 0b101000,
		SH = 0b101001,
		SWL = 0b101010,
		SW = 0b101011,
		SDL = 0b101100,
		SDR = 0b101101,
		SWR = 0b101110,
		CACHE = 0b101111,
		LL = 0b110000,
		LWC1 = 0b110001,
		LWC2 = 0b110010, // RSP
		LLD = 0b110100,
		LDC1 = 0b110101,
		LD = 0b110111,
		SC = 0b111000,
		SWC1 = 0b111001,
		SWC2 = 0b111010, // RSP
		SCD = 0b111100,
		SDC1 = 0b111101,
		SD = 0b111111,
		Invalid_0xFF = 0xFF,
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
		SYSCALL = 0b001100,
		BREAK = 0b001101, // RSP
		SYNC = 0b001111,
		MFHI = 0b010000,
		MTHI = 0b010001,
		MFLO = 0b010010,
		MTLO = 0b010011,
		DSLLV = 0b010100,
		DSRLV = 0b010110,
		DSRAV = 0b010111,
		MULT = 0b011000,
		MULTU = 0b011001,
		DIV = 0b011010,
		DIVU = 0b011011,
		DMULT = 0b011100,
		DMULTU = 0b011101,
		DDIV = 0b011110,
		DDIVU = 0b011111,
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
		TGEI = 0b01000,
		TGEIU = 0b01001,
		TLTI = 0b01010,
		TLTIU = 0b01011,
		TEQI = 0b01100,
		TNEI = 0b01110,
		BLTZAL = 0b10000,
		BGEZAL = 0b10001,
		BGEZALL = 0b10011,
		Invalid_0xFF = 0xFF,
	};

	enum class OpCopSub : uint8
	{
		MFC = 0b00000,
		DMFC = 0b00001,
		MTC = 0b00100,
		DMTC = 0b00101,
		CFC = 0b00010,
		CTC = 0b00110, // 0b10000
		CO_0x10 = 0x10,
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
		CO_0x1F = 0x1F, // 0b11111
	};

	enum class OpCop0TlbFunct : uint8
	{
		TLBR = 0x01,
		TLBWI = 0x02,
		TLBWR = 0x06,
		TLBP = 0x08,
		ERET = 0x18,
		WAIT = 0x20,
	};

	enum class OpCop1FmtFunct : uint8
	{
		AddFmt = 0b000000,
		SubFmt = 0b000001,
		MulFmt = 0b000010,
		DivFmt = 0b000011,
		SqrtFmt = 0b000100,
		AbsFmt = 0b000101,
		MovFmt = 0b000110,
		NegFmt = 0b000111,
		RoundLFmt = 0b001000,
		TruncLFmt = 0b001001,
		CeilLFmt = 0b001010,
		FloorLFmt = 0b001011,
		RoundWFmt = 0b001100,
		TruncWFmt = 0b001101,
		CeilWFmt = 0b001110,
		FloorWFmt = 0b001111,
		CvtSFmt = 0b100000,
		CvtDFmt = 0b100001,
		CvtWFmt = 0b100100,
		CvtLFmt = 0b100101,
		CondFFmt = 0b110000,
		CondUnFmt = 0b110001,
		CondEqFmt = 0b110010,
		CondUeqFmt = 0b110011,
		CondOltFmt = 0b110100,
		CondUltFmt = 0b110101,
		CondOleFmt = 0b110110,
		CondUleFmt = 0b110111,
		CondSfFmt = 0b111000,
		CondNgleFmt = 0b111001,
		CondSeqFmt = 0b111010,
		CondNglFmt = 0b111011,
		CondLtFmt = 0b111100,
		CondNgeFmt = 0b111101,
		CondLeFmt = 0b111110,
		CondNgtFmt = 0b111111,
	};

	enum class FloatingFmt
	{
		Single = 0x10,
		Double = 0x11,
		Word = 0x14,
		Long = 0x15,
	};

	enum class OpCop2VecFunct : uint8
	{
		VMULF = 0b000000,
		VMULU = 0b000001,
		VRNDP = 0b000010,
		VMULQ = 0b000011,
		VMUDL = 0b000100,
		VMUDM = 0b000101,
		VMUDN = 0b000110,
		VMUDH = 0b000111,
		VMACF = 0b001000,
		VMACU = 0b001001,
		VRNDN = 0b001010,
		VMACQ = 0b001011,
		VMADL = 0b001100,
		VMADM = 0b001101,
		VMADN = 0b001110,
		VMADH = 0b001111,
		VADD = 0b010000,
		VSUB = 0b010001,
		VSUT = 0b010010,
		VABS = 0b010011,
		VADDC = 0b010100,
		VSUBC = 0b010101,
		VADDB = 0b010110,
		VSUBB = 0b010111,
		VACCB = 0b011000,
		VSUCB = 0b011001,
		VSAD = 0b011010,
		VSAC = 0b011011,
		VSUM = 0b011100,
		VSAR = 0b011101,
		Unknown_0x1E = 0b011110,
		Unknown_0x1F = 0b011111,
		VLT = 0b100000,
		VEQ = 0b100001,
		VNE = 0b100010,
		VGE = 0b100011,
		VCL = 0b100100,
		VCH = 0b100101,
		VCR = 0b100110,
		VMRG = 0b100111,
		VAND = 0b101000,
		VNAND = 0b101001,
		VOR = 0b101010,
		VNOR = 0b101011,
		VXOR = 0b101100,
		VNXOR = 0b101101,
		Unknown_0x2E = 0b101110,
		Unknown_0x2F = 0b101111,
		VRCP = 0b110000,
		VRCPL = 0b110001,
		VRCPH = 0b110010,
		VMOV = 0b110011,
		VRSQ = 0b110100,
		VRSQL = 0b110101,
		VRSQH = 0b110110,
		VNOP = 0b110111,
		VEXTT = 0b111000,
		VEXTQ = 0b111001,
		VEXTN = 0b111010,
		Unknown_0x3B = 0b111011,
		VINST = 0b111100,
		VINSQ = 0b111101,
		VINSN = 0b111110,
		VNULL = 0b111111,
		Undocumented_0xFE = 0xFE,
	};

	enum class OpLwc2Funct
	{
		LBV = 0b00000,
		LSV = 0b00001,
		LLV = 0b00010,
		LDV = 0b00011,
		LQV = 0b00100,
		LRV = 0b00101,
		LPV = 0b00110,
		LUV = 0b00111,
		LHV = 0b01000,
		LFV = 0b01001,
		LTV = 0b01011,
		Invalid_0xFF = 0xFF,
	};

	enum class OpSwc2Funct
	{
		SBV = 0b00000,
		SSV = 0b00001,
		SLV = 0b00010,
		SDV = 0b00011,
		SQV = 0b00100,
		SRV = 0b00101,
		SPV = 0b00110,
		SUV = 0b00111,
		SHV = 0b01000,
		SFV = 0b01001,
		SWV = 0b01010,
		STV = 0b01011,
		Invalid_0xFF = 0xFF,
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
		uint16 Imm() const { return GetBits<0, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		uint32 Rs() const { return GetBits<21, 25>(Raw()); }

		String Stringify() const;
	};

	class InstructionRegimm : public Instruction
	{
	public:
		uint16 Imm() const { return GetBits<0, 15>(Raw()); }
		uint16 Offset() const { return Imm(); }
		OpRegimm Sub() const { return static_cast<OpRegimm>(GetBits<16, 20>(Raw())); }
		uint32 Rs() const { return GetBits<21, 25>(Raw()); }

		String Stringify() const;
	};

	class InstructionJ : public Instruction
	{
	public:
		uint32 Target() const { return GetBits<0, 25>(Raw()); }

		String Stringify() const;
	};

	class InstructionR : public Instruction
	{
	public:
		OpSpecialFunct Funct() const { return static_cast<OpSpecialFunct>(GetBits<0, 5>(Raw())); }
		uint32 Sa() const { return GetBits<6, 10>(Raw()); } // shift amount
		uint32 Rd() const { return GetBits<11, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		uint32 Rs() const { return GetBits<21, 25>(Raw()); }

		String Stringify() const;
	};

	class InstructionCop : public Instruction
	{
	public:
		OpCopSub Sub() const { return static_cast<OpCopSub>(GetBits<21, 25>(Raw())); }
		uint8 CopNumber() const { return GetBits<26, 27>(Raw()); }
		String Stringify() const;
	};

	class InstructionCopSub : public InstructionCop
	{
	public:
		uint32 ShouldBeZero() const { return GetBits<0, 10>(Raw()); }
		uint32 Rd() const { return GetBits<11, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }

		String Stringify() const;
	};

	class InstructionCop1Sub : public InstructionCopSub
	{
	public:
		uint32 Fs() const { return Rd(); }

		String Stringify() const;
	};

	class InstructionCop0Tlb : public InstructionCop
	{
	public:
		OpCop0TlbFunct Funct() const { return static_cast<OpCop0TlbFunct>(GetBits<0, 5>(Raw())); }
		uint32 ShouldBeZero() const { return GetBits<6, 24>(Raw()); }

		String Stringify() const;
	};

	class InstructionCop1Fmt : public InstructionCop
	{
	public:
		OpCop1FmtFunct Funct() const { return static_cast<OpCop1FmtFunct>(GetBits<0, 5>(Raw())); }
		uint8 Fd() const { return GetBits<6, 10>(Raw()); }
		uint8 Fs() const { return GetBits<11, 15>(Raw()); }
		uint8 Ft() const { return GetBits<16, 20>(Raw()); }
		FloatingFmt Fmt() const { return static_cast<FloatingFmt>(GetBits<21, 25>(Raw())); }

		String Stringify() const;
	};

	class InstructionCop2Vec : public Instruction
	{
	public:
		bool IsFunct() const { return GetBits<25>(Raw()); }
	};

	class InstructionCop2VecFunct : public InstructionCop2Vec
	{
	public:
		OpCop2VecFunct Funct() const { return static_cast<OpCop2VecFunct>(GetBits<0, 5>(Raw())); }
		uint32 Vd() const { return GetBits<6, 10>(Raw()); }
		uint32 Vs() const { return GetBits<11, 15>(Raw()); }
		uint32 De() const { return Vs(); }
		uint32 Vt() const { return GetBits<16, 20>(Raw()); }
		uint32 Element() const { return GetBits<21, 24>(Raw()); }

		String Stringify() const;
	};

	class InstructionCop2VecSub : public InstructionCop2Vec
	{
	public:
		uint8 ShouldBeZero() const { return GetBits<0, 6>(Raw()); }
		uint32 Element() const { return GetBits<7, 10>(Raw()); }
		uint32 Vs() const { return GetBits<11, 15>(Raw()); } // same as Rd
		uint32 Vt() const { return GetBits<16, 20>(Raw()); } // same as Rt
		OpCopSub Sub() const { return static_cast<OpCopSub>(GetBits<21, 25>(Raw())); }

		String Stringify() const;
	};

	class InstructionFi : public Instruction
	{
	public:
		uint16 Offset() const { return GetBits<0, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		uint32 Ft() const { return Rt(); }
		uint32 Base() const { return GetBits<21, 25>(Raw()); }

		String Stringify() const;
	};

	class InstructionLv : public Instruction
	{
	public:
		uint8 Offset() const { return GetBits<0, 6>(Raw()); }
		uint8 Element() const { return GetBits<7, 10>(Raw()); }
		OpLwc2Funct Funct() const { return static_cast<OpLwc2Funct>(GetBits<11, 15>(Raw())); }
		uint8 Vt() const { return GetBits<16, 20>(Raw()); }
		uint8 Base() const { return GetBits<21, 25>(Raw()); }

		String Stringify() const;
	};

	class InstructionSv : public Instruction
	{
	public:
		uint8 Offset() const { return GetBits<0, 6>(Raw()); }
		uint8 Element() const { return GetBits<7, 10>(Raw()); }
		OpSwc2Funct Funct() const { return static_cast<OpSwc2Funct>(GetBits<11, 15>(Raw())); }
		uint8 Vt() const { return GetBits<16, 20>(Raw()); }
		uint8 Base() const { return GetBits<21, 25>(Raw()); }

		String Stringify() const;
	};

	template <class T>
	concept HasImm16 = requires(T t)
	{
		{ t.Imm() } -> std::convertible_to<uint16>;
	};
}
