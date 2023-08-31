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

	typedef CpuCycles (*RecompiledCodeHandler)();

	struct RecompiledResult
	{
		RecompiledCodeHandler code;
		uint32 recompiledLength;
	};

	struct BlockCode
	{
		RecompiledCodeHandler code = nullptr;
		bool HasCode() const { return code != nullptr; }
	};

	struct BlockInfo
	{
		sint16 info = 0;

		void SetAsHead(sint16 codeLength) { info = codeLength; }
		void SetAsBody(sint16 headOffset) { info = headOffset; }
		bool IsCode() const { return info != 0; }
		bool IsCodeHead() const { return info > 0; }
		bool IsCodeBody() const { return info < 0; }
	};

	struct CachePage
	{
		std::array<BlockInfo, CachePageOffsetSize_0x400> infoList{};
		std::array<BlockCode, CachePageOffsetSize_0x400> codeList{};
		bool isBroken{};
		Array<RecompiledCodeHandler> generatedCodes{};
	};

	using CachePagePtr = std::unique_ptr<CachePage>;

	class RecompiledCache
	{
	public:
		RecompiledCodeHandler HitBlockCodeOrRecompile(N64System& n64, Cpu& cpu, PAddr32 pc);
		void CheckInvalidatePage(PAddr32 paddr) const;
		void CheckInvalidatePageBetween(PAddr32 beginInclusive, PAddr32 endInclusive) const;

	private:
		Array<CachePagePtr> m_pages{CachePageIndexSize_0x80000};
	};
}
