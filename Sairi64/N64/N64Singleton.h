#pragma once
#include "N64Frame.h"
#include "N64Logger.h"
#include "N64System.h"

namespace N64
{
	class N64Singleton
	{
	public:
		N64Singleton();
		~N64Singleton();

		N64System& GetSystem() { return m_system; }
		N64Logger& GetLogger() { return m_logger; }
		N64Frame& GetFrame() { return m_frame; }

	private:
		N64System m_system{};
		N64Logger m_logger{};
		N64Frame m_frame{};

		static N64Singleton* s_instance;
	};
}
