#pragma once

namespace N64::Memory_detail
{
	enum class CicType : uint32
	{
		CIC_UNKNOWN = 0,
		CIC_NUS_6101 = 1,
		CIC_NUS_7102 = 2,
		CIC_NUS_6102_7101 = 3,
		CIC_NUS_6103_7103 = 4,
		CIC_NUS_6105_7105 = 5,
		CIC_NUS_6106_7106 = 6
	};

	struct RomHeader
	{
		uint8 initialValue[4];
		uint32 clockRate;
		uint32 programCounter{};
		uint32 release;
		uint32 crc1;
		uint32 crc2;
		char unknown1[4];
		char unknown2[4];
		char imageName[20];
		// unknown from here
		char unknown5[4];
		uint32 manufacturerId;
		uint16 cartridgeId;

		union
		{
			char countryCode[2];
			uint16 countryCode16;
		};
	};

	static_assert(sizeof(RomHeader) == 0x40,
	              "rom_header_t size must be 0x40 bytes");

	class Rom
	{
	public:
		bool LoadFile(const FilePath& filePath);
		CicType Cic() const { return m_cic; }
		uint32 CicSeed() const;

	private:
		Array<uint8> m_rom{};
		RomHeader m_header{};
		CicType m_cic{};
	};
}
