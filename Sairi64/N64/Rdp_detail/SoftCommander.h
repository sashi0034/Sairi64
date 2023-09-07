#pragma once
#include "RdpCommand.h"

namespace N64::Rdp_detail
{
	// CPU処理
	class SoftCommander
	{
	public:
		void FullSync();
		void EnqueueCommand(const RdpCommand& cmd);

	private:
	};
}
