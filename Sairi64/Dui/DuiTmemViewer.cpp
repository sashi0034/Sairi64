﻿#include "stdafx.h"
#include "DuiTmemViewer.h"

#include "DearImGuiAddon/DearImGuiAddon.hpp"
#include "N64/N64System.h"

namespace Dui
{
	constexpr uint32 bufferSize_256 = 256;
}

class Dui::DuiTmemViewer::Impl
{
public:
	void Update(std::string_view viewName, const N64::Rdp& rdp)
	{
		if (not ImGui::Begin(viewName.data()))
		{
			ImGui::End();
			return;
		}

		ImGui::SliderInt("Tile ID", &m_tileId, 0, 7);

		const auto [width,height] = updateTexture(rdp);
		constexpr float showScale = 2.0f;
		if (const auto id = m_im.GetId())
			ImGui::Image(id.value(), ImVec2{width * showScale, height * showScale},
			             ImVec2{0, 0},
			             ImVec2{
				             width / static_cast<float>(bufferSize_256), height / static_cast<float>(bufferSize_256)
			             });

		ImGui::End();
	}

private:
	int m_tileId{};
	Image m_pixelBuffer{bufferSize_256, bufferSize_256, Palette::Black};
	DynamicTexture m_texture{m_pixelBuffer, TexturePixelFormat::R8G8B8A8_Unorm};
	ImS3dTexture m_im{m_texture};

	Size updateTexture(const N64::Rdp& rdp)
	{
		auto&& commanderState = rdp.GetCommanderState();
		const uint8 tileId = m_tileId;
		auto&& targetTile = commanderState.tiles[tileId];
		const uint8 bytesPerTexel = N64::Rdp_detail::GetBytesParTexel(targetTile.size);
		const uint32 width = targetTile.line * sizeof(uint64) / std::max(bytesPerTexel, static_cast<uint8>(1));
		// const uint32 height = (commanderState.tmem.size() - targetTile.tmemAddr * sizeof(uint64)) / std::max(
		// 	(width * bytesPerTexel * bytesPerTexel), 1u);
		const std::span tmem = commanderState.tmem;
		const auto safeSize = Size{std::min(width, bufferSize_256), bufferSize_256};
		dumpTmem(targetTile, safeSize, bytesPerTexel, tmem);

		m_texture.fill(m_pixelBuffer);
		return safeSize;
	}

	void dumpTmem(
		const N64::Rdp_detail::TileProps& tile, Size size, uint8 bytePerTexel, std::span<const uint8> tmem)
	{
		m_pixelBuffer.fill(Color{0});
		const uint32 bytesPerTileLine = tile.line * sizeof(uint64);
		for (uint32 y = 0; y < size.y; ++y)
		{
			for (uint32 x = 0; x < size.x; ++x)
			{
				const uint32 offset =
					tile.tmemAddr * sizeof(uint64) + (x ^ 1) * bytePerTexel + y * bytesPerTileLine; // ?
				if (offset >= tmem.size()) break;
				N64::Rdp_detail::Color16Bpp texel = Utils::ReadBytes16(tmem, offset);
				m_pixelBuffer[y][x].r = texel.R() << 3;
				m_pixelBuffer[y][x].g = texel.G() << 3;
				m_pixelBuffer[y][x].b = texel.B() << 3;
			}
		}
	}
};

namespace Dui
{
	DuiTmemViewer::DuiTmemViewer() :
		m_impl(std::make_unique<Impl>())
	{
	}

	void DuiTmemViewer::Update(std::string_view viewName, const N64::Rdp& rdp)
	{
		m_impl->Update(viewName, rdp);
	}

	DuiTmemViewer::ImplPtr::~ImplPtr() = default;
}
