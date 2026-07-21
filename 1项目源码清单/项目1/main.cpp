#include <Windows.h>

#include <iostream>
#include <locale>

#include "ConsoleUi7022.h"

void ConfigureConsole7022()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());
}

int wmain()
{
    ConfigureConsole7022();

    ConsoleUi7022 consoleUi7022;
    consoleUi7022.Run7022();
    return 0;
}
