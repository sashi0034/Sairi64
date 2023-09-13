#pragma once
#include "RdpCommand.h"
#include "Commander.h"

namespace N64::Rdp_detail
{
	struct SoftUnit
	{
	};

	// CPU処理
	class SoftCommander
	{
	public:
		void FullSync();
		void EnqueueCommand(N64System& n64, const RdpCommand& cmd);

	private:
		CommanderState m_state{};

		SoftUnit execute(const CommanderContext& ctx, const RdpCommand& cmd);
	};
}
