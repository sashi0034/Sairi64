#pragma once

#include "../SoftCommander.h"

namespace N64::Rdp_detail::Soft
{
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
