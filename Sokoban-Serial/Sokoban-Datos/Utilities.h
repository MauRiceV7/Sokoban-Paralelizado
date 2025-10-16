// Utilities.h
#pragma once
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <conio.h>   // for _getch() on Windows
    #include <windows.h> // for Sleep() on Windows
#else
    #include <termios.h> // for terminal control on Unix
    #include <unistd.h>  // for read() and usleep() on Unix
#endif

class Utilities {
public:
    // Clears the console screen
    static void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    // Waits for a single key press (doesn't require Enter)
    static char getch() {
        #ifdef _WIN32
            return _getch();
        #else
            struct termios oldt, newt;
            char ch;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            ch = getchar();
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return ch;
        #endif
    }

    // Sleep for N milliseconds
    static void sleepMs(int ms) {
        #ifdef _WIN32
            Sleep(ms);
        #else
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        #endif
    }

    // Pause the console
    static void pause() {
        #ifdef _WIN32
            system("pause");
        #else
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
        #endif
    }
};
