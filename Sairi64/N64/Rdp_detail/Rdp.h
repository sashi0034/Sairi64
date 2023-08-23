#pragma once
#include "Display.h"

namespace N64::Rdp_detail
{
	class Rdp
	{
	public:
		void UpdateDisplay(N64System& n64);
		void RenderReal(const Point& point) const;

	private:
		Display m_display{};
	};
}
