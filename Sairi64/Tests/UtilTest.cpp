#include "stdafx.h"
#include <Catch2/catch.hpp>

#include "Utils/Util.h"

namespace Tests
{
	using namespace Utils;

	TEST_CASE("UtilTest_GetBits")
	{
		constexpr uint16_t value = 0b0101'0101'0101'0101;
		REQUIRE(GetBits<4, 8>(value) == 0b1'0101);
		REQUIRE(GetBits<0, 0>(value) == 0b1);
		REQUIRE(GetBits<8, 15>(value) == 0b0101'0101);
		REQUIRE(GetBits<2, 3>(value) == 0b01);
		REQUIRE(GetBits<15, 15>(value) == 0b0);

		constexpr uint32_t value32 = 0b1100'1100'1100'1100'1100'1100'1100'1100;
		REQUIRE(GetBits<4, 8>(value32) == 0b0'1100);
		REQUIRE(GetBits<0, 31>(value32) == value32);
	}

	TEST_CASE("UtilTest_SetBits")
	{
		constexpr uint16_t value = 0b0101'0101'0101'0101;
		REQUIRE(SetBits<4, 8>(value, 0b0'0011) == 0b0101'0100'0011'0101);
		REQUIRE(SetBits<0, 7>(value, 0b0000'0010) == 0b0101'0101'0000'0010);
		REQUIRE(SetBits<8, 15>(value, 0b0000'0011) == 0b0000'0011'0101'0101);
		REQUIRE(SetBits<0, 15>(value, 0x23AB) == 0x23AB);
		REQUIRE(SetBits<7, 7>(value, 0b1) == 0b0101'0101'1101'0101);

		constexpr uint32_t value32 = 0b1100'1100'1100'1100'1100'1100'1100'1100;
		REQUIRE(SetBits<0, 31>(value32, 0xFF12FF34) == 0xFF12FF34);
		REQUIRE(SetBits<4, 8>(value32, 0b1'1011) == 0b1100'1100'1100'1100'1100'1101'1011'1100);
	}

	TEST_CASE("UtilTest_BitAccess")
	{
		uint16 bit = 0b0101'0000'1111'0011;

		auto&& access4_5 = BitAccess<4, 5>(bit);
		REQUIRE(access4_5.Get() == 0b11);

		access4_5.Set(0b00);
		REQUIRE(access4_5.Get() == 0b00);

		access4_5.Set(0b10'01);
		REQUIRE(access4_5.Get() == 0b01);
		REQUIRE(bit == 0b0101'0000'1101'0011);
	}

	TEST_CASE("UtilTest_ReadBytes")
	{
		std::vector<uint8_t> data = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

		// 32-bitの読み取り
		const uint32_t read32 = ReadBytes<uint32_t>(data, 4);
		REQUIRE(read32 == 0x04030201);

		// 64-bitの読み取り
		const uint64_t read64 = ReadBytes<uint64_t>(data, 4);
		REQUIRE(read64 == 0x04030201'08070605);
	}

	TEST_CASE("UtilTest_WriteBytes")
	{
		std::vector<uint8_t> data(32);

		// 32-bitの書き込み
		WriteBytes(data, 4, static_cast<uint32_t>(0x04030201));
		REQUIRE(data[4] == 0x01);
		REQUIRE(data[5] == 0x02);
		REQUIRE(data[6] == 0x03);
		REQUIRE(data[7] == 0x04);

		// 64-bitの書き込み
		WriteBytes(data, 8, static_cast<uint64_t>(0x0807060504030201));
		REQUIRE(data[8 + 0] == 0x05);
		REQUIRE(data[8 + 1] == 0x06);
		REQUIRE(data[8 + 2] == 0x07);
		REQUIRE(data[8 + 3] == 0x08);
		REQUIRE(data[8 + 4] == 0x01);
		REQUIRE(data[8 + 5] == 0x02);
		REQUIRE(data[8 + 6] == 0x03);
		REQUIRE(data[8 + 7] == 0x04);
		REQUIRE(ReadBytes64(data, 8) == 0x0807060504030201);
	}
}
