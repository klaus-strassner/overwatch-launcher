#include <windows.h>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

bool SetProcessAffinity(HANDLE hProcess, DWORD_PTR affinityMask) {
    cout << "[+] Attempting to set Process Affinity Mask" << endl;

    if (SetProcessAffinityMask(hProcess, affinityMask)) {
        cout << "[+] Set Process Affinity Mask " << affinityMask << endl;
    } else {
        cerr << "[!] Failed to set affinity. Error: " << GetLastError() << endl;
        return false;
    }

    return true;
}