#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <tchar.h>

using std::cout;
using std::cerr;
using std::endl;

bool DisableCrashReporting() {
    bool found {false};
    LPCTSTR processName = TEXT("CrashMailer_64.exe");
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe = { sizeof(pe) };

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_tcsicmp(pe.szExeFile, processName) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess) {
                    if(TerminateProcess(hProcess, 0)) {
                        cout << "[+] Killed " << processName << endl;
                        found = true;
                    }
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe));

        CloseHandle(hSnapshot);
    }

    if (!found) {
        cout << "[!] " << processName << " was not running" << endl;
    }

    return found;
}