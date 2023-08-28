#pragma once

namespace Utils
{
	template <class...>
	constexpr std::false_type AlwaysFalse{};

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

	using sint8 = std::int8_t;
	using sint16 = std::int16_t;
	using sint32 = std::int32_t;
	using sint64 = std::int64_t;
}

using Utils::sint8;
using Utils::sint16;
using Utils::sint32;
using Utils::sint64;
