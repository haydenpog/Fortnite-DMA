#pragma once
namespace settings
{
	inline int width = GetSystemMetrics(SM_CXSCREEN);
	inline int height = GetSystemMetrics(SM_CYSCREEN);
	inline int screen_center_x = 1920 / 2;
	inline int screen_center_y = 1080 / 2;
	inline bool show_menu = true;
	inline int tab = 0;
	
	namespace aimbot
	{
		inline bool enable = false;
		inline bool show_fov = false;
		inline float fov = 150;
		inline float smoothness = 5;
		inline bool prediction = false;
		inline bool headbone = false;
		inline bool chestbone = false;
	}
	
	namespace visuals
	{
		inline bool enable = false;
		inline bool box = false;
		inline bool fill_box = false;
		float boxColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float boxColor2[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float boxColor3[4] = { 1.0f, 1.0f, 1.0f, 50.0f };
		inline bool line = false;
		inline bool distance = false;
		inline bool fps = false;
		inline bool skeleton = false;
		inline int skeleton_thick = 1.5f;
		bool vsync = true;
		int fps_limit = 60;

	}
	
	namespace kmbox
	{
		inline bool kmboxb = false;
		inline bool kmboxnet = false;
		inline bool confirm = false;
		char KmboxIp[128] = "";
		char KmboxPort[128] = "";
		char KmboxUUID[128] = "";
	}
}

auto last_time = std::chrono::high_resolution_clock::now();