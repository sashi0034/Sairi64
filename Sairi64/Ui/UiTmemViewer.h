#pragma once
#include "N64/Rdp.h"

namespace Ui
{
	class UiTmemViewer
	{
	public:
		UiTmemViewer();
		void Update(std::string_view viewName, const N64::Rdp& rdp);

	private:
		class Impl; // @formatter:off
		struct ImplPtr : std::unique_ptr<Impl> { ~ImplPtr(); } m_impl; // @formatter:on
	};
}
