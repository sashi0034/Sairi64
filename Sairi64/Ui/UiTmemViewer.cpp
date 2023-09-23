#include "stdafx.h"
#include "UiTmemViewer.h"

#include "DearImGuiAddon/DearImGuiAddon.hpp"
#include "N64/N64System.h"

class Ui::UiTmemViewer::Impl
{
public:
	void Update(std::string_view viewName, const N64::Rdp& rdp)
	{
		ImGui::Begin(viewName.data());

		ImGui::SliderInt("Tile ID", &m_tileId, 0, 7);

		const auto [width,height] = updateTexture(rdp);
		constexpr float showScale = 4.0f;
		if (const auto id = m_im.GetId())
			ImGui::Image(id.value(), ImVec2{width * showScale, height * showScale});

		ImGui::End();
	}

private:
	int m_tileId{};
	Image m_pixelBuffer{};
	DynamicTexture m_texture{};
	ImS3dTexture m_im{m_texture};

	Size updateTexture(const N64::Rdp& rdp)
	{
		auto&& commanderState = rdp.GetCommanderState();
		const uint8 tileId = m_tileId;
		auto&& targetTile = commanderState.tiles[tileId];
		const uint8 bytesPerTexel = N64::Rdp_detail::GetBytesParTexel(targetTile.size);
		const uint32 width = targetTile.line * sizeof(uint64) / std::max(bytesPerTexel, static_cast<uint8>(1));
		const uint32 height = (commanderState.tmem.size() - targetTile.tmemAddr * sizeof(uint64)) / std::max(
			(width * bytesPerTexel * bytesPerTexel), 1u);
		const std::span tmem{
			&commanderState.tmem[targetTile.tmemAddr * sizeof(uint64)], width * height * bytesPerTexel
		};

		if (m_pixelBuffer.size() != Size{width, height})
		{
			m_pixelBuffer = Image(width, height, Palette::Black);;
			m_texture = DynamicTexture(m_pixelBuffer, TexturePixelFormat::R8G8B8A8_Unorm);
			m_im = ImS3dTexture(m_texture);
		}

		dumpTmem(targetTile, {width, height}, bytesPerTexel, tmem);

		m_texture.fill(m_pixelBuffer);
		return {width, height};
	}

	void dumpTmem(
		const N64::Rdp_detail::TileProps& tile, Size size, uint8 bytePerTexel, std::span<const uint8> tmem)
	{
		const uint32 bytesPerTileLine = tile.line * sizeof(uint64);
		for (uint32 y = 0; y < size.y; ++y)
		{
			for (uint32 x = 0; x < size.x; ++x)
			{
				const uint32 offset = (x ^ 1) * bytePerTexel + y * bytesPerTileLine; // ?
				if (offset >= tmem.size()) break;
				N64::Rdp_detail::Color16Bpp texel = Utils::ReadBytes16(tmem, offset);
				m_pixelBuffer[y][x].r = texel.R() << 3;
				m_pixelBuffer[y][x].g = texel.G() << 3;
				m_pixelBuffer[y][x].b = texel.B() << 3;
			}
		}
	}
};

namespace Ui
{
	UiTmemViewer::UiTmemViewer() :
		m_impl(std::make_unique<Impl>())
	{
	}

	void UiTmemViewer::Update(std::string_view viewName, const N64::Rdp& rdp)
	{
		m_impl->Update(viewName, rdp);
	}

	UiTmemViewer::ImplPtr::~ImplPtr() = default;
}
