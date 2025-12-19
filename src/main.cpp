#include "Launcher.h"
#include "tweaks\SetCompatibilityFlags.h"
#include "tweaks\DisableCrashReporting.h"
#include "tweaks\SetProcessAffinity.h"
#include "tweaks\SetThreadAffinity.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

const LPCTSTR path { TEXT("Overwatch_Original.exe") };
const DWORD_PTR processAffinity {0b11111100};
const DWORD_PTR mainThreadAffinity {0b00000100};
const DWORD_PTR heavyThreadAffinity {0b00001000};
const DWORD_PTR backgroundThreadAffinity {0b11110000};

int main() {
    Launcher owLauncher;
    std::map<DWORD, ULONGLONG> previousSnapshot;

    SetCompatibilityFlags(path);

    owLauncher.StartGameSuspended(path);
    SetProcessAffinity(owLauncher.GetProcessInformation().hProcess, processAffinity);
    SetMainThreadAffinity(owLauncher.GetProcessInformation().hThread, mainThreadAffinity);

    owLauncher.ResumeGame();
    Sleep(5000);
    DisableCrashReporting();

    while(true) {
        DWORD heaviestThreadId = PinHeaviestsBackgroundThread(owLauncher.GetProcessInformation(),0b00001000, previousSnapshot);

        SetBackgroundThreadAffinity(owLauncher.GetProcessInformation(), backgroundThreadAffinity, heaviestThreadId);
        Sleep(10000);
    }

    owLauncher.WaitForExit();
}