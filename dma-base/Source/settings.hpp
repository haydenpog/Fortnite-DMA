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
	}
	
	namespace visuals
	{
		inline bool enable = false;
		inline bool box = false;
		inline bool fill_box = false;
		inline bool line = false;
		inline bool distance = false;
	}
	
	namespace kmbox
	{
		inline bool kmboxb = false;
		inline bool kmboxnet = false;
		inline bool confirm = false;
	}
}
