#include "stdafx.h"
#include "Rdp.h"

namespace N64::Rdp_detail
{
	void Rdp::UpdateDisplay(N64System& n64)
	{
		m_display.ScanOut16Bit(n64);
	}

	void Rdp::Render(const Point& point) const
	{
		m_display.Render(point);
	}
}
