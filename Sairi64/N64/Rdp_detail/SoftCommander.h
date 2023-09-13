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
		void EnqueueCommand(const RdpCommand& cmd);

	private:
		CommanderState m_state{};

		SoftUnit execute(const RdpCommand& cmd);
	};
}
