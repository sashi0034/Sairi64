﻿#include "stdafx.h"
#include "SoftCommander.h"

#include "N64/N64Logger.h"
#include "N64/N64System.h"
#include "Soft/Soft.h"

namespace N64::Rdp_detail
{
	void SoftCommander::FullSync()
	{
		// TODO?
	}

	void SoftCommander::EnqueueCommand(N64System& n64, const RdpCommand& cmd)
	{
		N64_TRACE(Rdp, U"enqueued rdp command: {}"_fmt(StringifyEnum(cmd.Id())));

		const CommanderContext ctx{
			.state = &m_state,
			.rdram = n64.GetMemory().Rdram()
		};
		(void)execute(ctx, cmd);
	}

	inline void unimplementedCommand(CommandId cmd)
	{
		static bool warned[64]{};
		if (warned[static_cast<int>(cmd)]) return;
		warned[static_cast<int>(cmd)] = true;
		N64Logger::Warn(U"not implemented rdp command: {}"_fmt(StringifyEnum(cmd)));
	}

	SoftUnit SoftCommander::execute(const CommanderContext& ctx, const RdpCommand& cmd)
	{
		using Soft::Soft;

		switch (cmd.Id())
		{
		case CommandId::NonShadedTriangle:
			return Soft::NonShadedTriangle(ctx, cmd);
		case CommandId::FillZBufferTriangle:
			break;
		case CommandId::TextureTriangle:
			break;
		case CommandId::TextureZBufferTriangle:
			break;
		case CommandId::ShadeTriangle:
			break;
		case CommandId::ShadeZBufferTriangle:
			break;
		case CommandId::ShadeTextureTriangle:
			return Soft::ShadeTextureTriangle(ctx, cmd);
		case CommandId::ShadeTextureZBufferTriangle:
			return Soft::ShadeTextureZBufferTriangle(ctx, cmd);
		case CommandId::TextureRectangle:
			return Soft::TextureRectangle(ctx, cmd);
		case CommandId::TextureRectangleFlip:
			break;
		case CommandId::SyncLoad:
			return {}; // TODO?
		case CommandId::SyncPipe:
			return {}; // TODO?
		case CommandId::SyncTile:
			return {}; // TODO?
		case CommandId::SyncFull:
			return {}; // TODO?
		case CommandId::SetKeyGb:
			break;
		case CommandId::SetKeyR:
			break;
		case CommandId::SetConvert:
			break;
		case CommandId::SetScissor:
			return Soft::SetScissor(ctx, cmd);
		case CommandId::SetPrimDepth:
			break;
		case CommandId::SetOtherModes:
			return Soft::SetOtherModes(ctx, cmd);
		case CommandId::LoadTLut:
			break;
		case CommandId::SetTileSize:
			return Soft::SetTileSize(ctx, cmd);
		case CommandId::LoadBlock:
			return Soft::LoadBlock(ctx, cmd);
		case CommandId::LoadTile:
			return Soft::LoadTile(ctx, cmd);
		case CommandId::SetTile:
			return Soft::SetTile(ctx, cmd);
		case CommandId::FillRectangle:
			return Soft::FillRectangle(ctx, cmd);
		case CommandId::SetFillColor:
			return Soft::SetFillColor(ctx, cmd);
		case CommandId::SetFogColor:
			break;
		case CommandId::SetBlendColor:
			return Soft::SetBlendColor(ctx, cmd);
		case CommandId::SetPrimColor:
			break;
		case CommandId::SetEnvColor:
			break;
		case CommandId::SetCombine:
			return Soft::SetCombine(ctx, cmd);
		case CommandId::SetTextureImage:
			return Soft::SetTextureImage(ctx, cmd);
		case CommandId::SetMaskImage:
			return Soft::SetMaskImage(ctx, cmd);
		case CommandId::SetColorImage:
			return Soft::SetColorImage(ctx, cmd);
		default: ;
		}

		unimplementedCommand(cmd.Id());
		return {};
	}
}
