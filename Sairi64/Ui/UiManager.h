#pragma once

namespace N64
{
	class N64System;
	struct N64FrameInfo;
}

namespace Ui
{
	class UiManager
	{
	public:
		UiManager();
		void Update(N64::N64System& n64System, const N64::N64FrameInfo& n64Frame);

	private:
		class Impl; // @formatter:off
		struct ImplPtr : std::unique_ptr<Impl> { ~ImplPtr(); } m_impl; // @formatter:on
	};
}
