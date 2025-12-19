#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

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

bool SetBackgroundThreadAffinity(PROCESS_INFORMATION pi, DWORD_PTR affinityMask, DWORD ignoreThreadId) {
    std::string output {"[+] Set Background Thread Affinity Mask for:"};

    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) return false;

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pi.dwProcessId && te32.th32ThreadID != pi.dwThreadId && te32.th32ThreadID != ignoreThreadId) {

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

ULONGLONG GetThreadCpuTime(DWORD tid) {
    HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!hThread) return 0;

    FILETIME ftCreate, ftExit, ftKernel, ftUser;
    ULONGLONG total = 0;
    if (GetThreadTimes(hThread, &ftCreate, &ftExit, &ftKernel, &ftUser)) {
        ULARGE_INTEGER k, u;
        k.LowPart = ftKernel.dwLowDateTime;
        k.HighPart = ftKernel.dwHighDateTime;
        u.LowPart = ftUser.dwLowDateTime;
        u.HighPart = ftUser.dwHighDateTime;
        total = k.QuadPart + u.QuadPart;
    }
    CloseHandle(hThread);
    return total;
}

DWORD PinHeaviestsBackgroundThread(PROCESS_INFORMATION pi, DWORD_PTR affinityMask, std::map<DWORD, ULONGLONG>& history) {
    std::map<DWORD, ULONGLONG> currentSnapshot;
    DWORD heaviestTid = 0;
    ULONGLONG maxDelta = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return 0;

    THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
    if (Thread32First(hSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pi.dwProcessId) {
                ULONGLONG currentTime = GetThreadCpuTime(te32.th32ThreadID);
                // DEBUG: Verify we are actually getting time from the OS
                // if (currentTime > 0) cout << "Thread " << te32.th32ThreadID << " Time: " << currentTime << endl;

                currentSnapshot[te32.th32ThreadID] = currentTime;

                if (te32.th32ThreadID != pi.dwThreadId && history.count(te32.th32ThreadID)) {
                    ULONGLONG prevTime = history[te32.th32ThreadID];
                    if (currentTime > prevTime) {
                        ULONGLONG delta = currentTime - prevTime;
                        if (delta > maxDelta) {
                            maxDelta = delta;
                            heaviestTid = te32.th32ThreadID;
                        }
                    }
                }
            }
        } while (Thread32Next(hSnap, &te32));
    }
    CloseHandle(hSnap);

    // If history was empty, we won't have a heaviestTid yet
    if (history.empty()) {
        cout << "[i] History initialized with " << currentSnapshot.size() << " threads." << endl;
    }

    history = std::move(currentSnapshot);

    if (heaviestTid != 0) {
        HANDLE hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, heaviestTid);
        if (hThread) {
            SetThreadAffinityMask(hThread, affinityMask);
            std::cout << "[!] Heavy Thread: " << heaviestTid << " Delta: " << maxDelta << std::endl;
            CloseHandle(hThread);
        }
    }

    return heaviestTid;
}
