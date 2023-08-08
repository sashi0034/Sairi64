#pragma once

namespace Utils
{
	template <int x1, int x2, typename T>
	T GetBits(T value)
	{
		static_assert(std::is_integral<T>::value, "Type must be an integral type");
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

		const T mask = ((T(1) << (x2 - x1 + 1)) - 1) << x1;
		return (value & mask) >> x1;
	}

	template <int x1, int x2, typename T, typename U>
	[[nodiscard]] T SetBits(T value, U newBits)
	{
		static_assert(std::is_integral<T>::value, "Type must be an integral type");
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

		T mask = ((T(1) << (x2 - x1 + 1)) - 1);
		const T newBits1 = static_cast<U>(newBits) & mask;
		mask <<= x1;

		return (value & ~mask) | ((newBits1 << x1) & mask);
	}
}
