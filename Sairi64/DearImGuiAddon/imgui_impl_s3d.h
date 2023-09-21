#pragma once
struct imgui_impl_s3d;

IMGUI_IMPL_API bool ImGui_Impl_s3d_Init();
IMGUI_IMPL_API void ImGui_Impl_s3d_Shutdown();
IMGUI_IMPL_API void ImGui_Impl_s3d_NewFrame();
IMGUI_IMPL_API void ImGui_Impl_s3d_RenderDrawData(ImDrawData* draw_data);

IMGUI_IMPL_API ImTextureID ImGui_Impl_s3d_RegisterTexture(Texture& tex);
IMGUI_IMPL_API void ImGui_Impl_s3d_UnregisterTexture(Texture& tex);
IMGUI_IMPL_API Texture ImGui_Impl_s3d_GetTexture(ImTextureID id);
