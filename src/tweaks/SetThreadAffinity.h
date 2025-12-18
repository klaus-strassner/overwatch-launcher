#include <windows.h>
#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

bool SetMainThreadAffinity(HANDLE hThread, DWORD_PTR affinityMask) {
    cout << "[+] Attempting to set Main Thread Affinity Mask" << endl;

    if (SetThreadAffinityMask(hThread, affinityMask)) {
        cout << "[+] Set Main Thread Affinity Mask " << affinityMask << endl;
    } else {
        cerr << "[!] Failed to set affinity. Error: " << GetLastError() << endl;
        return false;
    }

    return true;
}

bool SetBackgroundThreadAffinity(PROCESS_INFORMATION pi, DWORD_PTR affinityMask) {
    std::string output {"[+] Set Background Thread Affinity Mask for:"};

    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) return false;

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pi.dwProcessId && te32.th32ThreadID != pi.dwThreadId) {

                HANDLE hThread = OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                if (hThread) {
                    DWORD_PTR dwOldAffinity = SetThreadAffinityMask(hThread, affinityMask);

                    if (dwOldAffinity == 0) {
                        std::cerr << "Error setting affinity for thread " << te32.th32ThreadID << " Error: " << GetLastError() << "\n";
                    } else if (dwOldAffinity != affinityMask) {
                        output += " " + std::to_string(te32.th32ThreadID);
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hThreadSnap, &te32));
    }

    if(output != "[+] Set Background Thread Affinity Mask for:") cout << output << endl;

    CloseHandle(hThreadSnap);
    return true;
}