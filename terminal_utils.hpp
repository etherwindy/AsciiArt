#pragma once
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#endif


namespace terminal_utils {

// 读取终端模拟器的长宽
void get_size(int &width, int &height) {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    #elif __linux__
        struct winsize size;
        ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
        width = size.ws_col;
        height = size.ws_row;
    #endif
}

// 清空终端
void clear() {
    #ifdef _WIN32
        system("cls");
    #elif __linux__
        system("clear");
    #endif
}
}
