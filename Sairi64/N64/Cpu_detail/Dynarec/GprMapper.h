#pragma once

namespace N64::Cpu_detail
{
	class Gpr;
}

namespace N64::Cpu_detail::Dynarec
{
	static constexpr uint8 GpAvailableSize_8 = 8;

	namespace x86 = asmjit::x86;

	class GprMapper
	{
	public:
		GprMapper();
		void PushNonVolatiles(x86::Assembler& x86Asm);
		void PopNonVolatiles(x86::Assembler& x86Asm);

		[[nodiscard]] x86::Gpq AssignMap(x86::Assembler& x86Asm, Gpr& gpr, uint8 rd);
		[[nodiscard]] std::pair<x86::Gpq, x86::Gpq> AssignMap(x86::Assembler& x86Asm, Gpr& gpr, uint8 rd, uint8 rs);
		[[nodiscard]] std::tuple<x86::Gpq, x86::Gpq, x86::Gpq> AssignMap(
			x86::Assembler& x86Asm, Gpr& gpr, uint8 rd, uint8 rs, uint8 rt);
		void FlushClear(x86::Assembler& x86Asm, Gpr& gpr);

	private:
		using hostGpId_t = int8;
		using guestGprId_t = int8;

		std::array<hostGpId_t, 32> m_gpMap{}; // N64 GPR -> Host GP
		std::array<guestGprId_t, GpAvailableSize_8> m_gpStack{}; // Used N64 GPR
		uint8 m_gpStackCount{};

		const x86::Gpq& assignNew(x86::Assembler& x86Asm, Gpr& gpr, uint8 rd);
	};
}
