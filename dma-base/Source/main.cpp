#include <Pch.h>
#include <game.hpp>

int main()
{
    SetConsoleTitleA("jouh DMA");
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
    directx_init();
    render_loop();
    exit(0);
}
