#include <Pch.h>
#include <game.hpp>

int main()
{
    SetConsoleTitleA("Jouh DMA");
    if (!mem.Init("FortniteClient-Win64-Shipping.exe", true, true))
    {
        std::cout << "Failed to initialize DMA" << std::endl;
        return 1;
    }
    system("CLS");
    if (!init())
    {
        printf("The gui was not initialized");
        Sleep(3000);
        exit(0);
    }
    system("CLS");
    create_overlay();
    std::thread logic_thread(actorloop);
    HANDLE logic_thread_handle = logic_thread.native_handle();
    SetThreadPriority(logic_thread_handle, THREAD_PRIORITY_HIGHEST);
    logic_thread.detach();
    directx_init();
    WPARAM result = render_loop();
    exit(0);
}