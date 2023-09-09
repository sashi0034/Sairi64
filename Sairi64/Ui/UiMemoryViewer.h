#pragma once

namespace N64
{
	class N64System;
}

namespace Ui
{
	class UiMemoryViewer
	{
	public:
		void Update(std::string_view viewName, std::span<uint8> memory);

	private:
		int m_baseAddr{};
	};
}
