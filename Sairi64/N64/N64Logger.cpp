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
}
