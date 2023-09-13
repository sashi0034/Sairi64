#pragma once
#include "Utils/Util.h"

// https://ultra64.ca/files/documentation/silicon-graphics/SGI_RDP_Command_Summary.pdf

namespace N64::Rdp_detail
{
	using namespace Utils;

	inline constexpr std::array<uint8, 64> RdpCommandLengthData = {
		2, 2, 2, 2, 2, 2, 2, 2, 8, 12, 24, 28, 24, 28, 40, 44,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
	};

	enum class CommandId
	{
		FillTriangle = 0x08,
		FillZBufferTriangle = 0x09,
		TextureTriangle = 0x0a,
		TextureZBufferTriangle = 0x0b,
		ShadeTriangle = 0x0c,
		ShadeZBufferTriangle = 0x0d,
		ShadeTextureTriangle = 0x0e,
		ShadeTextureZBufferTriangle = 0x0f,
		TextureRectangle = 0x24,
		TextureRectangleFlip = 0x25,
		SyncLoad = 0x26,
		SyncPipe = 0x27,
		SyncTile = 0x28,
		SyncFull = 0x29,
		SetKeyGb = 0x2a,
		SetKeyR = 0x2b,
		SetConvert = 0x2c,
		SetScissor = 0x2d,
		SetPrimDepth = 0x2e,
		SetOtherModes = 0x2f,
		LoadTLut = 0x30,
		SetTileSize = 0x32,
		LoadBlock = 0x33,
		LoadTile = 0x34,
		SetTile = 0x35,
		FillRectangle = 0x36,
		SetFillColor = 0x37,
		SetFogColor = 0x38,
		SetBlendColor = 0x39,
		SetPrimColor = 0x3a,
		SetEnvColor = 0x3b,
		SetCombine = 0x3c,
		SetTextureImage = 0x3d,
		SetMaskImage = 0x3e,
		SetColorImage = 0x3f
	};

	constexpr uint8 RdpCommandFullSync_0x29 = static_cast<uint8>(CommandId::SyncFull);

	class RdpCommand
	{
	public:
		CommandId Id() const { return static_cast<CommandId>(GetBits<56, 61>(m_span[0])); }
		template <uint8 index> uint64 Data() const { return m_span[index]; }

		static RdpCommand MakeWithRearrangeWords(std::span<uint32> span32);

	private:
		explicit RdpCommand(std::span<uint64> span): m_span(span) { return; }

		std::span<uint64> m_span{};
	};
}
