#include "stdafx.h"
#include "Rdp.h"

#include "Rdp_Interface.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"

class N64::Rdp_detail::Rdp::Impl
{
public:
	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/rdp/rdp.c#L148
	// https://github.com/SimoneN64/Kaizen/blob/75b98053c6a7353f6c6a9de04888ad54d99e0b00/src/backend/core/RDP.cpp#L117
	static void ProcessCommand(N64System& n64, Rdp& rdp)
	{
		auto&& dpc = rdp.m_dpc;
		dpc.status.Freeze().Set(true);
		auto&& buffer = rdp.m_commandBuffer;

		const uint32 current = dpc.current & 0x00FFFFF8;
		const uint32 end = dpc.end & 0x00FFFFF8;

		const int32 processLength = end - current;
		if (processLength <= 0) return;
		if (processLength + (buffer.lastUnprocessedWords * 4) > RdpCommandBufferSize_0xFFFFF)
		{
			N64Logger::Abort(U"too many rdp commands");
			return;
		}

		// メモリからバッファーへコマンド転送
		if (dpc.status.XbusDmemDma())
		{
			// DMEMから読み取り
			for (int i = 0; i < processLength; i += 4)
			{
				const uint32 commandWord = ReadBytes32(n64.GetRsp().Dmem(), (current + i) & SpMemoryMask_0xFFF);
				buffer.list[buffer.lastUnprocessedWords + (i >> 2)] = commandWord;
			}
		}
		else
		{
			// RDRAMから読み取り
			if (current > 0x7FFFFFF)
			{
				N64Logger::Abort(U"invalid rdram address for rdp commands");
				return;
			}
			for (int i = 0; i < processLength; i += 4)
			{
				const uint32 commandWord = ReadBytes32(n64.GetMemory().Rdram(), current + i);
				buffer.list[buffer.lastUnprocessedWords + (i >> 2)] = commandWord;
			}
		}

		// バッファーに読み込んだコマンドを処理
		const bool isProcessedAll = processCommandBuffer(n64, rdp, buffer, processLength);
		if (isProcessedAll)
		{
			buffer.lastUnprocessedWords = 0;
		}

		dpc.current = end;
		dpc.end = end;

		dpc.status.Freeze().Set(false);
	}

private:
	static bool processCommandBuffer(N64System& n64, Rdp& rdp, RdpCommandBuffer& buffer, int32 processLength)
	{
		const int32 processWords = (processLength >> 2) + buffer.lastUnprocessedWords;
		int32 bufferIndex = 0;
		bool isProcessedAll = true;

		// 処理可能な分だけループする
		while (bufferIndex < processWords)
		{
			const uint8 commandId = GetBits<24, 29>(buffer.list[bufferIndex]); // [56, 61]から32を引いた値
			static const auto commandLengthData = RdpCommandLengthData;
			const uint8 commandLength = commandLengthData[commandId];

			// 次のコマンドが残りの処理可能なバイト量以下なら、次回にそのコマンドを実行する
			if ((bufferIndex + commandLength) * 4 > processLength + (buffer.lastUnprocessedWords * 4))
			{
				buffer.lastUnprocessedWords = processWords - bufferIndex;

				Array<uint32> temp(buffer.lastUnprocessedWords);
				for (int i = 0; i < buffer.lastUnprocessedWords; i++)
				{
					temp[i] = buffer.list[bufferIndex + i];
				}
				for (int i = 0; i < buffer.lastUnprocessedWords; i++)
				{
					buffer.list[i] = temp[i];
				}

				isProcessedAll = false;
				break;
			}

			// コマンド番号が8以上なら処理する
			if (commandId >= 8)
			{
				// 実際にコマンド処理
				rdp.m_commander.EnqueueCommand(RdpCommand::MakeWithRearrangeWords(
					{buffer.list.data(), commandLength}));
			}
			if (commandId == RdpCommandFullSync_0x29)
			{
				onFullSync(n64, rdp);
			}
			bufferIndex += commandLength;
		}
		return isProcessedAll;
	}

	static void onFullSync(N64System& n64, Rdp& rdp)
	{
		rdp.m_commander.FullSync();
		rdp.m_dpc.status.PipeBusy().Set(false);
		rdp.m_dpc.status.StartGclk().Set(false);
		rdp.m_dpc.status.CbufReady().Set(false);
		InterruptRaise<Interruption::DP>(n64);
	}
};

namespace N64::Rdp_detail
{
	Rdp::Rdp()
	{
		m_commandBuffer.list.resize(RdpCommandBufferSize_0xFFFFF);
	}

	void Rdp::UpdateDisplay(N64System& n64)
	{
		switch (n64.GetVI().Control().GetType())
		{
		case ViType4::Blank:
			m_display.ScanBlank();
			break;
		case ViType4::Reserved:
			N64Logger::Abort();
			break;
		case ViType4::R5G5B5A1:
			m_display.ScanR5G5B5A1(n64);
			break;
		case ViType4::R8G8B8A8:
			m_display.ScanR8G8B8A8(n64);
			break;
		default: ;
		}
	}

	void Rdp::RenderReal(const RenderConfig& config) const
	{
		m_display.Render(config);
	}

	void Rdp::WriteStart(uint32 value)
	{
		Interface::WriteStart(*this, value);
	}

	void Rdp::WriteEnd(N64System& n64, uint32 value)
	{
		Interface::WriteEnd(n64, *this, value);
	}

	void Rdp::WriteStatus(N64System& n64, DpcStatusWrite32 write)
	{
		Interface::WriteStatus(n64, *this, write);
	}

	void Rdp::checkRunCommand(N64System& n64)
	{
		if (m_dpc.status.Freeze()) return;

		m_dpc.status.PipeBusy().Set(true);
		m_dpc.status.StartGclk().Set(true);

		if (m_dpc.end > m_dpc.current)
		{
			Impl::ProcessCommand(n64, *this);
		}

		m_dpc.status.CbufReady().Set(true);
	}
}
