#include "SoftBlender.h"

namespace N64::Rdp_detail::Soft
{
	// https://github.com/ata4/angrylion-rdp-plus/blob/20eaeaffc83ee7a4ca64a533a65f906b82d10dc4/src/core/n64video/rdp/blender.c#L72
	inline bool alphaCompare(const CommanderState& state, uint32 checkingAlpha)
	{
		if (state.otherModes.alphaCompareEn == false)
			return true;
		const uint32 threshold =
			(!state.otherModes.ditherAlphaEn)
			? Color32Bpp(state.blendColor).A()
			: Random(0, 0x7FFF) & 0xff;
		return (checkingAlpha >= threshold);
	}

	Color32Bpp Blend1Cycle(const CommanderState& state)
	{
		return {};
	}
}
