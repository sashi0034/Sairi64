#include "stdafx.h"
#include "RecompiledCache.h"

namespace N64::Cpu_detail::Dynarec
{
	CpuCycles RecompiledCodeMissingHandler(N64System& n64)
	{
		return {};
	}

	void RecompiledCache::CheckInvalidatePage(PAddr32 paddr)
	{
		auto&& page = m_pages[GetPageIndex(paddr)];
		if (page.get() == nullptr) return;
		page->isBroken = true;

		// JITコード内でメモリアクセス関数呼び出しすること考えたら、ここで解放するのよくない
		// if (page->infoList[GetPageOffset(paddr)].IsCode())
		// {
		// 	// ページ解放
		// 	for (int i = 0; i < CachePageOffsetSize_0x400; ++i)
		// 	{
		// 		if (page->infoList[GetPageOffset(paddr)].IsEntry())
		// 		{
		// 			// TODO: リコンパイルされたコードの解放?
		// 			// もしくはstruct CachePageに解放関数配列を定義?
		// 		}
		// 	}
		// 	m_pages.clear();
		// }
	}
}
