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

		auto&& commanderState = rdp.GetCommanderState();
		const uint8 tileId = 0; // TODO
		auto&& targetTile = commanderState.tiles[tileId];
		const uint32 width = targetTile.line;
		const uint32 height = 40; // TODO
		const uint8 bytePerTexel = N64::Rdp_detail::GetBytesParTexel(targetTile.size);
		const std::span tmem{&commanderState.tmem[targetTile.tmemAddr], width * height * bytePerTexel};

		if (m_pixelBuffer.size() != Size{width, height})
		{
			m_pixelBuffer = Image(width, height, Palette::Black);;
			m_texture = DynamicTexture(m_pixelBuffer, TexturePixelFormat::R8G8B8A8_Unorm);
			m_im = ImS3dTexture(m_texture);
		}


		readTmem(targetTile, {width, height}, bytePerTexel, tmem);

		m_texture.fill(m_pixelBuffer);

		constexpr float showScale = 4.0f;
		if (const auto id = m_im.GetId())
			ImGui::Image(id.value(), ImVec2{width * showScale, height * showScale});

		ImGui::End();
	}

private:
	Image m_pixelBuffer{};
	DynamicTexture m_texture{};
	ImS3dTexture m_im{m_texture};

	void readTmem(
		const N64::Rdp_detail::TileProps& targetTile, Size size, uint8 bytePerTexel, std::span<const uint8> tmem)
	{
		for (uint32 x = 0; x < size.x; ++x)
		{
			for (uint32 y = 0; y < size.y; ++y)
			{
				const uint32 offset = x + y * targetTile.line * bytePerTexel;
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
