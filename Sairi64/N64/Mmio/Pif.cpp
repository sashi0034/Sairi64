#include "stdafx.h"
#include "Pif.h"

#include "Pif_executeRomHle.h"
#include "N64/Mmu.h"
#include "N64/N64System.h"
#include "N64/N64Logger.h"
#include "N64/Joybus/Controller.h"

namespace N64::Mmio
{
	// 1コマンドの内訳 (多分)
	// [0] = コマンド入力長
	// [1] = コマンド出力長
	// [2:2+コマンド入力長-1] = コマンド入力本体
	// [2+コマンド入力長:2+コマンド入力長+コマンド出力長-1] = コマンド出力本体

	class PifCmdArgs
	{
	public:
		PifCmdArgs(Pif& pif, int cursor) : m_span(pif.Ram().data() + cursor, pif.Ram().size() - cursor) { return; }
		uint8 Length() const { return m_span[0] & 0x3F; }
		bool IsEndOfCommands() const { return m_span[1] == 0xFE; }
		uint8 ResultLength() const { return m_span[1] & 0x3F; }
		uint8 Index() const { return m_span[2]; }

		template <uint8 offset> void SetAt(uint8 value) const { m_span[offset] = value; }
		template <uint8 offset> uint8 GetAt() const { return m_span[offset]; }

	private:
		std::span<uint8> m_span{};
	};

	class PifCmdResult
	{
	public:
		PifCmdResult(Pif& pif, int cursor) : m_span(pif.Ram().data() + cursor, pif.Ram().size() - cursor) { return; }
		uint8 Length() const { return m_span[0] & 0x3F; }
		template <uint8 offset> void SetAt(uint8 value) const { m_span[offset] = value; };

		template <uint8 startOffset, uint8 endIndex, typename... Values>
		void SetBetween(Values... values) const
		{
			static_assert(endIndex - startOffset + 1 == sizeof...(values));
			setFromInternal<startOffset>(values...);
		}

	private:
		std::span<uint8> m_span{};

		template <uint8 currentOffset, typename First, typename... Rest>
		void setFromInternal(First value, Rest... rest) const
		{
			m_span[currentOffset] = static_cast<uint8>(value);
			if constexpr (sizeof...(rest) > 0)
			{
				setFromInternal<currentOffset + 1>(rest...);
			}
		}
	};
}

class N64::Mmio::Pif::Impl
{
public:
	// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/backend/core/mmio/PIF.cpp#L155
	// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/mem/pif.c#L593
	static void ProcessCommands_1(Pif& pif)
	{
		int channel = 0;

		int cursor = 0;
		while (cursor < 63) // RAMサイズ64分の処理
		{
			const auto cmd = PifCmdArgs(pif, cursor);
			const uint8 cmdLength = cmd.Length();
			cursor++;

			if (cmdLength == 0 || cmdLength == 0x3D)
			{
				// 0xFD in PIF RAM = send reset signal to this pif channel
				channel++;
				continue;
			}
			if (cmdLength == 0x3E) break; // 0xFE in PIF RAM = end of commands
			if (cmdLength == 0x3F) continue;

			if (cmd.IsEndOfCommands()) break;
			const auto result = PifCmdResult(pif, cursor + 1 + cmdLength);
			cursor += 1 + cmdLength + result.Length();

			// コマンド処理
			switch (cmd.Index())
			{
			case 0: [[fallthrough]];
			case 0xFF:
				readControllerId(pif, channel, result);
				channel++;
				break;
			case 1:
				if (readButtons(pif, channel, result) == false) cmd.SetAt<1>(cmd.GetAt<1>() | 0x80);
				channel++;
				break;
			default: N64Logger::Abort(U"not implemented pif command index: {:02X}"_fmt(cmd.Index()));
			}
		}
	}

	static void readControllerId(Pif& pif, int channel, const PifCmdResult& result)
	{
		if (const auto device = pif.m_deviceManager.TryGet(channel))
		{
			switch (device->Type())
			{
			case Joybus::JoybusType::None:
				result.SetBetween<0, 2>(0x00, 0x00, 0x00);
				return;
			case Joybus::JoybusType::Controller:
				result.SetBetween<0, 2>(0x05, 0x00, device->HasAccessor() ? 0x02 : 0x01);
				return;
			case Joybus::JoybusType::DancePad:
				break;
			case Joybus::JoybusType::VRU:
				break;
			case Joybus::JoybusType::Mouse:
				break;
			case Joybus::JoybusType::RandnetKeyboard:
				break;
			case Joybus::JoybusType::DenshaDeGo:
				break;
			case Joybus::JoybusType::Eeprom4KB:
				break;
			case Joybus::JoybusType::Eeprom16KB:
				break;
			default: ;
			}
			N64Logger::Abort(U"unsupported device: {}"_fmt(static_cast<int>(device->Type())));
		}
		else
		{
			N64Logger::Abort();
		}
	}

	static bool readButtons(Pif& pif, int channel, const PifCmdResult& result)
	{
		if (const auto controller = pif.m_deviceManager.TryGet<Joybus::Controller>(channel))
		{
			const auto state = controller->ReadState();
			result.SetAt<0>(state.byte1);
			result.SetAt<1>(state.byte2);
			result.SetAt<2>(state.joyX);
			result.SetAt<3>(state.joyY);
			return true;
		}
		else
		{
			result.SetBetween<0, 3>(0x00, 0x00, 0x00, 0x00);
			return false;
		}
	}
};

namespace N64::Mmio
{
	// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/backend/core/mmio/PIF.cpp#L155
	// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/mem/pif.c#L593
	void Pif::ProcessCommands()
	{
		N64_TRACE(Mmio, U"pif control write");

		const uint8 control = m_ram[63];
		if (control & 1)
		{
			Impl::ProcessCommands_1(*this);
		}
		if (control & 0x02)
		{
			// TODO: CIC Challenge?
			m_ram[63] &= ~2;
		}
		if (control & 0x08)
		{
			m_ram[63] &= ~8;
		}
		if (control & 0x30)
		{
			m_ram[63] = 0x80;
		}
	}

	void Pif::ExecuteRom(N64System& n64)
	{
		switch (n64.GetMemory().GetRom().Cic())
		{
		case CicType::CIC_UNKNOWN:
			N64Logger::Abort();
			break;
		case CicType::CIC_NUS_6101:
		case CicType::CIC_NUS_7102:
		case CicType::CIC_NUS_6102_7101:
		case CicType::CIC_NUS_6103_7103:
			Mmu::WritePaddr32(n64, PAddr32(0x318), RdramSize_0x00800000);
			break;
		case CicType::CIC_NUS_6105_7105:
			Mmu::WritePaddr32(n64, PAddr32(0x3F0), RdramSize_0x00800000);
			break;
		case CicType::CIC_NUS_6106_7106:
			break;
		default: ;
		}
		Pif_executeRomHle(n64);
	}
}
