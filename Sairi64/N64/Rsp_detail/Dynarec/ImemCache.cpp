#include "stdafx.h"
#include "ImemCache.h"

#include "SpRecompiler.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"
#include "N64/Rsp_detail/Rsp.h"

namespace N64::Rsp_detail::Dynarec
{
	constexpr BlockIndex invalidHead_0xFFFF = 0xFFFF;;

	ImemCache::ImemCache()
	{
		m_tagList.fill(-1);
		m_codeList.fill({
			.code = nullptr
		});
		m_headList.fill(invalidHead_0xFFFF);
	}

	RecompiledCodeHandler ImemCache::HitBlockOrRecompile(N64System& n64, Rsp& rsp, ImemAddr16 pc)
	{
		const BlockTag pcTag = GetBlockTag(pc);
		const uint8 pcIndex = GetBlockIndex(pc);

		if (m_tagList[pcIndex] == pcTag && m_codeList[pcIndex].code != nullptr)
		{
			// キャッシュヒット
			N64_TRACE(U"hit imem cache: index={:04X} tag={:02X} head={:04X}"_fmt(
				pcIndex, m_tagList[pcIndex], m_headList[pcIndex]));
			return m_codeList[pcIndex].code;
		}

		// 以下、ミス時の対処

		if (m_codeList[pcIndex].code != nullptr)
		{
			// コード開放
			N64_TRACE(U"release imem cache: index={:04X} tag={:02X} head={:04X}"_fmt(
				pcIndex, m_tagList[pcIndex], m_headList[pcIndex]));
			n64.GetJit().release(m_codeList[pcIndex].code);
		}

		N64_TRACE(U"sp-recompile start: pc={:04X}"_fmt(pc));

		// 再コンパイル処理
		const auto recompiled = SpRecompileFreshCode(n64, rsp, pc);
		m_tagList[pcIndex] = pcTag;
		m_codeList[pcIndex].code = recompiled.code;
		for (int i = pcIndex; i < pcIndex + recompiled.recompiledLength; ++i)
		{
			// 先頭情報更新
			if (m_headList[i] == invalidHead_0xFFFF || m_headList[i] > pcIndex)
				m_headList[i] = pcIndex;
		}

		N64_TRACE(U"sp-recompile finished: length={}"_fmt(recompiled.recompiledLength));

		return recompiled.code;
	}

	void ImemCache::InvalidBlock(ImemAddr16 addr)
	{
		const uint8 index = GetBlockIndex(addr);
		if (m_headList[index] == invalidHead_0xFFFF) [[likely]] return;

		const int headCursor = m_headList[index];
		int currentCursor = index;
		// 書き換えられたデータをコンパイルしていたコードを、ヘッド情報から全て無効化
		while (true)
		{
			m_tagList[currentCursor] = InvalidTag_0xFF;
			m_headList[currentCursor] = invalidHead_0xFFFF;
			currentCursor--;
			if (currentCursor <= headCursor) break;
			// 以前に無効にした領域に入るので打ち切り
			if (m_headList[currentCursor] == invalidHead_0xFFFF) break;
		}

		N64_TRACE(U"invalidated imem cache from index={:04X} to index={:04X}"_fmt(index, currentCursor + 1));
	}

	void ImemCache::InvalidBlockBetween(ImemAddr16 beginInclusive, ImemAddr16 endInclusive)
	{
		uint16 beginIndex = GetBlockIndex(beginInclusive);
		uint16 endIndex = GetBlockIndex(endInclusive);

		if (beginIndex > endIndex) std::swap(beginIndex, endIndex);

		for (uint16 i = beginIndex; i <= endIndex; ++i)
		{
			InvalidBlock(ImemAddr16(i));
		}
	}
}
