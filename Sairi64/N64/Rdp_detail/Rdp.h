#pragma once
#include "Dpc.h"
#include "DisplayManager.h"
#include "SoftCommander.h"

namespace N64::Rdp_detail
{
	namespace DpAddress
	{
		constexpr PAddr32 Start_0x04100000{0x04100000};
		constexpr PAddr32 End_0x04100004{0x04100004};
		constexpr PAddr32 Current_0x04100008{0x04100008};
		constexpr PAddr32 Status_0x0410000C{0x0410000C};
		constexpr PAddr32 Clock_0x04100010{0x04100010};
		constexpr PAddr32 CmdBusy_0x04100014{0x04100014};
		constexpr PAddr32 PipeBusy_0x04100018{0x04100018};
		constexpr PAddr32 TMem_0x0410001C{0x0410001C};
	}

	struct RenderConfig
	{
		Point startPoint;
		float scale;
	};

	constexpr uint32 RdpCommandBufferSize_0xFFFFF = 0xFFFFF;

	struct RdpCommandBuffer
	{
		Array<uint32> list;
		int32 lastUnprocessedWords;
	};

	class Rdp
	{
	public:
		Rdp();
		void UpdateDisplay(N64System& n64);
		void RenderReal(const RenderConfig& config) const;

		uint32 Read32(PAddr32 paddr) const;
		void Write32(N64System& n64, PAddr32 paddr, uint32 value);

		void WriteStart(uint32 value);
		void WriteEnd(N64System& n64, uint32 value);
		void WriteStatus(N64System& n64, DpcStatusWrite32 write);

		Dpc& GetDpc() { return m_dpc; };
		const Dpc& GetDpc() const { return m_dpc; };

	private:
		class Impl;
		class Interface;

		Dpc m_dpc{
			.status = 0x80,
		};
		DisplayManager m_display{};
		RdpCommandBuffer m_commandBuffer{};
		SoftCommander m_commander{};

		void checkRunCommand(N64System& n64);
	};
}
