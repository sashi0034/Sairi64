#include "stdafx.h"
#include "OrthoCommander.h"

#include "N64/N64Logger.h"

namespace N64::Rdp_detail
{
	void OrthoCommander::FullSync()
	{
		// TODO?
	}

	void OrthoCommander::EnqueueCommand(const RdpCommand& cmd)
	{
		N64_TRACE(Rdp, U"enqueued rdp command: {:02X}"_fmt(cmd.CommandId()));
	}
}
