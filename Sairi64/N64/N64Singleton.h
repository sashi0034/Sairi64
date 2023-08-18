#pragma once
#include "N64Logger.h"
#include "N64System.h"

namespace N64
{
	class N64Singleton
	{
	public:
		static N64Singleton& Instance();

		N64Singleton();
		~N64Singleton();

		N64System& GetSystem() { return m_system; }
		N64Logger& GetLogger() { return m_logger; }

	private:
		N64System m_system{};
		N64Logger m_logger{};

		static N64Singleton* s_instance;
	};
}
