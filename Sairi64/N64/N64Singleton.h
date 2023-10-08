#pragma once
#include "N64Logger.h"
#include "N64System.h"

namespace N64
{
	class N64Singleton : Uncopyable
	{
	public:
		static N64Singleton& Instance();

		N64Singleton();
		~N64Singleton();

		N64System& GetSystem() { return m_system; }
		const N64System& GetSystem() const { return m_system; }

		N64Logger& GetLogger() { return m_logger; }
		const N64Logger& GetLogger() const { return m_logger; }

	private:
		N64System m_system{};
		N64Logger m_logger{};
	};
}
