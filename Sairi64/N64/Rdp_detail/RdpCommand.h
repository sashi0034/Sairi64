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

	constexpr uint8 MaxRdpCommandLength_22 = 22;

	enum class CommandId
	{
		NonShadedTriangle = 0x08,
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

		static RdpCommand MakeFromDwords(const std::span<const uint64> span) { return RdpCommand(span); };
		static RdpCommand MakeWithRearrangeWords(std::span<uint32> span32);

	private:
		explicit RdpCommand(std::span<const uint64> span): m_span(span) { return; }
		std::span<const uint64> m_span{};
	};

	inline const RdpCommand& GetEmptyCommand()
	{
		static constexpr std::array<uint64, MaxRdpCommandLength_22> data{};
		return RdpCommand::MakeFromDwords(data);
	}

	class EdgeCoefficient : public RdpCommand
	{
	public:
		bool RightMajor() const { return GetBits<55>(Data<offset + 0>()); }
		uint8 Level() const { return GetBits<51, 53>(Data<offset + 0>()); }
		uint8 Tile() const { return GetBits<48, 50>(Data<offset + 0>()); }
		FixedPoint16<14, 2> Yl() const { return GetBits<32, 45>(Data<offset + 0>()); }
		FixedPoint16<14, 2> Ym() const { return GetBits<16, 29>(Data<offset + 0>()); }
		FixedPoint16<14, 2> Yh() const { return GetBits<0, 13>(Data<offset + 0>()); }

		uint16 Xl() const { return GetBits<48, 63>(Data<offset + 1>()); }
		uint16 XlFrac() const { return GetBits<32, 47>(Data<offset + 1>()); }
		uint16 DxLDy() const { return GetBits<16, 31>(Data<offset + 1>()); }
		uint16 DxLDyFrac() const { return GetBits<0, 15>(Data<offset + 1>()); }

		uint16 Xh() const { return GetBits<48, 63>(Data<offset + 2>()); }
		uint16 XhFrac() const { return GetBits<32, 47>(Data<offset + 2>()); }
		uint16 DxHDy() const { return GetBits<16, 31>(Data<offset + 2>()); }
		uint16 DxHDyFrac() const { return GetBits<0, 15>(Data<offset + 2>()); }

		uint16 Xm() const { return GetBits<48, 63>(Data<offset + 3>()); }
		uint16 XmFrac() const { return GetBits<32, 47>(Data<offset + 3>()); }
		uint16 DxMDy() const { return GetBits<16, 31>(Data<offset + 3>()); }
		uint16 DxMDyFrac() const { return GetBits<0, 15>(Data<offset + 3>()); }

	private:
		static constexpr uint8 offset = 0;
	};

	class TextureCoefficient : public RdpCommand
	{
	public:
		uint16 SInt() const { return GetBits<48, 63>(Data<offset + 0>()); }
		uint16 TInt() const { return GetBits<32, 47>(Data<offset + 0>()); }
		uint16 WInt() const { return GetBits<16, 31>(Data<offset + 0>()); }

		uint16 DsDxInt() const { return GetBits<48, 63>(Data<offset + 1>()); }
		uint16 DtDxInt() const { return GetBits<32, 47>(Data<offset + 1>()); }
		uint16 DwDxInt() const { return GetBits<16, 31>(Data<offset + 1>()); }

		uint16 SFrac() const { return GetBits<48, 63>(Data<offset + 2>()); }
		uint16 TFrac() const { return GetBits<32, 47>(Data<offset + 2>()); }
		uint16 WFrac() const { return GetBits<16, 31>(Data<offset + 2>()); }

		uint16 DsDxFrac() const { return GetBits<48, 63>(Data<offset + 3>()); }
		uint16 DtDxFrac() const { return GetBits<32, 47>(Data<offset + 3>()); }
		uint16 DwDxFrac() const { return GetBits<16, 31>(Data<offset + 3>()); }

		uint16 DsDeInt() const { return GetBits<48, 63>(Data<offset + 4>()); }
		uint16 DtDeInt() const { return GetBits<32, 47>(Data<offset + 4>()); }
		uint16 DwDeInt() const { return GetBits<16, 31>(Data<offset + 4>()); }

		uint16 DsDyInt() const { return GetBits<48, 63>(Data<offset + 5>()); }
		uint16 DtDyInt() const { return GetBits<32, 47>(Data<offset + 5>()); }
		uint16 DwDyInt() const { return GetBits<16, 31>(Data<offset + 5>()); }

		uint16 DsDeFrac() const { return GetBits<48, 63>(Data<offset + 6>()); }
		uint16 DtDeFrac() const { return GetBits<32, 47>(Data<offset + 6>()); }
		uint16 DwDeFrac() const { return GetBits<16, 31>(Data<offset + 6>()); }

		uint16 DsDyFrac() const { return GetBits<48, 63>(Data<offset + 7>()); }
		uint16 DtDyFrac() const { return GetBits<32, 47>(Data<offset + 7>()); }
		uint16 DwDyFrac() const { return GetBits<16, 31>(Data<offset + 7>()); }

		static TextureCoefficient Empty() { return static_cast<TextureCoefficient>(GetEmptyCommand()); }

	private:
		static constexpr uint8 offset = 12;
	};
}
