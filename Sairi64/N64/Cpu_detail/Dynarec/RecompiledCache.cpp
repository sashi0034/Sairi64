#include "stdafx.h"
#include "RecompiledCache.h"

#include "Recompiler.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"

namespace N64::Cpu_detail::Dynarec
{
	RecompiledCodeHandler RecompiledCache::HitBlockCodeOrRecompile(N64System& n64, Cpu& cpu, PAddr32 pc)
	{
		auto&& page = m_pages[GetPageIndex(pc)];
		if (page.get() == nullptr)
		{
			// ページ割り当て
			page.reset(new CachePage());
		}
		else if (page->isBroken)
		{
			// ページ解放
			for (const auto code : page->generatedCodes)
			{
				// コード解放
				n64.GetJit().release(code);
			}
			page.reset(new CachePage());
		}

		const uint32 pageOffset = GetPageOffset(pc);
		auto&& code = page->codeList[pageOffset];
		if (code.HasCode() == false)
		{
			N64_TRACE(U"recompile a new code: page index={:05X} offset={:03X}"_fmt(
				GetPageIndex(pc), GetPageOffset(pc)));

			// 新しいコード作成
			const auto recompiled = RecompileFreshCode(n64, cpu, pc);
			page->generatedCodes.push_back(recompiled.code);
			code.code = recompiled.code;

			page->infoList[pageOffset].info = recompiled.recompiledLength; // コード先頭にコード長を記す
			for (int i = 1; i < recompiled.recompiledLength; ++i)
			{
				page->infoList[pageOffset + i].info = -i; // 先頭以外にはコード先頭の位置を記す
			}

			N64_TRACE(U"recompiled finished: length={}"_fmt(recompiled.recompiledLength));
		}
		return code.code;
	}

	void RecompiledCache::CheckInvalidatePage(PAddr32 paddr) const
	{
		auto&& page = m_pages[GetPageIndex(paddr)];
		if (page.get() == nullptr) return;
		page->isBroken = true;
	}

	void RecompiledCache::CheckInvalidatePageBetween(PAddr32 beginInclusive, PAddr32 endInclusive) const
	{
		if (endInclusive < beginInclusive) return;

		const uint32 beginIndex = GetPageIndex(beginInclusive);
		const uint32 endIndex = GetPageIndex(endInclusive);
		for (uint32 i = beginIndex; i <= endIndex; ++i)
		{
			auto&& page = m_pages[i];
			if (page.get() != nullptr) page->isBroken = true;
		}
	}
}
