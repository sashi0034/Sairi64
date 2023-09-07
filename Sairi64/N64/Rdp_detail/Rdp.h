#pragma once
#include "Dpc.h"
#include "DisplayManager.h"
#include "OrthoCommander.h"

namespace N64::Rdp_detail
{
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

		void WriteStart(uint32 value);
		void WriteEnd(N64System& n64, uint32 value);
		void WriteStatus(N64System& n64, DpcStatusWrite32 write);

	private:
		class Impl;
		class Interface;

		Dpc m_dpc{};
		DisplayManager m_display{};
		RdpCommandBuffer m_commandBuffer{};
		OrthoCommander m_commander{};

		void checkRunCommand(N64System& n64);
	};
}
