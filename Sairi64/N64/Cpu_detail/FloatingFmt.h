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
	template <OpCop1FmtFunct funct> struct CvtTarget;

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
}
