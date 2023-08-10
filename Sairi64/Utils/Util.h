#pragma once

namespace Utils
{
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

	template <typename T>
	class EnumValue
	{
		static_assert(std::is_integral<T>::value);

	public:
		const T value;
		explicit constexpr EnumValue(T v) : value{v} { return; }
		operator T() const { return value; }
	};
}
