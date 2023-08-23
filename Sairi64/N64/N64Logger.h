#pragma once
#include <source_location>

namespace N64
{
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

		static bool IsTraceEnabled();

	private:
		// @formatter:off
		class Impl;
		struct ImplPtr : std::unique_ptr<Impl> { ~ImplPtr(); } m_impl;
		// @formatter:on
	};
}

#if _DEBUG
#define N64LOGGER_TRACE_ENABLE
#else
// TODO: リリースビルドでトレースしたくないときはコメントアウト
#define N64LOGGER_TRACE_ENABLE
#endif

#ifdef N64LOGGER_TRACE_ENABLE
#define N64_TRACE(message) if (N64Logger::IsTraceEnabled()) N64Logger::Trace(message)
#else
#define N64_TRACE(message) ((void)0)
#endif
