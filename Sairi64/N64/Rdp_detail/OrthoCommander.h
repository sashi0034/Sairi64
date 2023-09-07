#pragma once
#include "RdpCommand.h"

namespace N64::Rdp_detail
{
	// CPU処理
	class OrthoCommander
	{
	public:
		void FullSync();
		void EnqueueCommand(const RdpCommand& cmd);

	private:
	};
}
