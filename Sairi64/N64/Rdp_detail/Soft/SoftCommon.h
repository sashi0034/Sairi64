#pragma once

#include "../SoftCommander.h"

namespace N64::Rdp_detail::Soft
{
	template <typename Wire>
	Wire ReadRdram(const CommanderContext& ctx, uint32 address)
	{
		static_assert(std::same_as<Wire, uint8> || std::same_as<Wire, uint16>);
		return ReadBytes<Wire>(ctx.rdram, EndianAddress<Wire>(address));
	}

	template <typename Wire>
	void WriteRdram(const CommanderContext& ctx, uint32 address, Wire value)
	{
		static_assert(std::same_as<Wire, uint8> || std::same_as<Wire, uint16>);
		WriteBytes<Wire>(ctx.rdram, EndianAddress<Wire>(address), value);
	}

	template <typename Wire>
	Wire ReadTmem(const CommanderState& state, uint16 address)
	{
		static_assert(std::same_as<Wire, uint8> || std::same_as<Wire, uint16>);
		return ReadBytes<Wire>(state.tmem, EndianAddress<Wire>(address));
	}

	template <typename Wire>
	void WriteTmem(CommanderState& state, uint16 address, Wire value)
	{
		static_assert(std::same_as<Wire, uint8> || std::same_as<Wire, uint16>);
		WriteBytes<Wire>(state.tmem, EndianAddress<Wire>(address), value);
	}

	static BlendSource From1a(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::PixelAlpha;
		case 1: return BlendSource::MemoryColor;
		case 2: return BlendSource::BlendColor;
		case 3: return BlendSource::FogColor;
		default: return {};
		}
	}

	static BlendSource From1b(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::PixelAlpha;
		case 1: return BlendSource::PrimitiveAlpha;
		case 2: return BlendSource::ShadeAlpha;
		case 3: return BlendSource::Zero;
		default: return {};
		}
	}

	static BlendSource From2a(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::PixelAlpha;
		case 1: return BlendSource::MemoryColor;
		case 2: return BlendSource::BlendColor;
		case 3: return BlendSource::FogColor;
		default: return {};
		}
	}

	static BlendSource From2b(uint8 value)
	{
		switch (value)
		{
		case 0: return BlendSource::OneMinusAlpha;
		case 1: return BlendSource::MemoryAlpha;
		case 2: return BlendSource::One;
		case 3: return BlendSource::Zero;
		default: return {};
		}
	}

	inline uint8 GetBytesPerPixel(const CommanderState& state)
	{
		switch (state.colorImage.size)
		{
		// case 1: return 1;
		case 2: return 2;
		case 3: return 4;
		default: throw std::range_error("size of pixel color element is invalid");
		}
	}

	struct HSpan
	{
		int32 startX;
		int32 endX;
	};

	template <class T>
	concept IEdgeWalker = requires(T t)
	{
		{ t.GetStartY() } -> std::same_as<uint32>;
		{ t.GetEndY() } -> std::same_as<uint32>;
		{ t.GetHSpan(std::declval<uint32>()) } -> std::same_as<const HSpan&>;
	};
}
