#pragma once
#include "N64/Forward.h"

namespace N64::Cpu_detail::Dynarec
{
	// PC物理アドレス上位20ビット(最上位は常に0なので実質19ビット)をページに対応させる
	constexpr int CachePageIndexSize_0x80000 = 1 << 19;
	// PC物理アドレス下位12ビット(下位2bitは常に0なので実質10ビット)をページに対応させる
	constexpr int CachePageOffsetSize_0x400 = 1 << 10;

	inline uint32 GetPageIndex(PAddr32 paddr) { return Utils::GetBits<12, 31, uint32>(paddr); }
	inline uint32 GetPageOffset(PAddr32 paddr) { return Utils::GetBits<2, 11, uint32>(paddr); }

	typedef void (*RecompiledCode)(N64System& n64, Cpu& cpu);

	void RecompiledCodeMissingHandler(N64System& n64, Cpu& cpu);

	struct BlockCode
	{
		RecompiledCode code = &RecompiledCodeMissingHandler;
	};

	struct BlockInfo
	{
		sint16 cycles = 0;

		bool IsCode() const { return cycles != 0; }
		bool IsEntry() const { return cycles > 0; }
		bool IsBody() const { return cycles < 0; }
	};

	struct CachePage
	{
		Array<BlockCode> codeList{CachePageOffsetSize_0x400};
		Array<BlockInfo> infoList{CachePageOffsetSize_0x400};
		bool isBroken{};
	};

	using CachePagePtr = std::unique_ptr<CachePage>;

	class RecompiledCache
	{
	public:
		void CheckInvalidatePage(PAddr32 paddr);

	private:
		Array<CachePagePtr> m_pages{Arg::reserve(CachePageIndexSize_0x80000)};
	};
}
