#include "stdafx.h"
#include "Rdp.h"

#include "N64/N64Logger.h"
#include "N64/N64System.h"

namespace N64::Rdp_detail
{
	void Rdp::UpdateDisplay(N64System& n64)
	{
		switch (n64.GetVI().Control().GetType())
		{
		case ViType4::Blank:
			m_display.ScanBlank();
			break;
		case ViType4::Reserved:
			N64Logger::Abort();
			break;
		case ViType4::R5G5B5A1:
			m_display.ScanR5G5B5A1(n64);
			break;
		case ViType4::R8G8B8A8:
			m_display.ScanR8G8B8A8(n64);
			break;
		default: ;
		}
	}

	void Rdp::RenderReal(const Point& point) const
	{
		m_display.Render(point);
	}
}
