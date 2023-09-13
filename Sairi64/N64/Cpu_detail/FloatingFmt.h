#pragma once
#include "N64/Instruction.h"

namespace N64::Cpu_detail
{
	template <FloatingFmt fmt> struct FloatingFmtType;

	template <> struct FloatingFmtType<FloatingFmt::Single>
	{
		using type = float;
	};

	template <> struct FloatingFmtType<FloatingFmt::Double>
	{
		using type = double;
	};

	template <> struct FloatingFmtType<FloatingFmt::Word>
	{
		using type = uint32;
	};

	template <> struct FloatingFmtType<FloatingFmt::Long>
	{
		using type = uint64;
	};

	// Cvt
	template <OpCop1FmtFunct funct> struct CvtTarget
	{
		static_assert(AlwaysFalseValue<OpCop1FmtFunct, funct>);
	};

	template <> struct CvtTarget<OpCop1FmtFunct::CvtSFmt>
	{
		using type = float;
	};

	template <> struct CvtTarget<OpCop1FmtFunct::CvtDFmt>
	{
		using type = double;
	};

	template <> struct CvtTarget<OpCop1FmtFunct::CvtWFmt>
	{
		using type = uint32;
	};

	template <> struct CvtTarget<OpCop1FmtFunct::CvtLFmt>
	{
		using type = uint64;
	};

	// Trunc
	template <OpCop1FmtFunct funct> struct TruncTarget
	{
		static_assert(AlwaysFalseValue<OpCop1FmtFunct, funct>);
	};

	template <> struct TruncTarget<OpCop1FmtFunct::TruncWFmt>
	{
		using type = sint32;
	};

	template <> struct TruncTarget<OpCop1FmtFunct::TruncLFmt>
	{
		using type = sint64;
	};

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/float_util.h#L25
	template <typename T>
	static inline bool IsQNaN(T f)
	{
		if constexpr (std::same_as<T, float>)
		{
			const uint32 v = *reinterpret_cast<uint32*>(&(f));
			return (v & 0x7FC00000) == 0x7FC00000;
		}
		else if constexpr (std::same_as<T, double>)
		{
			const uint64 v = *reinterpret_cast<uint64*>(&(f));
			return ((v & 0x7FF0000000000000) == 0x7FF0000000000000) && ((v & 0xFFFFFFFFFFFFF) != 0);
		}
		else
		{
			static_assert(AlwaysFalse<T>);
			return {};
		}
	}
}
