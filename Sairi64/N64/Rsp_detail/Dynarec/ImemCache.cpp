#include "stdafx.h"
#include "ImemCache.h"

#include "Recompiler.h"
#include "N64/N64System.h"
#include "N64/Rsp_detail/Rsp.h"

namespace N64::Rsp_detail::Dynarec
{
	ImemCache::ImemCache()
	{
		m_tags.fill(-1);
		m_codes.fill({
			.fetchedInstruction = InvalidInstruction_0xFx64,
			.code = nullptr
		});
	}

	RecompiledCodeHandler ImemCache::HitBlockOrRecompile(N64System& n64, Rsp& rsp, ImemAddr16 pc)
	{
		const BlockTag tag = GetBlockTag(pc);
		const uint8 index = GetBlockIndex(pc);

		if (m_tags[index] == tag && m_codes[index].code != nullptr)
		{
			// キャッシュヒット
			return m_codes[index].code;
		}

		// 以下、キャッシュミス時の対処

		if (m_codes[index].code != nullptr)
		{
			// コード開放
			n64.GetJit().release(m_codes[index].code);
		}

		// 再コンパイル処理
		const auto target = RecompilingTarget{
			.startPc = pc,
			.destArray = &m_codes
		};
		RecompileFreshCode(n64, rsp, target);
		return m_codes[index].code;
	}

	void ImemCache::InvalidBlock(ImemAddr16 addr)
	{
		const uint8 index = GetBlockIndex(addr);
		m_tags[index] = InvalidTag_0xFF;
		m_codes[index].fetchedInstruction = InvalidInstruction_0xFx64;
	}
}
