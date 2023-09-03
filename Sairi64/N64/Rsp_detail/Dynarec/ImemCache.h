#pragma once
#include "N64/Forward.h"
#include "N64/Rsp_detail/RspAddress.h"

namespace N64::Rsp_detail::Dynarec
{
	using BlockTag = uint8;

	constexpr int BlockSize_0x400 = SpImemSize_0x1000 / 4;
	constexpr uint64 InvalidInstruction_0xFx64 = UINT64_MAX;
	constexpr uint8 InvalidTag_0xFF = 0xFF;
	inline BlockTag GetBlockTag(ImemAddr16 addr) { return GetBits<0, 1, uint16>(addr); };
	inline uint16 GetBlockIndex(ImemAddr16 addr) { return addr >> 2; }

	typedef RspCycles (*RecompiledCodeHandler)();

	struct BlockCode
	{
		uint64 fetchedInstruction = InvalidInstruction_0xFx64;
		RecompiledCodeHandler code = nullptr;
		bool HasCode() const { return code != nullptr; }
	};

	using BlockCodeArray = std::array<BlockCode, BlockSize_0x400>;

	class ImemCache
	{
	public:
		ImemCache();
		RecompiledCodeHandler HitBlockOrRecompile(N64System& n64, Rsp& rsp, ImemAddr16 pc);
		void InvalidBlock(ImemAddr16 addr);

	private:
		std::array<BlockTag, BlockSize_0x400> m_tags{};
		std::array<BlockCode, BlockSize_0x400> m_codes{};
	};
}
