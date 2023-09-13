#pragma once

#include "N64/Forward.h"

namespace N64::Rdp_detail
{
	struct ScissorRect
	{
		uint16_t xl;
		uint16_t yl;
		uint16_t xh;
		uint16_t yh;
		bool f;
		bool o;
	};

	struct CommanderState
	{
		ScissorRect scissorRect;
	};

	struct CommanderContext
	{
		CommanderState* state;
	};
}
