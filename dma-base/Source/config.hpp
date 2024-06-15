#include <pch.h>

void load_settings_from_ini() {
    std::ifstream ini_file("imgui.ini");
    if (!ini_file.is_open()) return;

    std::string line;
    while (std::getline(ini_file, line)) {
        if (line.find("Aimbot") != std::string::npos) {
            ini_file >> settings::aimbot::enable;
            ini_file >> settings::kmbox::kmboxb;
            ini_file >> settings::kmbox::kmboxnet;
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
