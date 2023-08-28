#include "stdafx.h"
#include "RecompiledCache.h"

namespace N64::Cpu_detail::Dynarec
{
	void RecompiledCache::CheckInvalidatePage(PAddr32 paddr)
	{
		auto&& page = m_pages[GetPageIndex(paddr)];
		if (page.get() == nullptr) return;

		if (page->infos[GetPageOffset(paddr)].IsCode())
		{
			// ページ解放
			for (int i = 0; i < CachePageOffsetSize_0x400; ++i)
			{
				if (page->infos[GetPageOffset(paddr)].IsEntry())
				{
					// TODO: リコンパイルされたコードの解放?
					// もしくはstruct CachePageに解放関数配列を定義?
				}
			}
			m_pages.clear();
		}
	}
}
