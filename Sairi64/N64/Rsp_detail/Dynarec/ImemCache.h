#pragma once
#include "N64/Forward.h"

namespace N64::Rsp_detail::Dynarec
{
	constexpr uint32 SpImemSize_0x1000 = 0x1000;

	typedef RspCycles (*RecompiledCodeHandler)();

	struct BlockCode
	{
		RecompiledCodeHandler code;
	};

	class ImemAddr16 : public Utils::EnumValue<uint16>
	{
	public:
		explicit constexpr ImemAddr16(uint16 v): EnumValue(v) { return; }
	};

	class ImemCache
	{
	public:
		RecompiledCodeHandler HitBlockOrRecompile(N64System& n64, Rsp& rsp, ImemAddr16 pc);

	private:
		Array<BlockCode> m_blocks{SpImemSize_0x1000};
	};
}
