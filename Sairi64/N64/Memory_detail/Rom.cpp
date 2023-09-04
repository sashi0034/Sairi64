#include "stdafx.h"
#include "Rom.h"

#include "N64/N64Logger.h"
#include "Utils/Util.h"

namespace N64::Memory_detail
{
	constexpr uint32 maxRomSize = 0xF000'0000;

	constexpr int crc32TableSize_256 = 256;

	constexpr std::array<uint32_t, crc32TableSize_256> crc32Table()
	{
		std::array<uint32_t, crc32TableSize_256> table{};
		for (int i = 0; i < crc32TableSize_256; i++)
		{
			uint32_t rem = i;
			for (int j = 0; j < 8; j++)
			{
				if (rem & 1)
				{
					rem >>= 1;
					rem ^= 0xedb88320;
				}
				else
					rem >>= 1;
			}
			table[i] = rem;
		}
		return table;
	}

	// https://rosettacode.org/wiki/CRC-32#C
	// https://github.com/Dillonb/n64/blob/e015f9dddf82d4d8c813ff3a16d7044965acde86/src/mem/n64rom.c#L60C1-L60C53
	uint32_t crc32(uint32_t crc, const uint8_t* buffer, const size_t length)
	{
		constexpr std::array<uint32_t, crc32TableSize_256> table = crc32Table();

		crc = ~crc;
		const uint8_t* p_end = buffer + length;
		for (const uint8_t* p = buffer; p < p_end; p++)
		{
			const uint8_t octet = *p;
			crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
		}

		return ~crc;
	}

	CicType checksumToCic(uint32_t checksum)
	{
		// @formatter:off
		const auto result =
			checksum == 0xEC8B1325 ? CicType::CIC_NUS_7102 :
			checksum == 0x1DEB51A9 ? CicType::CIC_NUS_6101 :
			checksum == 0xC08E5BD6 ? CicType::CIC_NUS_6102_7101 :
			checksum == 0x03B8376A ? CicType::CIC_NUS_6103_7103 :
			checksum == 0xCF7F41DC ? CicType::CIC_NUS_6105_7105 :
			checksum == 0xD1059C6A ? CicType::CIC_NUS_6106_7106 :
			CicType::CIC_UNKNOWN;
		// @formatter:on

		return result;
	}

	enum class SupportedExtension
	{
		Unsupported,
		Z64
	};

	void rearrangeRomData(Array<uint8>& rom, SupportedExtension extension, const String& extensionName)
	{
		switch (extension)
		{
		case SupportedExtension::Z64:
			// ビッグエンディアンからリトルエンディアンに
			Utils::ByteSwapWordArray(rom);
			break;
		default: ;
			N64Logger::Abort(U"unsupported extension: {}"_fmt(extensionName));
			break;
		}
	}

	bool Rom::LoadFile(const FilePath& filePath)
	{
		const auto extensionName = FileSystem::Extension(filePath);
		const auto extension = [&]()
		{
			if (extensionName == U"z64") return SupportedExtension::Z64;
			return SupportedExtension::Unsupported;
		}();
		if (extension == SupportedExtension::Unsupported)
		{
			N64Logger::Error(U"unsupported file extension: {}"_fmt(filePath));
			return false;
		}

		BinaryReader reader{filePath};
		if (reader.isOpen() == false)
		{
			N64Logger::Error(U"could not open file: {}"_fmt(filePath));
			return false;
		};

		const int romSize = reader.size();

		if (romSize < sizeof(RomHeader))
		{
			N64Logger::Error(U"rom file is too small: size={} < {}"_fmt(romSize, static_cast<int>(sizeof(RomHeader))));
			return false;
		}
		if (romSize > maxRomSize)
		{
			N64Logger::Error(U"rom file is huge: size={} > {}"_fmt(romSize, maxRomSize));
			return false;
		}

		m_actualRomSize = romSize;
		m_rom.resize(std::bit_ceil(static_cast<uint64>(romSize))); // バイトスワップなどをするので、一応切りのいい数にしておく

		// ファイル内容を配列に転送
		reader.read(m_rom.data(), romSize);

		// ヘッダ情報やCICなど取得
		readRomInfo();

		// ファイル形式に応じて、ビットスワップなどを行う
		rearrangeRomData(m_rom, extension, extensionName);

		return true;
	}


	void Rom::readRomInfo()
	{
		// header
		m_header = *reinterpret_cast<RomHeader*>(m_rom.data());

		// cic
		const uint32 checksum = crc32(0, &m_rom[0x40], 0x9C0);
		m_cic = checksumToCic(checksum);

		N64Logger::Info(U"imageName: \"{}\""_fmt(Unicode::Widen(m_header.imageName)));
		N64Logger::Info(U"CIC: {}"_fmt(Utils::StringifyEnum(m_cic)));

		// PAL check
		// https://github.com/SimoneN64/Kaizen/blob/d0bccfc7e7c0d6eaa3662e8286b9d2bf5888b74f/src/backend/core/Mem.hpp#L198
		constexpr std::array palCodes = {'D', 'F', 'I', 'P', 'S', 'U', 'X', 'Y'};
		m_isPal = std::ranges::any_of(palCodes, [this](const char c)
		{
			return m_rom[0x3d] == c;
		});
	}

	uint32 Rom::CicSeed() const
	{
		// https://github.com/SimoneN64/Kaizen/blob/dffd36fc31731a0391a9b90f88ac2e5ed5d3f9ec/src/backend/core/mmio/PIF.hpp#L84
		constexpr std::array<uint32, static_cast<int>(CicType::CIC_NUS_6106_7106) + 1> cicSeeds = {
			0x0,
			0x00043F3F, // CIC_NUS_6101
			0x00043F3F, // CIC_NUS_7102
			0x00043F3F, // CIC_NUS_6102_7101
			0x00047878, // CIC_NUS_6103_7103
			0x00049191, // CIC_NUS_6105_7105
			0x00048585, // CIC_NUS_6106_7106
		};
		return cicSeeds[static_cast<uint32>(m_cic)];
	}
}
