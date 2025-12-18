#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <windows.h>

class Launcher {
public:
    Launcher();
    ~Launcher();

    bool StartGameSuspended(LPCTSTR path);
    void ResumeGame();
    void WaitForExit();

    PROCESS_INFORMATION GetProcessInformation() const;

private:
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    bool isProcessOpen;
};

#endif