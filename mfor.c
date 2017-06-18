// MoveFileOnReboot (mfor)
// This file is public domain software (PDS).
// Copyright (C) 2017 Katayama Hirofumi MZ.
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
#endif
INT WINAPI
_tWinMain(HINSTANCE   hInstance,
          HINSTANCE   hPrevInstance,
          LPTSTR      lpCmdLine,
          INT         nCmdShow)
{
    OPENFILENAME ofn1, ofn2;
    TCHAR szFile1[MAX_PATH] = TEXT("");
    TCHAR szFile2[MAX_PATH] = TEXT("");
    TCHAR szBuf[256];
    LPTSTR pch1, pch2;
    DWORD flags;
    BOOL Success;
    int argc = __argc;
#if defined(UNICODE) && !defined(__wargv)
    LPWSTR *__wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
#endif
#ifndef _wargv
    #define _wargv __wargv
#endif

    if (argc == 1)
    {
        ZeroMemory(&ofn1, sizeof(ofn1));
        ZeroMemory(&ofn2, sizeof(ofn2));

        ofn1.lStructSize = OPENFILENAME_SIZE_VERSION_400;
        ofn1.lpstrFilter = TEXT("All Files (*.*)\0*.*\0");
        ofn1.lpstrFile = szFile1;
        ofn1.nMaxFile = MAX_PATH;
        ofn1.lpstrTitle = TEXT("MoveFileOnReboot (mfor): Choose the source file");
        if (!GetOpenFileName(&ofn1))
        {
            MessageBox(NULL, TEXT("The operation was cancelled."),
                       TEXT("Cancelled"), MB_ICONINFORMATION);
            return 0;
        }

        ofn2.lStructSize = OPENFILENAME_SIZE_VERSION_400;
        ofn2.lpstrFilter = TEXT("All Files (*.*)\0*.*\0");
        ofn2.lpstrFile = szFile2;
        ofn2.nMaxFile = MAX_PATH;
        ofn2.lpstrTitle = TEXT("MoveFileOnReboot (mfor): Choose the destination file");
        if (!GetOpenFileName(&ofn2))
        {
            MessageBox(NULL, TEXT("The operation was cancelled."),
                       TEXT("Cancelled"), MB_ICONINFORMATION);
            return 0;
        }

        pch1 = szFile1;
        pch2 = szFile2;
    }
    else if (argc == 3)
    {
        pch1 = __targv[1];
        if (lstrcmpi(__targv[2], TEXT("NULL")) == 0)
            pch2 = NULL;
        else
            pch2 = __targv[2];
    }
    else
    {
        MessageBox(NULL,
                   TEXT("MoveFileOnReboot (mfor)\r\n")
                   TEXT("Usage: mfor source_file destination_file"), NULL,
                   MB_ICONERROR);
        return 1;   /* failure */
    }

    flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING;
    if (MoveFileEx(pch1, pch2, flags))
    {
        MessageBox(NULL, TEXT("The file was moved."),
                   TEXT("Success"), MB_ICONINFORMATION);
        return 0;   /* success */
    }

    flags = MOVEFILE_DELAY_UNTIL_REBOOT;
    if (pch2)
        Success = MoveFileEx(pch2, NULL, flags) && MoveFileEx(pch1, pch2, flags);
    else
        Success = MoveFileEx(pch1, NULL, flags);

    if (Success)
    {
        MessageBox(NULL, TEXT("The file will be moved on reboot."),
                   TEXT("Success"), MB_ICONINFORMATION);
    }
    else
    {
        wsprintf(szBuf, TEXT("The operation was failed. Error Code: %ld"), GetLastError());
        MessageBox(NULL, szBuf, TEXT("Failure"), MB_ICONERROR);
        return 2;   /* failure */
    }

    return 0;   /* success */
}
