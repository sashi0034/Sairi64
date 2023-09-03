#pragma once
#include "N64/Forward.h"
#include "N64/Rsp_detail/RspAddress.h"

namespace N64::Rsp_detail::Dynarec
{
	using BlockIndex = int16;
	using BlockTag = uint8;

	constexpr int BlockSize_0x400 = SpImemSize_0x1000 / 4;
	constexpr uint8 InvalidTag_0xFF = 0xFF;
	inline BlockTag GetBlockTag(ImemAddr16 addr) { return GetBits<0, 1, uint16>(addr); };
	inline uint16 GetBlockIndex(ImemAddr16 addr) { return addr >> 2; }

	typedef RspCycles (*RecompiledCodeHandler)();

	struct SpRecompileResult
	{
		RecompiledCodeHandler code;
		uint16 recompiledLength;
	};

	struct BlockCode
	{
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
		std::array<BlockTag, BlockSize_0x400> m_tagList{};
		std::array<BlockCode, BlockSize_0x400> m_codeList{};
		std::array<BlockIndex, BlockSize_0x400> m_headList{};
	};
}
