#include <pch.h>
#include <sdk.hpp>
#include <dwmapi.h>
#include <settings.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <chrono>
#include <thread>

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
        return E_FAIL;

    ZeroMemory(&p_params, sizeof(p_params));
    p_params.Windowed = TRUE;
    p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p_params.hDeviceWindow = my_wnd;
    p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
    p_params.BackBufferWidth = settings::width;
    p_params.BackBufferHeight = settings::height;
    p_params.EnableAutoDepthStencil = TRUE;
    p_params.AutoDepthStencilFormat = D3DFMT_D16;
    p_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, nullptr, &p_device)))
    {
        p_object->Release();
        return E_FAIL;
    }

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(my_wnd);
    ImGui_ImplDX9_Init(p_device);
    ImGui::GetIO().IniFilename = nullptr;

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
        settings::width, settings::height,
        nullptr, nullptr,
        wcex.hInstance, nullptr
    );

    SetWindowLong(my_wnd, GWL_EXSTYLE, GetWindowLong(my_wnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(my_wnd, &margin);
    ShowWindow(my_wnd, SW_SHOW);
    UpdateWindow(my_wnd);
}


const int radar_pos_x = 0;
const int radar_pos_y = 0;
const int radar_size = 500;
const float radar_range = 1000.0f;  // Define radar range

void RadarRange(float* x, float* y, float range)
{
    if (fabs((*x)) > range || fabs((*y)) > range)
    {
        if ((*y) > (*x))
        {
            if ((*y) > -(*x))
            {
                (*x) = range * (*x) / (*y);
                (*y) = range;
            }
            else
            {
                (*y) = -range * (*y) / (*x);
                (*x) = -range;
            }
        }
        else
        {
            if ((*y) > -(*x))
            {
                (*y) = range * (*y) / (*x);
                (*x) = range;
            }
            else
            {
                (*x) = -range * (*x) / (*y);
                (*y) = -range;
            }
        }
    }
}

void CalcRadarPoint(Vector2 vOrigin, int& screenx, int& screeny)
{
    Camera view_point = get_view_point();

    Vector2 vAngle;
    vAngle.x = view_point.rotation.x;
    vAngle.y = view_point.rotation.y;
    auto fYaw = vAngle.y * M_PI / 180.0f;

    float dx = vOrigin.x - view_point.location.x;
    float dy = vOrigin.y - view_point.location.y;

    float fsin_yaw = sinf(fYaw);
    float fminus_cos_yaw = -cosf(fYaw);

    float x = dy * fminus_cos_yaw + dx * fsin_yaw;
    x = -x;
    float y = dx * fminus_cos_yaw - dy * fsin_yaw;

    float range = radar_range * 1000.f;

    RadarRange(&x, &y, range);

    ImVec2 DrawPos = ImVec2(radar_pos_x, radar_pos_y);
    ImVec2 DrawSize = ImVec2(radar_size, radar_size);

    int rad_x = (int)DrawPos.x;
    int rad_y = (int)DrawPos.y;

    float r_siz_x = DrawSize.x;
    float r_siz_y = DrawSize.y;

    int x_max = (int)r_siz_x + rad_x - 5;
    int y_max = (int)r_siz_y + rad_y - 5;

    screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
    screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

    if (screenx > x_max)
        screenx = x_max;

    if (screenx < rad_x)
        screenx = rad_x;

    if (screeny > y_max)
        screeny = y_max;

    if (screeny < rad_y)
        screeny = rad_y;
}

void render_radar_main() {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(radar_pos_x, radar_pos_y), ImVec2(radar_pos_x + radar_size, radar_pos_y + radar_size), ImGui::GetColorU32({ 0.13f, 0.13f, 0.13f, 0.7f }), 0.f, 0);
    ImVec2 center = ImVec2(radar_pos_x + (radar_size / 2), radar_pos_y + (radar_size / 2));
    float orangeCircleRadius = 2.3f;
    ImGui::GetBackgroundDrawList()->AddCircleFilled(center, orangeCircleRadius, ImGui::GetColorU32({ 1.f, 0.5f, 0.f, 1.f }), 12);
}

void add_to_radar(Vector2 WorldLocation, bool visible) {
    int ScreenX = 0, ScreenY = 0;
    CalcRadarPoint(WorldLocation, ScreenX, ScreenY);

    if (!visible)
    {
        ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenX, ScreenY), 6.f, ImGui::GetColorU32({ 1.f, 0.f, 0.f, 1.f }), 12);
    }
    else
    {
        ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenX, ScreenY), 6.f, ImGui::GetColorU32({ 0.f, 1.f, 0.f, 1.f }), 12);
    }

    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ScreenX, ScreenY), 6.f, ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 12, 1.f);
}

void draw_cornered_box(int x, int y, int w, int h, ImColor color, int thickness)
{

    ImGui::GetForegroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 3.0f, 0, thickness);
}

void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color);
}

void draw_line(const Vector2& target, const ImColor color)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

void draw_distance(const Vector2& location, float distance, const ImColor color)
{
    char dist[64];
    sprintf_s(dist, "%.fm", distance);
    ImVec2 text_size = ImGui::CalcTextSize(dist);
    ImGui::GetForegroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
}

void skeleton(uintptr_t skeleton_mesh, const ImColor color)
{
        if (!skeleton_mesh || !settings::visuals::skeleton) return;

        auto neck = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_HEAD));
        auto chest = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_NECK));
        auto left_shoulder = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_LSHOULDER));
        auto left_elbow = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_LELBOW));
        auto left_hand = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_LHAND));
        auto right_shoulder = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_RSHOULDER));
        auto right_elbow = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_RELBOW));
        auto right_hand = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_RHAND));
        auto pelvis = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_PELVIS));
        auto left_hip = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_LTHIGH));
        auto left_knee = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_LKNEE));
        auto left_foot = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_LFOOT));
        auto right_hip = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_RTHIGH));
        auto right_knee = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_RKNEE));
        auto right_foot = project_world_to_screen(get_entity_bone(skeleton_mesh, bone::BONE_RFOOT));

        auto draw_list = ImGui::GetBackgroundDrawList();
        auto thickness = settings::visuals::skeleton_thick + 2.5f;

        draw_list->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), color, thickness);
        draw_list->AddLine(ImVec2(chest.x, chest.y), ImVec2(right_shoulder.x, right_shoulder.y), color, thickness);
        draw_list->AddLine(ImVec2(left_shoulder.x, left_shoulder.y), ImVec2(chest.x, chest.y), color, thickness);
        draw_list->AddLine(ImVec2(left_shoulder.x, left_shoulder.y), ImVec2(left_elbow.x, left_elbow.y), color, thickness);
        draw_list->AddLine(ImVec2(left_elbow.x, left_elbow.y), ImVec2(left_hand.x, left_hand.y), color, thickness);
        draw_list->AddLine(ImVec2(right_shoulder.x, right_shoulder.y), ImVec2(right_elbow.x, right_elbow.y), color, thickness);
        draw_list->AddLine(ImVec2(right_elbow.x, right_elbow.y), ImVec2(right_hand.x, right_hand.y), color, thickness);
        draw_list->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), color, thickness);
        draw_list->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(left_hip.x, left_hip.y), color, thickness);
        draw_list->AddLine(ImVec2(left_hip.x, left_hip.y), ImVec2(left_knee.x, left_knee.y), color, thickness);
        draw_list->AddLine(ImVec2(left_knee.x, left_knee.y), ImVec2(left_foot.x, left_foot.y), color, thickness);
        draw_list->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(right_hip.x, right_hip.y), color, thickness);
        draw_list->AddLine(ImVec2(right_hip.x, right_hip.y), ImVec2(right_knee.x, right_knee.y), color, thickness);
        draw_list->AddLine(ImVec2(right_knee.x, right_knee.y), ImVec2(right_foot.x, right_foot.y), color, thickness);
   
}


std::string read_file(const std::string& filename, int line_number) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }

    std::string line;
    int current_line = 0;
    while (std::getline(file, line)) {
        if (++current_line == line_number) {
            return line;
        }
    }
    return "";
}

void case0()
{
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

        if (ImGui::Checkbox("Kmbox Net", &settings::kmbox::kmboxnet))
        {
            std::string ip = read_file("kmnet.txt", 1);
            std::string port = read_file("kmnet.txt", 2);
            std::string uuid = read_file("kmnet.txt", 3);

            strncpy(settings::kmbox::kmbox_ip, ip.c_str(), sizeof(settings::kmbox::kmbox_ip) - 1);
            settings::kmbox::kmbox_ip[sizeof(settings::kmbox::kmbox_ip) - 1] = '\0';

            strncpy(settings::kmbox::kmbox_port, port.c_str(), sizeof(settings::kmbox::kmbox_port) - 1);
            settings::kmbox::kmbox_port[sizeof(settings::kmbox::kmbox_port) - 1] = '\0'; 

            strncpy(settings::kmbox::kmbox_uuid, uuid.c_str(), sizeof(settings::kmbox::kmbox_uuid) - 1);
            settings::kmbox::kmbox_uuid[sizeof(settings::kmbox::kmbox_uuid) - 1] = '\0';

            kmNet_init(settings::kmbox::kmbox_ip, settings::kmbox::kmbox_port, settings::kmbox::kmbox_uuid);
        }
        if ((settings::kmbox::kmboxb) || (settings::kmbox::kmboxnet))
        {
            settings::aimbot::scaledProjectileSpeed = settings::aimbot::projectileSpeed * 1000.0f;
            ImGui::Checkbox("Show FOV Circle", &settings::aimbot::show_fov);
            ImGui::Checkbox("Triggerbot", &settings::aimbot::triggerbot);
            ImGui::SliderFloat("Prediction Speed", &settings::aimbot::projectileSpeed, 10.0f, 100.0f);
            ImGui::SliderFloat("FOV Radius", &settings::aimbot::fov, 50.0f, 300.0f, "%.2f");
            ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 1.0f, 10.0f, "%.2f");
        }
    }
}

void case1()
{
    ImGui::Checkbox("Enable", &settings::visuals::enable);
    ImGui::Checkbox("Box", &settings::visuals::box);
    if (settings::visuals::box)
    {
        ImGui::ColorEdit4("Visible", settings::visuals::boxColor);
        ImGui::ColorEdit4("Non-Visible", settings::visuals::boxColor2);
    }
    ImGui::Checkbox("Fill Box", &settings::visuals::fill_box);
    ImGui::Checkbox("Skeleton", &settings::visuals::skeleton);
    ImGui::Checkbox("Line", &settings::visuals::line);
    ImGui::Checkbox("Distance", &settings::visuals::distance);
}

void case2()
{
    ImGui::Checkbox("FPS", &settings::visuals::fps);
    if (ImGui::Button("Unload Cheat", { 120, 20 })) exit(0);
    ImGui::Checkbox("Transparent", &settings::misc::zero_alpha);
    if (settings::misc::zero_alpha)
    {
        SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    }
    else
    {
        SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    }
}

void render_menu()
{
    
    ImGuiIO& io = ImGui::GetIO();

    if (settings::visuals::fps)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::End();
    }
    

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        settings::show_menu = !settings::show_menu;
    }

    if (settings::aimbot::show_fov) {
        ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(250, 250, 250, 250), 100, 1.0f);
    }

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
            case0();
            break;

        case 1:
            case1();
            break;

        case 2:
            case2();
            break;
        }
        ImGui::End();
    }
    ImGui::EndFrame();
}
