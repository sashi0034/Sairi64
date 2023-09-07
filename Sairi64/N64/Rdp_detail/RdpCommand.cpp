#include "stdafx.h"
#include "RdpCommand.h"

namespace N64::Rdp_detail
{
	RdpCommand RdpCommand::MakeWithRearrangeWords(std::span<uint32> span32)
	{
		const auto command = RdpCommand(
			std::span<uint64>(reinterpret_cast<uint64*>(span32.data()), span32.size() / 2));
		for (int i = 0; i < command.m_span.size(); i++)
		{
			const uint64_t lo = (command.m_span[i] >> 0) & 0xFFFFFFFF;
			const uint64_t hi = (command.m_span[i] >> 32) & 0xFFFFFFFF;
			command.m_span[i] = (lo << 32) | hi;
		}
		return command;
	}
}
