#pragma once

namespace Utils
{
	template <class...>
	constexpr std::false_type AlwaysFalse{};

	template <int x1, int x2, typename T>
	T GetBits(T value)
	{
		static_assert(std::is_integral<T>::value);
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

		constexpr T mask = ((T(1) << (x2 - x1 + 1)) - 1) << x1;
		return (value & mask) >> x1;
	}

	template <int x1, int x2, typename T, typename U>
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

	inline uint64_t ReadBytes64(std::span<const uint8_t> span, uint64_t offset)
	{
		return (static_cast<uint64_t>(span[offset + 0]) << 56) |
			(static_cast<uint64_t>(span[offset + 1]) << 48) |
			(static_cast<uint64_t>(span[offset + 2]) << 40) |
			(static_cast<uint64_t>(span[offset + 3]) << 32) |
			(static_cast<uint64_t>(span[offset + 4]) << 24) |
			(static_cast<uint64_t>(span[offset + 5]) << 16) |
			(static_cast<uint64_t>(span[offset + 6]) << 8) |
			(static_cast<uint64_t>(span[offset + 7]) << 0);
	}

	inline uint32_t ReadBytes32(std::span<const uint8_t> span, uint64_t offset)
	{
		return static_cast<uint32_t>(span[offset + 0] << 24) |
			static_cast<uint32_t>(span[offset + 1] << 16) |
			static_cast<uint32_t>(span[offset + 2] << 8) |
			static_cast<uint32_t>(span[offset + 3] << 0);
	}

	inline uint16_t ReadBytes16(std::span<const uint8_t> span, uint64_t offset)
	{
		return (static_cast<uint16_t>(span[offset + 0] << 8)) +
			(static_cast<uint16_t>(span[offset + 1] << 0));
	}

	inline uint8_t ReadBytes8(std::span<const uint8_t> span, uint64_t offset)
	{
		return span[offset];
	}

	template <typename Wire>
	inline Wire ReadBytes(std::span<const uint8_t> span, uint64_t offset)
	{
		if constexpr (std::is_same<Wire, uint8_t>::value)
			return ReadBytes8(span, offset);
		else if constexpr (std::is_same<Wire, uint16_t>::value)
			return ReadBytes16(span, offset);
		else if constexpr (std::is_same<Wire, uint32_t>::value)
			return ReadBytes32(span, offset);
		else if constexpr (std::is_same<Wire, uint64_t>::value)
			return ReadBytes64(span, offset);
		else
			static_assert(AlwaysFalse<Wire>);
	}

	inline void WriteBytes64(std::span<uint8_t> span, uint64_t offset, uint64_t value)
	{
		span[offset + 0] = (value >> 56) & 0XFF;
		span[offset + 1] = (value >> 48) & 0XFF;
		span[offset + 2] = (value >> 40) & 0XFF;
		span[offset + 3] = (value >> 32) & 0XFF;
		span[offset + 4] = (value >> 24) & 0XFF;
		span[offset + 5] = (value >> 16) & 0XFF;
		span[offset + 6] = (value >> 8) & 0XFF;
		span[offset + 7] = (value) & 0XFF;
	}

	inline void WriteBytes32(std::span<uint8_t> span, uint64_t offset, uint32_t value)
	{
		span[offset + 0] = (value >> 24) & 0XFF;
		span[offset + 1] = (value >> 16) & 0XFF;
		span[offset + 2] = (value >> 8) & 0XFF;
		span[offset + 3] = (value) & 0XFF;
	}

	inline void WriteBytes16(std::span<uint8_t> span, uint64_t offset, uint16_t value)
	{
		span[offset + 0] = (value >> 8) & 0XFF;
		span[offset + 1] = (value) & 0XFF;
	}

	inline void WriteBytes8(std::span<uint8_t> span, uint64_t offset, uint8_t value)
	{
		span[offset] = value;
	}

	template <typename Wire>
	inline void WriteBytes(std::span<uint8_t> span, uint64_t offset, Wire value)
	{
		if constexpr (std::is_same<Wire, uint8_t>::value)
			WriteBytes8(span, offset, value);
		else if constexpr (std::is_same<Wire, uint16_t>::value)
			WriteBytes16(span, offset, value);
		else if constexpr (std::is_same<Wire, uint32_t>::value)
			WriteBytes32(span, offset, value);
		else if constexpr (std::is_same<Wire, uint64_t>::value)
			WriteBytes64(span, offset, value);
		else
			static_assert(AlwaysFalse<Wire>);
	}

	template <typename T>
	class EnumValue
	{
		static_assert(std::is_integral<T>::value);

	public:
		const T value;
		explicit constexpr EnumValue(T v) : value{v} { return; }
		operator T() const { return value; }
	};

	template <typename T>
	class AddressRange
	{
		static_assert(std::is_integral<T>::value);

	public:
		const T base;
		const T end;
		explicit constexpr AddressRange(T baseAddr, T endAddr) : base{baseAddr}, end{endAddr} { return; }
		bool IsBetween(T addr) const { return base <= addr && addr <= end; }
		// T Offset(T addr) const { return addr - base; }
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
}
