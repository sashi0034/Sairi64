#include "stdafx.h"
#include "N64Logger.h"

namespace N64
{
	void N64Logger::Trace(const String& message)
	{
		Console.writeln(U"[TRACE] {}"_fmt(message));
	}

	void N64Logger::Info(const String& message)
	{
		Console.writeln(U"[INFO] {}"_fmt(message));
	}

	void N64Logger::Warn(const String& message)
	{
		Console.writeln(U"[WARN] {}"_fmt(message));
	}

	void N64Logger::Error(const String& message)
	{
		Console.writeln(U"[ERROR] {}"_fmt(message));
	}

	void N64Logger::Assert(bool ok, const String& message)
	{
		if (ok == false) Error(message);
	}

	void N64Logger::Abort(const String& message)
	{
		Error(message);
		System::MessageBoxOK(U"N64 Abort", MessageBoxStyle::Error);
		std::exit(-1);
	}

	void N64Logger::Abort(const std::source_location& location)
	{
		Abort(U"abort: {} ({}) {}"_fmt(
			Unicode::Widen(location.file_name()), location.line(), Unicode::Widen(location.function_name())));
	}
}
