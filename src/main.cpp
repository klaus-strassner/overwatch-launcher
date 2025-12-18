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
const DWORD_PTR processAffinity {0b111111111100};
const DWORD_PTR mainThreadAffinity {0b000000000100};
const DWORD_PTR backgroundThreadAffinity {0b111111111000};

int main() {
    Launcher owLauncher;

    SetCompatibilityFlags(path);

    owLauncher.StartGameSuspended(path);
    SetProcessAffinity(owLauncher.GetProcessInformation().hProcess, processAffinity);
    SetMainThreadAffinity(owLauncher.GetProcessInformation().hThread, mainThreadAffinity);

    owLauncher.ResumeGame();
    Sleep(5000);
    DisableCrashReporting();

    while(true) {
        SetBackgroundThreadAffinity(owLauncher.GetProcessInformation(), backgroundThreadAffinity);
        Sleep(10000);
    }

    owLauncher.WaitForExit();
}