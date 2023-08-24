#pragma once
#include "Display.h"

namespace N64::Rdp_detail
{
	struct RenderConfig
	{
		Point startPoint;
		float scale;
	};

	class Rdp
	{
	public:
		void UpdateDisplay(N64System& n64);
		void RenderReal(const RenderConfig& config) const;

	private:
		Display m_display{};
	};
}
