﻿#include "stdafx.h"
#include "N64Logger.h"

#include "N64Singleton.h"

namespace N64
{
	class N64Logger::Impl
	{
	public:
		static N64System& System() { return N64Singleton::Instance().GetSystem(); }
		static N64Logger& Instance() { return N64Singleton::Instance().GetLogger(); }
		bool isTraceEnabled{};

		void UpdateTranceEnabled()
		{
#if 0
			isTraceEnabled = true;
#endif
#if 0
			static int count = 0;
			count++;
			// if (count > 23772368) isTraceEnabled = true; // SI_STATUS登場付近
#endif
#if 0
			if (System().GetCpu().GetPc().Curr() == 0xFFFFFFFF80010CDC) // VI_XScale_0x04400030 書き込み付近
				isTraceEnabled = true;
#endif
		}
	};

	N64Logger::N64Logger() :
		m_impl(std::make_unique<Impl>())
	{
	}

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

	void N64Logger::Abort(const String& message, const std::source_location& location)
	{
		Error(U"{}\n\tlocation: {} ({}) {}"_fmt(
			message,
			Unicode::Widen(location.file_name()),
			location.line(),
			Unicode::Widen(location.function_name())));
		System::MessageBoxOK(U"N64 Abort", MessageBoxStyle::Error);
		std::exit(-1);
	}

	void N64Logger::Abort(const std::source_location& location)
	{
		Abort(U"aborted!", location);
	}

	bool N64Logger::IsTraceEnabled()
	{
		auto&& impl = Impl::Instance().m_impl;
		impl->UpdateTranceEnabled();
		return impl->isTraceEnabled;
	}

	N64Logger::ImplPtr::~ImplPtr() = default;
}
