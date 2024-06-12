﻿#include <pch.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <sdk.hpp>
#include <settings.hpp>
#include <render.hpp>
#include <offsets.hpp>

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
std::vector<EntityData> entities_render;
std::atomic<bool> running(true);
std::atomic<int> player_count(0);
std::mutex data_mutex;

void bases() {
    while (running) {
        cache::uworld = mem.Read<uintptr_t>(cache::base + offsets::UWORLD);

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
        player_count.store(mem.Read<int>(cache::game_state + (offsets::PLAYER_ARRAY + sizeof(uintptr_t))));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void actorloop() {
    while (running) {
        std::vector<EntityData> temp_entities;
        int count = player_count.load();
        temp_entities.reserve(count);

        for (int i = 0; i < count; i++) {
            uintptr_t player_state = mem.Read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
            if (!player_state) continue;

            int player_team_id = mem.Read<int>(player_state + offsets::TEAM_INDEX);
            if (player_team_id == cache::my_team_id) continue;

            uintptr_t pawn_private = mem.Read<uintptr_t>(player_state + offsets::PAWN_PRIVATE);
            if (!pawn_private || pawn_private == cache::local_pawn) continue;

            uintptr_t mesh = mem.Read<uintptr_t>(pawn_private + offsets::MESH);
            if (!mesh) continue;

            cache::head3d = get_entity_bone(mesh, bone::BONE_HEAD);
            Vector3 neck3d = get_entity_bone(mesh, bone::BONE_LOWERHEAD);
            Vector2 head2d = project_world_to_screen(cache::head3d);
            Vector2 neck2d = project_world_to_screen(neck3d);
            Vector3 bottom3d = get_entity_bone(mesh, 0);
            Vector2 bottom2d = project_world_to_screen(bottom3d);
            int box_height = abs(head2d.y - bottom2d.y);
            int box_width = static_cast<int>(box_height * 0.50f);
            float distance = cache::relative_location.distance(bottom3d) / 100.0f;

            temp_entities.push_back(EntityData{
                head2d,
                neck2d,
                bottom2d,
                box_width,
                box_height,
                distance,
                mesh
                });
        }

        {
            std::lock_guard<std::mutex> lock(data_mutex);
            entities.swap(temp_entities);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }
}

void render_entities() {
    std::lock_guard<std::mutex> lock(data_mutex);
    entities_render = entities; 
}

void draw_entities() {
    std::lock_guard<std::mutex> lock(data_mutex);

    for (const auto& entity : entities_render) {
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
        draw_entities();
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

bool init() {
    create_overlay();
    return SUCCEEDED(directx_init());
}