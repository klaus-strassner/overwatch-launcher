#include <windows.h>
#include <iostream>
#include "..\Utils.h"

using std::cout;
using std::cerr;
using std::endl;


bool SetCompatibilityFlags(LPCTSTR filename) {
    HKEY hKey;
    LSTATUS status;
    LPCTSTR registryFlag = TEXT("~ RUNASADMIN DISABLEDXMAXIMIZEDWINDOWEDMODE HIGHDPIAWARE");
    LPCTSTR subKey = TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");

    cout << "[+] Attempting to set Compatibility Flags" << endl;

    // open registry key
    status = RegCreateKeyEx(
        HKEY_CURRENT_USER,
        subKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL
    );

    if(status != ERROR_SUCCESS) {
        cerr << "[!] Failed to open registry key. Error Code: " << status << endl;
        return false;
    }

    // set registry value
    status = RegSetValueEx(
        hKey,
        Utils::GetAbsolutePath(filename),
        0,
        REG_SZ,
        (const BYTE*)registryFlag,
        (DWORD)((_tcslen(registryFlag) + 1) * sizeof(TCHAR))
    );

    // close registry key
    RegCloseKey(hKey);

    if(status != ERROR_SUCCESS) {
        cerr << "[!] Failed to set registry key. Error Code: " << status << endl;
        return false;
    }

    cout << "[+] Compatibility Flags set" << endl;
    return true;
}