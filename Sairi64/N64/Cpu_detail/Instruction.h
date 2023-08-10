#pragma once
#include "Utils/Util.h"

namespace N64::Cpu_detail
{
	using namespace Utils;

	class Instruction
	{
	public:
		Instruction(uint32 raw) : m_raw(raw) { return; }
		uint32 Raw() const { return m_raw; }
		uint32 Op() const { return GetBits<26, 31>(Raw()); }

	private:
		uint32 m_raw{};
	};

	class InstructionI : public Instruction
	{
	public:
		uint32 Imm() const { return GetBits<0, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
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
		uint32 Funct() const { return GetBits<0, 5>(Raw()); }
		uint32 Sa() const { return GetBits<6, 10>(Raw()); } // shift amount
		uint32 Rd() const { return GetBits<11, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		uint32 Rs() const { return GetBits<21, 25>(Raw()); }
	};

	class InstructionCopZ1 : public Instruction
	{
	public:
		uint32 ShouldBeZero() const { return GetBits<0, 10>(Raw()); }
		uint32 Rd() const { return GetBits<11, 15>(Raw()); }
		uint32 Rt() const { return GetBits<16, 20>(Raw()); }
		uint32 Sub() const { return GetBits<21, 25>(Raw()); }
	};

	class InstructionCopFi : public Instruction
	{
	public:
		uint32 Offset() const { return GetBits<0, 15>(Raw()); }
		uint32 Ft() const { return GetBits<16, 20>(Raw()); }
		uint32 Base() const { return GetBits<21, 25>(Raw()); }
	};
}
