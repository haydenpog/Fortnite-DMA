#include <pch.h>
#include <sdk.hpp>
#include <game.hpp>
#include <dwmapi.h>
#include <settings.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")


IDirect3D9Ex* p_object = nullptr;
IDirect3DDevice9Ex* p_device = nullptr;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { nullptr };
HWND my_wnd = nullptr;

HRESULT directx_init()
{
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object)))
        exit(3);

    ZeroMemory(&p_params, sizeof(p_params));
    p_params.Windowed = TRUE;
    p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p_params.hDeviceWindow = my_wnd;
    p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
    p_params.BackBufferWidth = 1920;
    p_params.BackBufferHeight = 1080;
    p_params.EnableAutoDepthStencil = TRUE;
    p_params.AutoDepthStencilFormat = D3DFMT_D16;
    p_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
    {
        p_object->Release();
        exit(4);
    }

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(my_wnd);
    ImGui_ImplDX9_Init(p_device);
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::GetStyle();

    p_object->Release();
    return S_OK;
}

void create_overlay()
{
    WNDCLASSEXA wcex = {
        sizeof(WNDCLASSEXA),
        0,
        DefWindowProcA,
        0,
        0,
        0,
        LoadIcon(0, IDI_APPLICATION),
        LoadCursor(0, IDC_ARROW),
        0,
        0,
        "OverlayWindowClass",
        LoadIcon(0, IDI_APPLICATION)
    };
    RegisterClassExA(&wcex);

    my_wnd = CreateWindowExA(
        0,
        "OverlayWindowClass",
        "OverlayWindowTitle",
        WS_POPUP | WS_VISIBLE,
        0, 0,
        1920, 1080,
        nullptr, nullptr,
        wcex.hInstance, nullptr
    );

    SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);
    SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(my_wnd, &margin);
    ShowWindow(my_wnd, SW_SHOW);
    UpdateWindow(my_wnd);
}


void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
    auto draw_list = ImGui::GetForegroundDrawList();
    draw_list->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0.0f, ImDrawFlags_RoundCornersNone, thickness);
}

void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color);
}

void draw_line(Vector2 target, const ImColor color)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

void draw_distance(Vector2 location, float distance, const ImColor color)
{
    char dist[64];
    sprintf_s(dist, "%.fm", distance);
    ImVec2 text_size = ImGui::CalcTextSize(dist);
    ImGui::GetForegroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
}

void skeleton(uintptr_t skeleton_mesh, const ImColor color)
{
    if (!skeleton_mesh || settings::visuals::skeleton) {

        auto neck = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_HEAD));
        auto chest = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_NECK));
        auto left_shoulder = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_LSHOULDER));
        auto left_elbow = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_LELBOW));
        auto left_hand = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_LHAND));
        auto right_shoulder = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_RSHOULDER));
        auto right_elbow = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_RELBOW));
        auto right_hand = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_RHAND));
        auto pelvis = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_PELVIS));
        auto left_hip = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_LTHIGH));
        auto left_knee = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_LKNEE));
        auto left_foot = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_LFOOT));
        auto right_hip = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_RTHIGH));
        auto right_knee = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_RKNEE));
        auto right_foot = project_world_to_screen(get_entity_bone(skeleton_mesh, perfect_skeleton::BONE_RFOOT));

        if (settings::visuals::skeleton) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(chest.x, chest.y), ImVec2(right_shoulder.x, right_shoulder.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(left_shoulder.x, left_shoulder.y), ImVec2(chest.x, chest.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(left_shoulder.x, left_shoulder.y), ImVec2(left_elbow.x, left_elbow.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(left_elbow.x, left_elbow.y), ImVec2(left_hand.x, left_hand.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(right_shoulder.x, right_shoulder.y), ImVec2(right_elbow.x, right_elbow.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(right_elbow.x, right_elbow.y), ImVec2(right_hand.x, right_hand.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(left_hip.x, left_hip.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(left_hip.x, left_hip.y), ImVec2(left_knee.x, left_knee.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(left_knee.x, left_knee.y), ImVec2(left_foot.x, left_foot.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(right_hip.x, right_hip.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(right_hip.x, right_hip.y), ImVec2(right_knee.x, right_knee.y), color, settings::visuals::skeleton_thick + 2.5);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(right_knee.x, right_knee.y), ImVec2(right_foot.x, right_foot.y), color, settings::visuals::skeleton_thick + 2.5);
        }
    }
}

void render_menu()
{
    ImGuiIO& io = ImGui::GetIO();
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time);
    if (settings::visuals::fps) {
        int frame_duration = 1000 / settings::visuals::fps_limit;
        if (elapsed.count() < frame_duration) {
            std::this_thread::sleep_for(std::chrono::milliseconds(frame_duration - elapsed.count()));
        }
    }
    last_time = std::chrono::high_resolution_clock::now();
    if (settings::visuals::fps)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::End();
    }
    if (settings::aimbot::show_fov)
        ImGui::GetForegroundDrawList()->AddCircle(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), settings::aimbot::fov, ImColor(250, 250, 250, 250), 100, 1.0f);

    if (GetAsyncKeyState(VK_INSERT) & 1)
        settings::show_menu = !settings::show_menu;

    if (settings::show_menu)
    {
        ImGui::SetNextWindowSize({ 620, 350 });
        ImGui::Begin("Jouh", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
        if (ImGui::Button("Aimbot", { 196, 20 })) settings::tab = 0;
        ImGui::SameLine();
        if (ImGui::Button("Visuals", { 196, 20 })) settings::tab = 1;
        ImGui::SameLine();
        if (ImGui::Button("Misc", { 196, 20 })) settings::tab = 2;

        switch (settings::tab)
        {
        case 0:
            ImGui::Checkbox("Enable Aimbot", &settings::aimbot::enable);
            if (settings::aimbot::enable)
            {
                if (ImGui::Checkbox("Kmbox B+", &settings::kmbox::kmboxb))
                {
                    if (!kmBox::init())
                    {
                        settings::kmbox::kmboxb = false;
                    }
                }
                if (settings::kmbox::kmboxb)
                {
                    ImGui::Checkbox("Show FOV Circle", &settings::aimbot::show_fov);
                    ImGui::SliderFloat("FOV Radius", &settings::aimbot::fov, 50.0f, 300.0f, "%.2f");
                    ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 1.0f, 10.0f, "%.2f");
                }
                ImGui::Checkbox("Kmbox Net", &settings::kmbox::kmboxnet);
                if (settings::kmbox::kmboxnet)
                {
                    ImGui::InputText("Kmbox IP", settings::kmbox::KmboxIp, IM_ARRAYSIZE(settings::kmbox::KmboxIp));
                    ImGui::InputText("Kmbox Port", settings::kmbox::KmboxPort, IM_ARRAYSIZE(settings::kmbox::KmboxPort));
                    ImGui::InputText("Kmbox UUID", settings::kmbox::KmboxUUID, IM_ARRAYSIZE(settings::kmbox::KmboxUUID));
                    if (ImGui::Button("Connect to Kmbox .NET"))
                    {
                        if (!kmNet_init(settings::kmbox::KmboxIp, settings::kmbox::KmboxPort, settings::kmbox::KmboxUUID))
                        {
                            settings::kmbox::kmboxnet = false;
                        }
                    }
                    if (settings::kmbox::kmboxnet)
                    {
                        ImGui::Checkbox("Show FOV Circle", &settings::aimbot::show_fov);
                        ImGui::SliderFloat("FOV Radius", &settings::aimbot::fov, 50.0f, 300.0f, "%.2f");
                        ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 1.0f, 10.0f, "%.2f");
                    }
                }
            }
            break;

        case 1:
            ImGui::Checkbox("Enable", &settings::visuals::enable);
            ImGui::Checkbox("Box", &settings::visuals::box);
            if (settings::visuals::box)
            {
                ImGui::ColorEdit4("Visible", settings::visuals::boxColor);
                ImGui::ColorEdit4("Non-Visible", settings::visuals::boxColor2);
            }
            ImGui::Checkbox("Fill Box", &settings::visuals::fill_box);
            ImGui::Checkbox("skeleton", &settings::visuals::skeleton);
            ImGui::Checkbox("Line", &settings::visuals::line);
            ImGui::Checkbox("Distance", &settings::visuals::distance);
            break;

        case 2:
            ImGui::Checkbox("FPS", &settings::visuals::fps);
            if (settings::visuals::fps)
            {
                ImGui::SliderInt("FPS Limit", &settings::visuals::fps_limit, 30, 240);
            }
            if (ImGui::Button("Unload Cheat", { 120, 20 })) exit(0);
            break;
        }
        ImGui::End();
    }
    ImGui::End();
}

