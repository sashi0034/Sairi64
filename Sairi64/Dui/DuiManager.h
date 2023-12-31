﻿#pragma once
#include "N64/N64Frame.h"

namespace Dui
{
	struct DuiExportData
	{
		bool powerSaving;
	};

	class DuiManager
	{
	public:
		DuiManager();

		void Update(N64::N64System& n64System, N64::N64Frame& n64Frame, const N64::N64Config& n64Config);
		const DuiExportData& ExportData() const;

	private:
		class Impl; // @formatter:off
		struct ImplPtr : std::unique_ptr<Impl> { ~ImplPtr(); } m_impl; // @formatter:on
	};
}
