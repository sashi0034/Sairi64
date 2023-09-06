#pragma once
#include <source_location>
#include "Forward.h"

namespace N64
{
	enum class LogTag
	{
		True,
		General,
		Cpu,
		Mmio,
		Mmu,
		Rsp,
		Rdp,
		Max,
	};

	class N64Logger
	{
	public:
		N64Logger();
		static void Trace(const String& message);
		static void Info(const String& message);
		static void Warn(const String& message);
		static void Error(const String& message);
		static void Assert(bool ok, const String& message);
		static void Abort(const String& message,
		                  const std::source_location& location = std::source_location::current());
		static void Abort(const std::source_location& location = std::source_location::current());

		static bool IsTraceEnabled(LogTag tag);

	private:
		// @formatter:off
		class Impl;
		struct ImplPtr : std::unique_ptr<Impl> { ~ImplPtr(); } m_impl;
		// @formatter:on
	};
}

#ifdef _DEBUG // debug
#define N64LOGGER_TRACE_ENABLE
#else // release
#if not N64_RELEASE_ENABLE
#define N64LOGGER_TRACE_ENABLE
#endif
#endif

#ifdef N64LOGGER_TRACE_ENABLE
#define N64_TRACE(tag, message) do {if (N64Logger::IsTraceEnabled(LogTag:: tag)) N64Logger::Trace(message); } while (0)
#else
#define N64_TRACE(message) ((void)0)
#endif
