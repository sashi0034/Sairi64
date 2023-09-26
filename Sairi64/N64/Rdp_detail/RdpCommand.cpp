#include "stdafx.h"
#include "RdpCommand.h"

namespace N64::Rdp_detail
{
	RdpCommand RdpCommand::MakeWithRearrangeWords(std::span<uint32> span32)
	{
		const auto command = std::span<uint64>(reinterpret_cast<uint64*>(span32.data()), span32.size() / 2);
		for (int i = 0; i < command.size(); i++)
		{
			const uint64_t lo = (command[i] >> 0) & 0xFFFFFFFF;
			const uint64_t hi = (command[i] >> 32) & 0xFFFFFFFF;
			command[i] = (lo << 32) | hi;
		}
		return RdpCommand(command);
	}
}
