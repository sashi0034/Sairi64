#pragma once

#include "../SoftCommander.h"

// https://ultra64.ca/files/documentation/silicon-graphics/SGI_RDP_Command_Summary.pdf

namespace N64::Rdp_detail
{
	class Soft
	{
	public:
		[[nodiscard]]
		static SoftUnit SetScissor(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& scissor = ctx.state->scissorRect;
			scissor.yl = GetBits<0, 11>(cmd.Data<0>());
			scissor.xl = GetBits<12, 23>(cmd.Data<0>());
			scissor.yh = GetBits<32, 43>(cmd.Data<0>());
			scissor.xh = GetBits<44, 55>(cmd.Data<0>());
			scissor.f = GetBits<25>(cmd.Data<0>());
			scissor.o = GetBits<24>(cmd.Data<0>());
			return {};
		}
	};
}
