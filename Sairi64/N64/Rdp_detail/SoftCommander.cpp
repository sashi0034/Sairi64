﻿#include "stdafx.h"
#include "SoftCommander.h"

#include "N64/N64Logger.h"
#include "Soft/Soft.h"

namespace N64::Rdp_detail
{
	void SoftCommander::FullSync()
	{
		// TODO?
	}


	void SoftCommander::EnqueueCommand(const RdpCommand& cmd)
	{
		N64_TRACE(Rdp, U"enqueued rdp command: {:02X}"_fmt(StringifyEnum(cmd.Id())));

		(void)execute(cmd);
	}

	SoftUnit SoftCommander::execute(const RdpCommand& cmd)
	{
		CommanderContext ctx{
			.state = &m_state
		};

		switch (cmd.Id())
		{
		case CommandId::FillTriangle:
			break;
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
			break;
		case CommandId::ShadeTextureZBufferTriangle:
			break;
		case CommandId::TextureRectangle:
			break;
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
			break;
		case CommandId::LoadBlock:
			break;
		case CommandId::LoadTile:
			break;
		case CommandId::SetTile:
			break;
		case CommandId::FillRectangle:
			break;
		case CommandId::SetFillColor:
			break;
		case CommandId::SetFogColor:
			break;
		case CommandId::SetBlendColor:
			break;
		case CommandId::SetPrimColor:
			break;
		case CommandId::SetEnvColor:
			break;
		case CommandId::SetCombine:
			break;
		case CommandId::SetTextureImage:
			break;
		case CommandId::SetMaskImage:
			break;
		case CommandId::SetColorImage:
			return Soft::SetColorImage(ctx, cmd);
		default: ;
		}

		N64Logger::Abort(U"not implemented rdp command: {}"_fmt(StringifyEnum(cmd.Id())));
		return {};
	}
}
