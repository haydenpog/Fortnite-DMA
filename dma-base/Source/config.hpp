#include <pch.h>
void style() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 5.0f;

    style.Colors[ImGuiCol_Text] = ImVec4(settings::misc::textColor[0], settings::misc::textColor[1], settings::misc::textColor[2], settings::misc::textColor[3]);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(settings::misc::bgColor[0], settings::misc::bgColor[1], settings::misc::bgColor[2], settings::misc::bgColor[3]);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(settings::misc::bgColor[0], settings::misc::bgColor[1], settings::misc::bgColor[2], settings::misc::bgColor[3]);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.23f, 0.24f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

    style.Colors[ImGuiCol_CheckMark] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_Button] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_Header] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(settings::misc::blueColor[0], settings::misc::blueColor[1], settings::misc::blueColor[2], settings::misc::blueColor[3]);
}



void load_settings_from_ini() {
    std::ifstream ini_file("imgui.ini");
    if (!ini_file.is_open()) return;

    std::string line;
    while (std::getline(ini_file, line)) {
        if (line.find("Aimbot") != std::string::npos) {
            ini_file >> settings::aimbot::enable;
            ini_file >> settings::aimbot::projectileSpeed;
            ini_file >> settings::aimbot::fov;
            ini_file >> settings::aimbot::smoothness;
            ini_file >> settings::aimbot::show_fov;
            ini_file >> settings::aimbot::triggerbot;
        }
        else if (line.find("Visuals") != std::string::npos) {
            ini_file >> settings::visuals::enable;
            ini_file >> settings::visuals::box;
            ini_file >> settings::visuals::fill_box;
            ini_file >> settings::visuals::skeleton;
            ini_file >> settings::visuals::line;
            ini_file >> settings::visuals::distance;
            for (int i = 0; i < 4; i++) ini_file >> settings::visuals::boxColor[i];
            for (int i = 0; i < 4; i++) ini_file >> settings::visuals::boxColor2[i];
        }
        else if (line.find("Misc") != std::string::npos) {
            ini_file >> settings::visuals::fps;
            ini_file >> settings::misc::zero_alpha;
            ini_file >> settings::misc::color;
            for (int i = 0; i < 4; i++) ini_file >> settings::misc::textColor[i];
            for (int i = 0; i < 4; i++) ini_file >> settings::misc::bgColor[i];
            for (int i = 0; i < 4; i++) ini_file >> settings::misc::blueColor[i];
        }
    }
    ini_file.close();
}

void save_settings_to_ini() {
    std::ofstream ini_file("imgui.ini");
    if (!ini_file.is_open()) return;

    ini_file << "[Aimbot]\n";
    ini_file << settings::aimbot::enable << "\n";
    ini_file << settings::aimbot::projectileSpeed << "\n";
    ini_file << settings::aimbot::fov << "\n";
    ini_file << settings::aimbot::smoothness << "\n";
    ini_file << settings::aimbot::show_fov << "\n";
    ini_file << settings::aimbot::triggerbot << "\n";

    ini_file << "[Visuals]\n";
    ini_file << settings::visuals::enable << "\n";
    ini_file << settings::visuals::box << "\n";
    ini_file << settings::visuals::fill_box << "\n";
    ini_file << settings::visuals::skeleton << "\n";
    ini_file << settings::visuals::line << "\n";
    ini_file << settings::visuals::distance << "\n";
    for (int i = 0; i < 4; i++) ini_file << settings::visuals::boxColor[i] << " ";
    ini_file << "\n";
    for (int i = 0; i < 4; i++) ini_file << settings::visuals::boxColor2[i] << " ";
    ini_file << "\n";

    ini_file << "[Misc]\n";
    ini_file << settings::visuals::fps << "\n";
    ini_file << settings::misc::zero_alpha << "\n";
    ini_file << settings::misc::color << "\n";
    for (int i = 0; i < 4; i++) ini_file << settings::misc::textColor[i] << " ";
    ini_file << "\n";
    for (int i = 0; i < 4; i++) ini_file << settings::misc::bgColor[i] << " ";
    ini_file << "\n";
    for (int i = 0; i < 4; i++) ini_file << settings::misc::blueColor[i] << " ";
    ini_file << "\n";

    ini_file.close();
}
