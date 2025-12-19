#include "Launcher.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;


Launcher::Launcher() : si{ sizeof(si) }, pi{} {}

Launcher::~Launcher() {
    if (pi.hProcess) CloseHandle(pi.hProcess);
    if (pi.hThread) CloseHandle(pi.hThread);
}

bool Launcher::StartGameSuspended(LPCTSTR path) {
    TCHAR cmdArgs[] = TEXT("");
    // --tank_WorkerThreadCount 0 --tank_MinWorkerThreadCount 0 --tank_MaxWorkerThreadCount 0
    cout << "[+] Attempting to launch " << path << endl;

    // Launch Game suspended
    BOOL success = CreateProcess(
        path,                   // Application Name
        cmdArgs,                // Command Line
        NULL,                   // Process Attributes
        NULL,                   // Thread Attributes
        FALSE,                  // Inherit Handles
        CREATE_SUSPENDED,       // Creation Flags
        NULL,                   // Environment
        NULL,                   // Current Directory (NULL = same as launcher)
        &si,                    // Startup Info
        &pi                     // Process Info
    );

    if(!success) {
        cerr << "[!] Failed to create process. Error Code: " << GetLastError() << endl;
        return false;
    }

    isProcessOpen = true;
    cout << "[+] Process created. PID: " << pi.dwProcessId << endl;

    return true;
}

void Launcher::ResumeGame() {
    if (pi.hThread) ResumeThread(pi.hThread);
}

void Launcher::WaitForExit() {
    if (pi.hProcess) WaitForSingleObject(pi.hProcess, INFINITE);
}

PROCESS_INFORMATION Launcher::GetProcessInformation() const {
    return pi;
}
