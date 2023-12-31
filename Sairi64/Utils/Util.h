﻿#pragma once

namespace Utils
{
	template <class...>
	constexpr std::false_type AlwaysFalse{};

	template <typename T, T value>
	constexpr std::false_type AlwaysFalseValue{};

	template <int x1, int x2 = x1, typename T>
	T GetBits(T value)
	{
		static_assert(std::is_integral<T>::value);
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

		if constexpr (x2 + 1 == std::numeric_limits<T>::digits)
		{
			return value >> x1;
		}

		constexpr T mask = ((T(1) << (x2 - x1 + 1)) - 1) << x1;
		return (value & mask) >> x1;
	}

	template <int x1, int x2 = x1, typename T, typename U>
	[[nodiscard]] T SetBits(T value, U newBits)
	{
		static_assert(std::is_integral<T>::value);
		static_assert(std::is_convertible<U, T>::value);
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

		constexpr T mask1 = ((T(1) << (x2 - x1 + 1)) - 1);
		const T newBits1 = static_cast<U>(newBits) & mask1;

		constexpr T mask2 = mask1 << x1;
		return (value & ~mask2) | ((newBits1 << x1) & mask2);
	}

	template <int x1, int x2, typename T>
	class BitAccessor
	{
		static_assert(std::is_integral<T>::value);
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

	public:
		explicit BitAccessor(T& ref) : m_ref(ref) { return; };
		T Get() const { return Utils::GetBits<x1, x2>(m_ref); }
		operator T() const { return Get(); }
		void Set(T value) { m_ref = Utils::SetBits<x1, x2>(m_ref, value); }

	private:
		T& m_ref;
	};

	template <int x1, int x2 = x1, typename T>
	auto BitAccess(T& ref)
	{
		return BitAccessor<x1, x2, T>(ref);
	}

	constexpr inline uint32_t ByteSwap32(uint32_t value) noexcept
	{
		// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/external/portable_endian_bswap.h#L11
		return (((value) & 0xFF000000u) >> 24u) | (((value) & 0x00FF0000u) >> 8u) |
			(((value) & 0x0000FF00u) << 8u) | (((value) & 0x000000FFu) << 24u);
	}

	inline void ByteSwapWordArray(std::span<uint8> span)
	{
		if (span.size() & 0b11)
		{
			throw std::invalid_argument("The size of the array is not a multiple of 4.");
		}
		for (uint64 i = 0; i < span.size(); i += 4)
		{
			const uint32 original = *reinterpret_cast<uint32*>(&span[i]);
			*reinterpret_cast<uint32*>(&span[i]) = Utils::ByteSwap32(original);
		}
	}

	template <typename Wire>
	inline Wire ReadBytes(std::span<const uint8_t> span, uint64_t offset)
	{
		static_assert(std::endian::native == std::endian::little);
		// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/utils/MemoryHelpers.hpp#L9
		static_assert(std::is_integral<Wire>::value);
		if constexpr (sizeof(Wire) == 8) // 64-bit
		{
			const uint32 hi = *reinterpret_cast<const uint32*>(&span[offset + 0]);
			const uint32 lo = *reinterpret_cast<const uint32*>(&span[offset + 4]);
			Wire result = (static_cast<Wire>(hi) << 32) | static_cast<Wire>(lo);
			return result;
		}
		else
		{
			return *reinterpret_cast<const Wire*>(&span[offset]);
		}
	}

	inline uint64_t ReadBytes64(std::span<const uint8_t> span, uint64_t offset)
	{
		return ReadBytes<uint64_t>(span, offset);
	}

	inline uint32_t ReadBytes32(std::span<const uint8_t> span, uint64_t offset)
	{
		return ReadBytes<uint32_t>(span, offset);
	}

	inline uint16_t ReadBytes16(std::span<const uint8_t> span, uint64_t offset)
	{
		return ReadBytes<uint16_t>(span, offset);
	}

	inline uint8_t ReadBytes8(std::span<const uint8_t> span, uint64_t offset)
	{
		return ReadBytes<uint8_t>(span, offset);
	}

	template <typename Wire>
	inline void WriteBytes(std::span<uint8_t> span, uint64_t offset, Wire value)
	{
		static_assert(std::endian::native == std::endian::little);
		// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/utils/MemoryHelpers.hpp#L21
		static_assert(std::is_integral<Wire>::value);
		if constexpr (sizeof(Wire) == 8) // 64-bit
		{
			const uint32 hi = value >> 32;
			const uint32 lo = value;

			*reinterpret_cast<uint32*>(&span[offset + 0]) = hi;
			*reinterpret_cast<uint32*>(&span[offset + 4]) = lo;
		}
		else
		{
			*reinterpret_cast<Wire*>(&span[offset]) = value;
		}
	}

	inline void WriteBytes64(std::span<uint8_t> span, uint64_t offset, uint64_t value)
	{
		WriteBytes<uint64_t>(span, offset, value);
	}

	inline void WriteBytes32(std::span<uint8_t> span, uint64_t offset, uint32_t value)
	{
		WriteBytes<uint32_t>(span, offset, value);
	}

	inline void WriteBytes16(std::span<uint8_t> span, uint64_t offset, uint16_t value)
	{
		WriteBytes<uint16_t>(span, offset, value);
	}

	inline void WriteBytes8(std::span<uint8_t> span, uint64_t offset, uint8_t value)
	{
		WriteBytes<uint8_t>(span, offset, value);
	}

	template <typename T>
	class EnumValue
	{
		static_assert(std::is_integral<T>::value);

	public:
		const T value;
		explicit constexpr EnumValue(T v) : value{v} { return; }
		constexpr operator T() const { return value; }
	};

	template <typename T>
	class AddressRange
	{
		static_assert(std::is_integral<T>::value);

	public:
		const T base;
		const T end;

		explicit consteval AddressRange(T baseAddr, T endAddr) : base{baseAddr}, end{endAddr}
		{
			if (baseAddr >= endAddr) throw std::logic_error("base should be less than end");
		}

		bool IsBetween(T addr) const { return base <= addr && addr <= end; }
	};

	// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.cpp#L156
	template <typename T, uint8 intPart, uint8 fracPart>
	class FixedPointType
	{
	public:
		static_assert(intPart + fracPart == sizeof(T) * 8);
		FixedPointType(T raw = 0): m_raw(raw) { return; }
		FixedPointType(T intValue, T fracValue): m_raw((intValue << fracPart) | fracValue) { return; }

		// operator uint16() const { return m_raw; }
		T Raw() const { return m_raw; }
		auto Frac() { return BitAccess<0, fracPart - 1>(m_raw); }
		T Frac() const { return GetBits<0, fracPart - 1>(m_raw); }
		auto Int() { return BitAccess<fracPart, sizeof(T) * 8 - 1>(m_raw); }
		T Int() const { return GetBits<fracPart, sizeof(T) * 8 - 1>(m_raw); }

		template <uint8 otherIntPart, uint8 otherFracPart>
		FixedPointType<T, intPart, fracPart> operator+(FixedPointType<T, otherIntPart, otherFracPart> other)
		{
			FixedPointType<T, intPart, fracPart> result{};
			uint8 thisShift{};
			uint8 otherShift{};
			uint8 afterShift{};
			if constexpr (fracPart < otherFracPart)
			{
				thisShift = otherFracPart - fracPart;
				otherShift = 0;
				afterShift = thisShift;
			}
			else if constexpr (fracPart > otherFracPart)
			{
				thisShift = 0;
				otherShift = fracPart - otherFracPart;
				afterShift = 0;
			}
			if constexpr (std::same_as<T, uint16>)
			{
				const int32 a = static_cast<int32>(static_cast<int16>(m_raw)) << thisShift;
				const int32 b = static_cast<int32>(static_cast<int16>(other.Raw())) << otherShift;
				result.m_raw = (a + b) >> afterShift;
			}
			else if constexpr (std::same_as<T, uint32>)
			{
				const int64 a = static_cast<int64>(static_cast<int32>(m_raw)) << thisShift;
				const int64 b = static_cast<int64>(static_cast<int32>(other.Raw())) << otherShift;
				result.m_raw = (a + b) >> afterShift;
			}
			else static_assert(AlwaysFalse<T>);
			return result;
		}

		template <uint8 otherIntPart, uint8 otherFracPart>
		void operator+=(FixedPointType<T, otherIntPart, otherFracPart> other)
		{
			*this = *this + other;
		}

	private:
		T m_raw{};
	};

	template <uint8 intPart, uint8 fracPart>
	using FixedPoint16 = FixedPointType<uint16, intPart, fracPart>;

	template <uint8 intPart, uint8 fracPart>
	using FixedPoint32 = FixedPointType<uint32, intPart, fracPart>;

	inline int16 Sign8To16(int8 value)
	{
		return static_cast<int16>(value);
	}

	inline int32 Sign8To32(int8 value)
	{
		return static_cast<int32>(value);
	}

	inline int64 Sign8To64(int8 value)
	{
		return static_cast<int64>(value);
	}

	inline int32 Sign16To32(int16 value)
	{
		return static_cast<int32>(value);
	}

	inline int64 Sign16To64(int16 value)
	{
		return static_cast<int64>(value);
	}

	inline int64 Sign32To64(int32 value)
	{
		return static_cast<int64>(value);
	}

	inline void WaitAnyKeyOnConsole()
	{
		(void)(std::getchar());
	}

	template <typename E>
	[[nodiscard]] String StringifyEnum(E value) noexcept
	{
		const auto enumName = NAMEOF_ENUM(value);
		return Unicode::Widen(enumName);
	}

	template <int digit>
	inline std::string DecimalStdStr(uint64_t value)
	{
		std::stringstream stream{};
		stream << std::setfill('0') << std::setw(digit) << value;
		return stream.str();
	}

	template <int digit>
	inline std::string HexStdStr(uint64_t value)
	{
		std::stringstream stream{};
		stream << "0x" << std::uppercase << std::setfill('0') << std::setw(digit) << std::hex << value;
		return stream.str();
	}

	static void WriteStdStrToFile(const std::string& fileName, const std::string& str)
	{
		std::ofstream outputFile(fileName);
		if (outputFile.is_open())
		{
			outputFile << str;
			outputFile.close();
		}
		else
			throw std::runtime_error("could not open file: " + fileName);
	}

	static void EmptyEffect()
	{
		std::cout << "";
	}
}

using sint8 = std::int8_t;
using sint16 = std::int16_t;
using sint32 = std::int32_t;
using sint64 = std::int64_t;
