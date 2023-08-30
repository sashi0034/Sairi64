#include "stdafx.h"
#include "Mmu.h"

#include "N64Logger.h"
#include "N64System.h"

namespace N64::Mmu
{
	template <class...>
	constexpr std::false_type always_false{};

	inline Optional<PAddr32> resolveVAddrInternal(Cpu& cpu, uint64 vaddr)
	{
		// TODO: 64 bit mode

		switch (static_cast<uint32>(vaddr) >> 29)
		{
		case VMap::KSEG0_sr29_4: // 0x80000000, 0x9FFFFFFF
		case VMap::KSEG1_sr29_5: // 0xA0000000, 0xBFFFFFFF
			return PAddr32(vaddr & 0x1FFFFFFF);
		default: break;
		}

		N64Logger::Abort(U"unsupported vaddr: {:016x}"_fmt(vaddr));
		return none;
	}

	Optional<PAddr32> ResolveVAddr(Cpu& cpu, uint64 vaddr)
	{
		const Optional<PAddr32> paddr = resolveVAddrInternal(cpu, vaddr);
		N64_TRACE(paddr.has_value()
			? U"address translation vaddr:{:#018x} => paddr:{:#010x}"_fmt(vaddr, static_cast<uint32>(paddr.value()))
			: U"address translation vaddr:{:#018x} => failed"_fmt(vaddr));
		return paddr;
	}

#if not N64_RELEASE_ENABLE
	// #define MMU_ACCESS_LOG_READ
	// #define MMU_ACCESS_LOG_WRITE
#if defined(MMU_ACCESS_LOG_READ) || defined(MMU_ACCESS_LOG_WRITE)
	// デバッグ用アクセスログ
	const std::string accessLogFile = "asset/data/mini_paddr_0.txt";

	static struct
	{
		std::string log;
		bool wrote;
		uint64 count;
	} s_accessLog{};

	bool accessLogStart()
	{
		auto&& log = s_accessLog;
		if (log.wrote) return false;
		const uint64_t count = log.count;
		log.count++;
#if 0
		return false;
#endif
		return count >= 300'0000;
	}

	void accessLogEnd()
	{
#if 0
		return;
#endif
		auto&& log = s_accessLog;
		if (log.count > 650'0000)
		{
			WriteStdStrToFile(accessLogFile, log.log);
			log.wrote = true;
		}
	}
#endif
#ifdef MMU_ACCESS_LOG_READ
	// デバッグ用読み取りログ
	void readPaddr_log(int wire, uint32_t paddr, uint64_t value)
	{
		if (accessLogStart() == false) return;
		s_accessLog.log += DecimalStdStr<2>(wire) + " " + HexStdStr<8>(paddr) + "->" + HexStdStr<16>(value) += "\n";
		accessLogEnd();
	}
#endif
#ifdef MMU_ACCESS_LOG_WRITE
	// デバッグ用書き込みログ
	void writePaddr_log(int wire, uint32_t paddr, uint64_t value)
	{
		if (accessLogStart() == false) return;
		s_accessLog.log += DecimalStdStr<2>(wire) + " " + HexStdStr<8>(paddr) + "<-" + HexStdStr<16>(value) += "\n";
		accessLogEnd();
	}
#endif
#endif

	template <typename Wire>
	Wire readPaddr_unsupported(PAddr32 paddr)
	{
		N64Logger::Abort(U"read unsupported paddr {}-bit: {:08X}"_fmt(
			static_cast<int>(std::numeric_limits<Wire>::digits), static_cast<uint32>(paddr)));
		return {};
	}

	template <typename Wire>
	using readPaddr_func = Wire (*)(N64System& n64, PAddr32 paddr);

	template <typename Wire>
	consteval std::array<readPaddr_func<Wire>, 0x1000> readPaddr_mapHi12()
	{
		constexpr bool wire64 = std::is_same<Wire, uint64_t>::value;
		constexpr bool wire32 = std::is_same<Wire, uint32_t>::value;
		constexpr bool wire16 = std::is_same<Wire, uint16_t>::value;
		constexpr bool wire8 = std::is_same<Wire, uint8_t>::value;
		static_assert(wire64 || wire32 || wire16 || wire8);

		std::array<readPaddr_func<Wire>, 0x1000> mapHi12{};
		mapHi12.fill({
			[](N64System& n64, PAddr32 paddr)
			{
				return readPaddr_unsupported<Wire>(paddr);
			}
		});
		constexpr int hi_12 = 12;
		constexpr int lo_20 = 20;

		// RDRAM
		for (uint32 addr = PMap::RdramMemory.base; addr < PMap::RdramMemory.end; addr += (1 << lo_20))
		{
			// 0x00000000, 0x007FFFFF
			mapHi12[addr >> lo_20] = [](N64System& n64, PAddr32 paddr)
			{
				const uint32 offset = paddr - PMap::RdramMemory.base;
				return ReadBytes<Wire>(n64.GetMemory().Rdram(), EndianAddress<Wire>(offset));
			};
		}

		// RSP
		mapHi12[0x040] = [](N64System& n64, PAddr32 paddr)
		{
			switch (GetBits<12, 19, uint32>(paddr))
			{
			case 0x00: // 0x04000000, 0x04000FFF
			{
				const uint32 offset = paddr & 0xFFF;
				return Utils::ReadBytes<Wire>(n64.GetRsp().Dmem(), EndianAddress<Wire>(offset));
			}
			case 0x01: // 0x04001000, 0x04001FFF
			{
				const uint32 offset = paddr & 0xFFF;
				return Utils::ReadBytes<Wire>(n64.GetRsp().Imem(), EndianAddress<Wire>(offset));
			}
			default:
				if (PMap::RspRegisters.IsBetween(paddr)) // 0x04040000, 0x040BFFFF
				{
					if constexpr (wire32) return n64.GetRsp().ReadPAddr32(paddr);
				}
				return readPaddr_unsupported<Wire>(paddr);
			}
		};

		// MI
		mapHi12[0x043] = [](N64System& n64, PAddr32 paddr) // 0x04300000, 0x043FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetMI().Read32(paddr);
			}
			return readPaddr_unsupported<Wire>(paddr);
		};

		// VI
		mapHi12[0x044] = [](N64System& n64, PAddr32 paddr) // 0x04400000, 0x044FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetVI().Read32(paddr);
			}
			return readPaddr_unsupported<Wire>(paddr);
		};

		// AI
		mapHi12[0x045] = [](N64System& n64, PAddr32 paddr) // 0x04500000, 0x045FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetAI().Read32(paddr);
			}
			return readPaddr_unsupported<Wire>(paddr);
		};

		// PI
		mapHi12[0x046] = [](N64System& n64, PAddr32 paddr) // 0x04600000, 0x046FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetPI().Read32(n64, paddr);
			}
			return readPaddr_unsupported<Wire>(paddr);
		};

		// RI
		mapHi12[0x047] = [](N64System& n64, PAddr32 paddr) // 0x04700000, 0x047FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetRI().Read32(paddr);
			}
			return readPaddr_unsupported<Wire>(paddr);
		};

		// SI
		mapHi12[0x048] = [](N64System& n64, PAddr32 paddr) // 0x04800000, 0x048FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetSI().Read32(n64, paddr);
			}
			return readPaddr_unsupported<Wire>(paddr);
		};

		// PIF RAM
		mapHi12[0x1FC] = [](N64System& n64, PAddr32 paddr)
		{
			if (GetBits<6, 20, uint32>(paddr) == ((0x007 << 2) | 0b11)) // 0x1FC007C0, 0x1FC007FF
			{
				if constexpr (wire32)
				{
					const uint64 offset = paddr - PMap::PifRam.base;
					return ReadBytes32(n64.GetSI().GetPifRam(), offset);
				}
			}
			return readPaddr_unsupported<Wire>(paddr);
		};

		// ROM
		for (uint32 addr = PMap::Rom.base; addr < PMap::Rom.end; addr += (1 << lo_20))
		{
			// 0x10000000, 0x1FBFFFFF
			mapHi12[addr >> lo_20] = [](N64System& n64, PAddr32 paddr)
			{
				if constexpr (wire32)
				{
					const uint64 offset = paddr - PMap::Rom.base;
					return ReadBytes32(n64.GetMemory().GetRom().Data(), offset);
				}
				return readPaddr_unsupported<Wire>(paddr);
			};
		}

		return mapHi12;
	}

	template <typename Wire>
	void writePaddr_unsupported(PAddr32 paddr)
	{
		N64Logger::Abort(U"write unsupported paddr {}-bit: {:08X}"_fmt(
			static_cast<int>(std::numeric_limits<Wire>::digits), static_cast<uint32>(paddr)));
	}

	template <typename Wire, typename Value>
	using writePaddr_func = void (*)(N64System& n64, PAddr32 paddr, Value value);

	template <typename Wire, typename Value>
	consteval std::array<writePaddr_func<Wire, Value>, 0x1000> writePaddr_mapHi12()
	{
		constexpr bool wire64 = std::is_same<Wire, uint64_t>::value;
		constexpr bool wire32 = std::is_same<Wire, uint32_t>::value;
		constexpr bool wire16 = std::is_same<Wire, uint16_t>::value;
		constexpr bool wire8 = std::is_same<Wire, uint8_t>::value;
		static_assert(
			(wire8 && std::is_same<Value, uint32>::value) ||
			(wire16 && std::is_same<Value, uint32>::value) ||
			(wire32 && std::is_same<Value, uint32>::value) ||
			(wire64 && std::is_same<Value, uint64>::value)); // Valueは32bit以上

		std::array<writePaddr_func<Wire, Value>, 0x1000> mapHi12{};
		mapHi12.fill({
			[](N64System& n64, PAddr32 paddr, Value value)
			{
				return writePaddr_unsupported<Wire>(paddr);
			}
		});
		constexpr int hi_12 = 12;
		constexpr int lo_20 = 20;

		// RDRAM
		for (uint32 addr = PMap::RdramMemory.base; addr < PMap::RdramMemory.end; addr += (1 << lo_20))
		{
			// 0x00000000, 0x007FFFFF
			mapHi12[addr >> lo_20] = [](N64System& n64, PAddr32 paddr, Value value)
			{
				const uint32 offset = paddr - PMap::RdramMemory.base;
				return WriteBytes<Wire>(n64.GetMemory().Rdram(), EndianAddress<Wire>(offset), value);
			};
		}

		// RSP
		mapHi12[0x040] = [](N64System& n64, PAddr32 paddr, Value value)
		{
			switch (GetBits<12, 19, uint32>(paddr))
			{
			case 0x00: // 0x04000000, 0x04000FFF
			{
				if constexpr (wire8) value <<= 8 * (0b11 - (paddr & 0b11));
				if constexpr (wire16) value <<= 16 * !(paddr & 0b10);
				if constexpr (wire64) value >>= 32; // ?
				const uint32 offset = paddr & 0xFFF;
				// TODO: 本当にValue-bit書き込みであってる? 8, 16-bitアクセスの時はシフト量に応じて書き込み量変わる? 書き込み量はWire?
				return Utils::WriteBytes<Value>(n64.GetRsp().Dmem(), EndianAddress<Wire>(offset), value);
			}
			case 0x01: // 0x04001000, 0x04001FFF
			{
				if constexpr (wire8) value <<= 8 * (0b11 - (paddr & 0b11));
				if constexpr (wire16) value <<= 16 * !(paddr & 0b10);
				if constexpr (wire64) value >>= 32; // ?
				const uint32 offset = paddr & 0xFFF;
				return Utils::WriteBytes<Value>(n64.GetRsp().Imem(), EndianAddress<Wire>(offset), value);
			}
			default:
				if (PMap::RspRegisters.IsBetween(paddr)) // 0x04040000, 0x040BFFFF
				{
					if constexpr (wire32) return n64.GetRsp().WritePAddr32(n64, paddr, value);
				}
				return writePaddr_unsupported<Wire>(paddr);
			}
		};

		// MI
		mapHi12[0x043] = [](N64System& n64, PAddr32 paddr, Value value) // 0x04300000, 0x043FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetMI().Write32(n64, paddr, value);
			}
			return writePaddr_unsupported<Wire>(paddr);
		};

		// VI
		mapHi12[0x044] = [](N64System& n64, PAddr32 paddr, Value value) // 0x04400000, 0x044FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetVI().Write32(n64, paddr, value);
			}
			return writePaddr_unsupported<Wire>(paddr);
		};

		// AI
		mapHi12[0x045] = [](N64System& n64, PAddr32 paddr, Value value) // 0x04500000, 0x045FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetAI().Write32(n64, paddr, value);
			}
			return writePaddr_unsupported<Wire>(paddr);
		};

		// PI
		mapHi12[0x046] = [](N64System& n64, PAddr32 paddr, Value value) // 0x04600000, 0x046FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetPI().Write32(n64, paddr, value);
			}
			return writePaddr_unsupported<Wire>(paddr);
		};

		// RI
		mapHi12[0x047] = [](N64System& n64, PAddr32 paddr, Value value) // 0x04700000, 0x047FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetRI().Write32(paddr, value);
			}
			return writePaddr_unsupported<Wire>(paddr);
		};

		// SI
		mapHi12[0x048] = [](N64System& n64, PAddr32 paddr, Value value) // 0x04800000, 0x048FFFFF
		{
			if constexpr (wire32)
			{
				return n64.GetSI().Write32(n64, paddr, value);
			}
			return writePaddr_unsupported<Wire>(paddr);
		};

		// PIF RAM
		mapHi12[0x1FC] = [](N64System& n64, PAddr32 paddr, Value value)
		{
			if (GetBits<6, 20, uint32>(paddr) == ((0x007 << 2) | 0b11)) // 0x1FC007C0, 0x1FC007FF
			{
				if constexpr (wire32)
				{
					const uint64 offset = paddr - PMap::PifRam.base;
					return WriteBytes32(n64.GetSI().GetPifRam(), offset, value);
				}
			}
			return writePaddr_unsupported<Wire>(paddr);
		};

		// ROM
		for (uint32 addr = PMap::Rom.base; addr < PMap::Rom.end; addr += (1 << lo_20))
		{
			// 0x10000000, 0x1FBFFFFF
			mapHi12[addr >> lo_20] = [](N64System& n64, PAddr32 paddr, Value value)
			{
				if constexpr (wire32)
				{
					const uint64 offset = paddr - PMap::Rom.base;
					return WriteBytes32(n64.GetMemory().GetRom().Data(), offset, value);
				}
				return writePaddr_unsupported<Wire>(paddr);
			};
		}

		return mapHi12;
	}

	template <typename Wire>
	Wire readPaddr_internal(N64System& n64, PAddr32 paddr)
	{
		static constexpr std::array<readPaddr_func<Wire>, 0x1000> map = readPaddr_mapHi12<Wire>();
		return map[paddr >> 20](n64, paddr);
	}

	template <typename Wire>
	inline Wire readPaddr(N64System& n64, PAddr32 paddr)
	{
		const Wire value = readPaddr_internal<Wire>(n64, paddr);
#ifdef MMU_ACCESS_LOG_READ
		readPaddr_log(sizeof(Wire) * 8, paddr, value);
#endif
		return value;
	}

	template <typename Wire, typename Value>
	void writePaddr_internal(N64System& n64, PAddr32 paddr, Value value)
	{
		static constexpr std::array<writePaddr_func<Wire, Value>, 0x1000> map = writePaddr_mapHi12<Wire, Value>();
		n64.GetCpu().RecompiledCache().CheckInvalidatePage(paddr);
		return map[paddr >> 20](n64, paddr, value);
	}

	template <typename Wire, typename Value>
	inline void writePaddr(N64System& n64, PAddr32 paddr, Value value)
	{
#ifdef MMU_ACCESS_LOG_WRITE
		writePaddr_log(sizeof(Wire) * 8, paddr, value);
#endif
		writePaddr_internal<Wire, Value>(n64, paddr, value);
	}

	uint64 ReadPaddr64(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint64>(n64, paddr);
	}

	uint32 ReadPaddr32(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint32>(n64, paddr);
	}

	uint16 ReadPaddr16(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint16>(n64, paddr);
	}

	uint8 ReadPaddr8(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint8>(n64, paddr);
	}

	void WritePaddr64(N64System& n64, PAddr32 paddr, uint64 value64)
	{
		writePaddr<uint64>(n64, paddr, value64);
	}

	void WritePaddr32(N64System& n64, PAddr32 paddr, uint32 value32)
	{
		writePaddr<uint32>(n64, paddr, value32);
	}

	void WritePaddr16(N64System& n64, PAddr32 paddr, uint32 value32)
	{
		writePaddr<uint16>(n64, paddr, value32);
	}

	void WritePaddr8(N64System& n64, PAddr32 paddr, uint32 value32)
	{
		writePaddr<uint8>(n64, paddr, value32);
	}
}
