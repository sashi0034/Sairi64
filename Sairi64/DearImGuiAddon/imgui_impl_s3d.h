#pragma once

IMGUI_IMPL_API bool ImGui_ImplS3d_Init();
IMGUI_IMPL_API void ImGui_ImplS3d_Shutdown();
IMGUI_IMPL_API void ImGui_ImplS3d_NewFrame();
IMGUI_IMPL_API void ImGui_ImplS3d_RenderDrawData(ImDrawData* draw_data);

IMGUI_IMPL_API ImTextureID ImGui_ImplS3d_RegisterTexture(Texture& tex);
IMGUI_IMPL_API void ImGui_ImplS3d_UnregisterTexture(Texture& tex);
IMGUI_IMPL_API Texture ImGui_ImplS3d_GetTexture(ImTextureID id);
