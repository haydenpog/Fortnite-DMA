#include <pch.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include <sdk.hpp>
#include <settings.hpp>
#include <render.hpp>
#include <offsets.hpp>
#include <thread>
#include <atomic>
#include <iostream>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

std::atomic<bool> running(true);
std::ofstream log_file("actorloop_log.txt");
std::mutex data_mutex;

void log(const std::string& message) {
    log_file << message << std::endl;
}

struct EntityData {
    Vector2 head2d;
    Vector2 neck2d;
    Vector2 bottom2d;
    int box_width;
    int box_height;
    float distance;
    uintptr_t mesh;
};

std::vector<EntityData> entities;

void actorloop() 
{
    while (running) 
    {
        try 
            {
                cache::base = mem.GetBaseAddress("FortniteClient-Win64-Shipping.exe");
                cache::uworld = mem.Read<uintptr_t>(cache::base + offsets::UWORLD);
                if (!cache::uworld) continue;

                cache::game_instance = mem.Read<uintptr_t>(cache::uworld + offsets::GAME_INSTANCE);
                cache::local_players = mem.Read<uintptr_t>(mem.Read<uintptr_t>(cache::game_instance + offsets::LOCAL_PLAYERS));
                cache::player_controller = mem.Read<uintptr_t>(cache::local_players + offsets::PLAYER_CONTROLLER);
                cache::local_pawn = mem.Read<uintptr_t>(cache::player_controller + offsets::LOCAL_PAWN);

                if (cache::local_pawn) {
                    cache::root_component = mem.Read<uintptr_t>(cache::local_pawn + offsets::ROOT_COMPONENT);
                    cache::relative_location = mem.Read<Vector3>(cache::root_component + offsets::RELATIVE_LOCATION);
                    cache::player_state = mem.Read<uintptr_t>(cache::local_pawn + offsets::PLAYER_STATE);
                    cache::my_team_id = mem.Read<int>(cache::player_state + offsets::TEAM_INDEX);
                }

                cache::game_state = mem.Read<uintptr_t>(cache::uworld + offsets::GAME_STATE);
                cache::player_array = mem.Read<uintptr_t>(cache::game_state + offsets::PLAYER_ARRAY);
                cache::player_count = mem.Read<int>(cache::game_state + (offsets::PLAYER_ARRAY + sizeof(uintptr_t)));

                cache::closest_distance = FLT_MAX;
                cache::closest_mesh = NULL;
                std::vector<EntityData> temp_entities;

                for (int i = 0; i < cache::player_count; i++) {
                    uintptr_t player_state = mem.Read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
                    if (!player_state) continue;

                    int player_team_id = mem.Read<int>(player_state + offsets::TEAM_INDEX);
                    if (player_team_id == cache::my_team_id) continue;

                    uintptr_t pawn_private = mem.Read<uintptr_t>(player_state + offsets::PAWN_PRIVATE);
                    if (!pawn_private || pawn_private == cache::local_pawn) continue;

                    uintptr_t mesh = mem.Read<uintptr_t>(pawn_private + offsets::MESH);
                    if (!mesh) continue;

                    cache::head3d = get_entity_bone(mesh, bone::BONE_HEAD);
                    cache::neck3d = get_entity_bone(mesh, bone::BONE_LOWERHEAD);
                    cache::head2d = project_world_to_screen(cache::head3d);
                    cache::neck2d = project_world_to_screen(cache::neck3d);
                    Vector3 bottom3d = get_entity_bone(mesh, 0);
                    cache::bottom2d = project_world_to_screen(bottom3d);
                    cache::box_height = abs(cache::head2d.y - cache::bottom2d.y);
                    cache::box_width = static_cast<int>(cache::box_height * 0.50f);
                    cache::distance = cache::relative_location.distance(bottom3d) / 100.0f;
                    cache::box_height = abs(cache::head2d.y - cache::bottom2d.y);
                    cache::box_width = static_cast<int>(cache::box_height * 0.50f);

                    temp_entities.push_back(EntityData(
                        cache::head2d,
                        cache::neck2d,
                        cache::bottom2d,
                        cache::box_width,
                        cache::box_height,
                        cache::distance,
                        mesh
                    ));
                }

                {
                    std::lock_guard<std::mutex> lock(data_mutex);
                    entities = std::move(temp_entities);
                }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void render_entities() 
{
    std::lock_guard<std::mutex> lock(data_mutex);

    for (const auto& entity : entities)
    {
        ImColor box_color = is_visible(entity.mesh)
            ? ImColor(settings::visuals::boxColor[0], settings::visuals::boxColor[1], settings::visuals::boxColor[2], settings::visuals::boxColor[3])
            : ImColor(settings::visuals::boxColor2[0], settings::visuals::boxColor2[1], settings::visuals::boxColor2[2], settings::visuals::boxColor2[3]);

        if (settings::visuals::box) {
            draw_cornered_box(entity.head2d.x - (entity.box_width / 2), entity.head2d.y, entity.box_width, entity.box_height, box_color, 1);
        }
        if (settings::visuals::fill_box) {
            ImColor fill_color = box_color;
            fill_color.Value.w = 0.5f;
            draw_filled_rect(entity.head2d.x - (entity.box_width / 2), entity.head2d.y, entity.box_width, entity.box_height, fill_color);
        }
        if (settings::visuals::line) {
            draw_line(entity.bottom2d, box_color);
        }
        if (settings::visuals::skeleton) {
            skeleton(entity.mesh, box_color);
        }
        if (settings::visuals::distance) {
            draw_distance(entity.bottom2d, entity.distance, ImColor(250, 250, 250, 250));
        }
    }
}


WPARAM render_loop() {
    ZeroMemory(&messager, sizeof(MSG));
    while (messager.message != WM_QUIT) {
        if (PeekMessage(&messager, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&messager);
            DispatchMessage(&messager);
        }
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = 1.0f / 60.0f;
        POINT p;
        GetCursorPos(&p);
        io.MousePos.x = static_cast<float>(p.x);
        io.MousePos.y = static_cast<float>(p.y);
        io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) != 0;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        render_entities();
        render_menu();

        ImGui::EndFrame();
        p_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

        p_device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        if (p_device->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            p_device->EndScene();
        }
        HRESULT result = p_device->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            p_device->Reset(&p_params);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
    }
    return messager.wParam;
}
bool init()
{
    create_overlay();
    return SUCCEEDED(directx_init());
}