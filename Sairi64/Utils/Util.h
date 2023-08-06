#pragma once

namespace Utils
{
	template <int x1, int x2, typename T>
	T GetBits(T value)
	{
		static_assert(std::is_integral<T>::value, "Type must be an integral type");
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

		T mask = ((T(1) << (x2 - x1 + 1)) - 1) << x1;
		return (value & mask) >> x1;
	}

	template <int x1, int x2, typename T>
	T SetBits(T value, T newBits)
	{
		static_assert(std::is_integral<T>::value, "Type must be an integral type");
		static_assert(x1 <= x2 && x2 < std::numeric_limits<T>::digits);

		T mask = ((T(1) << (x2 - x1 + 1)) - 1);
		newBits &= mask;
		mask <<= x1;

		return (value & ~mask) | ((newBits << x1) & mask);
	}
}
