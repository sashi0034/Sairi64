#include "stdafx.h"
#include <Catch2/catch.hpp>

#include "UtilTest.h"

#include "Utils/Util.h"

namespace Tests
{
	using namespace Utils;

	TEST_CASE("GetBits")
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

	TEST_CASE("SetBits")
	{
		constexpr uint16_t value = 0b0101'0101'0101'0101;
		REQUIRE(SetBits<4, 8>(value, 0b0'0011) == 0b0101'0100'0011'0101);
		REQUIRE(SetBits<0, 7>(value, 0b0000'0010) == 0b0101'0101'0000'0010);
		REQUIRE(SetBits<8, 15>(value, 0b0000'0011) == 0b0000'0011'0101'0101);
		REQUIRE(SetBits<0, 15>(value, 0b1100110011001100) == 0b1100110011001100);
		REQUIRE(SetBits<7, 7>(value, 0b1) == 0b0101'0101'1101'0101);

		constexpr uint32_t value32 = 0b1100'1100'1100'1100'1100'1100'1100'1100;
		REQUIRE(SetBits<0, 31>(value32, 0xFFFFFFFF) == 0xFFFFFFFF);
		REQUIRE(SetBits<4, 8>(value32, 0b1'1011) == 0b1100'1100'1100'1100'1100'1101'1011'1100);
	}
}
