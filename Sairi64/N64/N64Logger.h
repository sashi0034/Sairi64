#pragma once

namespace N64
{
	class N64Logger
	{
	public:
		static void Trace(const String& message);
		static void Info(const String& message);
		static void Warn(const String& message);
		static void Error(const String& message);
		static void Assert(bool ok, const String& message);
		static void Abort();
	};
}

#define N64LOGGER_TRACE_ENABLE

#ifdef N64LOGGER_TRACE_ENABLE
#define N64_TRACE(message) N64Logger::Trace(message)
#else
#define N64_TRACE(message) ((void)0)
#endif
