#pragma once
#include <windows.h>
#include <tchar.h>
#include <string>

class Utils {
public:
    static LPCTSTR GetAbsolutePath(LPCTSTR filename) {
        static TCHAR fullPath[MAX_PATH];
        TCHAR dir[MAX_PATH];

        // 1. Get the path of the launcher
        GetModuleFileName(NULL, dir, MAX_PATH);

        // 2. Remove the filename from the path
        TCHAR* lastBackslash = _tcsrchr(dir, _T('\\'));
        if (lastBackslash) *(lastBackslash + 1) = _T('\0');

        // 3. Combine with the target filename
        _stprintf_s(fullPath, MAX_PATH, _T("%s%s"), dir, filename);

        return fullPath;
    }
};