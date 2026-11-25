//
//      EXECUTOR.EXE
//
//      Version 1.0
//
//      25 June 1991
//
//      (c) 1991   by   Innovation Inc
//
//      Developed by Chris Nystrom, and J.R. McConnell
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "executor.h"

char
    szIFileName [96] = "",      // Input File Name
    szFileSpec [16] = "*.exe",  // Initial File Specification
    szAppName [] = "Executor";  // Application Name

HANDLE  hInstance;

int     PASCAL WinMain (HANDLE, HANDLE, LPSTR, int);
long    FAR PASCAL WndProc (HWND, WORD, WORD, LONG);
BOOL    FAR PASCAL FileDlgProc (HWND, WORD, WORD, LONG);
BOOL    FAR PASCAL AboutDlgProc (HWND, WORD, WORD, LONG);

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdParam, int nCmdShow)
    {
    HWND        hWnd;
    MSG         message;
    WNDCLASS    class;
    HMENU       hMenu;

    if (!hPrevInstance)
        {
        class.style = NULL;
        class.lpfnWndProc = WndProc;
        class.cbClsExtra = 0;
        class.cbWndExtra = 0;
        class.hInstance = hInstance;
        class.hIcon = LoadIcon (hInstance, szAppName);
        class.hCursor = LoadCursor (NULL, IDC_ARROW);
        class.hbrBackground = GetStockObject (WHITE_BRUSH);
        class.lpszMenuName = szAppName;
        class.lpszClassName = szAppName;
        RegisterClass (&class);
        }
    hWnd = CreateWindow (szAppName, "File Executor", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                NULL, NULL, hInstance, NULL);
    hMenu = GetSystemMenu (hWnd, FALSE);
    AppendMenu (hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu (hMenu, MF_STRING, IDM_ABOUT, "About ...");
    ShowWindow (hWnd, SW_SHOWMINNOACTIVE) ;
    UpdateWindow (hWnd);
    while (GetMessage (&message, NULL, 0, 0))
        {
        TranslateMessage(&message);
        DispatchMessage(&message);
        }
    return message.wParam;
    }

// Standard Windows Procedure
long FAR PASCAL WndProc (HWND hWnd, WORD message, WORD wParam, LONG lParam)
    {
    static FARPROC  lpfnFileDlgProc;
    static FARPROC  lpfnAboutDlgProc;

    switch (message)
        {
        // Create Application and Dialog Boxes
        case WM_CREATE:
            hInstance = ((LPCREATESTRUCT) lParam) -> hInstance;
            lpfnFileDlgProc = MakeProcInstance (FileDlgProc, hInstance);
            lpfnAboutDlgProc = MakeProcInstance (AboutDlgProc, hInstance);
            return 0;

        case WM_SYSCOMMAND:
            switch (wParam)
                {
                // About selected from System Menu
                case IDM_ABOUT:
                    DialogBox (hInstance, "AboutBox", hWnd,
                            lpfnAboutDlgProc);
                    return 0;
                }
            break;

        // Open File Selection Dialog Box and execute program
        case WM_QUERYOPEN:
            DialogBox (hInstance, "FILE", hWnd, lpfnFileDlgProc);
            if (strlen(szIFileName) > 0)
                WinExec(szIFileName, SW_SHOWNORMAL);
            return 0;

        // Quit Application
        case WM_DESTROY:
            PostQuitMessage (0);
            return 0;
        }
    return DefWindowProc (hWnd, message, wParam, lParam);
    }

// Procedure for File Selection Dialog Box
BOOL FAR PASCAL FileDlgProc  (HWND hDlg, WORD message, WORD wParam, LONG lParam)
    {
    char
        work1 [80],                 // Temporary workspace 1
        work2 [80],                 // Temporary workspace 2
        cLastChar = 0x00;           // chr 0 (last char of string)
    short       nEditLen = 0x00;    // Length of Edit Field
    static int  nExtFlag = DLG_EXE; // Current FileSpec Flag


    switch (message)
        {
        case WM_INITDIALOG:
            SendDlgItemMessage (hDlg, IDD_IFNAME, EM_LIMITTEXT, 80, 0L);
            DlgDirList (hDlg, szFileSpec, IDD_FLIST, NULL, FILES);
            DlgDirList (hDlg, szFileSpec, IDD_DLIST, IDD_FPATH, PATH);
            SetDlgItemText (hDlg, IDD_IFNAME, szFileSpec);
            InvalidateRect (hDlg, NULL, TRUE);
            CheckRadioButton(hDlg, DLG_ALL, DLG_EXE, nExtFlag);
            SendMessage (hDlg, WM_COMMAND, DLG_OK, 0L);
            return TRUE;

        case WM_COMMAND:
            switch (wParam)
                {
                case IDD_FLIST:
                    switch (HIWORD (lParam))
                        {
                        case LBN_SELCHANGE:
                            DlgDirSelect (hDlg, szIFileName, IDD_FLIST);
                            SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                            return TRUE;

                        case LBN_DBLCLK:
                            DlgDirSelect (hDlg, szIFileName, IDD_FLIST);
                            SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                            SendMessage (hDlg, WM_COMMAND, DLG_OK, 0L);
                            return TRUE;
                        }
                    break;

                case IDD_DLIST:
                    switch (HIWORD (lParam))
                        {
                        case LBN_SELCHANGE:
                            DlgDirSelect (hDlg, szIFileName, IDD_DLIST);
                            lstrcat (szIFileName, szFileSpec);
                            SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                            return TRUE;

                        case LBN_DBLCLK:
                            DlgDirSelect (hDlg, szIFileName, IDD_DLIST);
                            lstrcat (szIFileName, szFileSpec);
                            DlgDirList (hDlg, szFileSpec, IDD_FLIST, NULL, FILES);
                            DlgDirList (hDlg, szFileSpec, IDD_DLIST, IDD_FPATH, PATH);
                            SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                            SendMessage (hDlg, WM_COMMAND, DLG_OK, 0L);
                            return TRUE;
                        }
                    break;

                case DLG_EXE:
                    lstrcpy(szFileSpec,"*.exe");
                    lstrcpy (szIFileName, szFileSpec);
                    nExtFlag = DLG_EXE;
                    SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                    SendMessage (hDlg, WM_COMMAND, DLG_OK, 0L);
                    return TRUE;

                case DLG_COM:
                    lstrcpy(szFileSpec,"*.com");
                    lstrcpy (szIFileName, szFileSpec);
                    nExtFlag = DLG_COM;
                    SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                    SendMessage (hDlg, WM_COMMAND, DLG_OK, 0L);
                    return TRUE;

                case DLG_BAT:
                    lstrcpy(szFileSpec,"*.bat");
                    lstrcpy (szIFileName, szFileSpec);
                    nExtFlag = DLG_BAT;
                    SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                    SendMessage (hDlg, WM_COMMAND, DLG_OK, 0L);
                    return TRUE;

                case DLG_ALL:
                    lstrcpy(szFileSpec,"*.*");
                    lstrcpy (szIFileName, szFileSpec);
                    nExtFlag = DLG_ALL;
                    SetDlgItemText (hDlg, IDD_IFNAME, szIFileName);
                    SendMessage (hDlg, WM_COMMAND, DLG_OK, 0L);
                    return TRUE;

                case DLG_OK:
                    GetDlgItemText (hDlg, IDD_IFNAME, szIFileName, 80);
                    nEditLen = lstrlen (szIFileName);
                    cLastChar = *AnsiPrev (szIFileName, szIFileName + nEditLen);
                    if (cLastChar == '\\' || cLastChar == ':')
                        lstrcat (szIFileName, szFileSpec);
                    if (strchr (szIFileName, '*') || strchr (szIFileName, '?'))
                        {
                        DlgDirList (hDlg, szIFileName, IDD_FLIST, NULL, FILES);
                        if (DlgDirList (hDlg, szIFileName, IDD_DLIST, IDD_FPATH, PATH))
                            {
                            lstrcpy (szFileSpec, szIFileName);
                            SetDlgItemText (hDlg, IDD_IFNAME, szFileSpec);
                            }
                        else
                            MessageBeep (0);
                        return TRUE;
                        }
                    lstrcat (lstrcat (szIFileName,"\\"), szFileSpec);
                    DlgDirList (hDlg, szIFileName, IDD_FLIST, NULL, FILES);
                    if (DlgDirList (hDlg, szIFileName, IDD_DLIST, IDD_FPATH, PATH))
                        {
                        lstrcpy (szFileSpec, szIFileName);
                        SetDlgItemText (hDlg, IDD_IFNAME, szFileSpec);
                        return TRUE;
                        }
                    szIFileName [nEditLen] = '\0';
                    if (access (szIFileName, 0x04))
                        {
                        LoadString (hInstance, ERR1, work1, 128);
                        sprintf (work2, work1, szIFileName);
                        MessageBox (hDlg, work2, szAppName, MB_ICONINFORMATION | MB_OK);
                        return TRUE;
                        }
                    OemToAnsi (szIFileName, szIFileName);
                    EndDialog (hDlg, FALSE);
                    return TRUE;

                case DLG_CAN:
                    szIFileName [0] = 0x00;
                    EndDialog (hDlg, FALSE);
                    return TRUE;
                }
        }
    return FALSE;
    }

// Procedure for About Dialog Box
BOOL FAR PASCAL AboutDlgProc(HWND hDlg, WORD message, WORD wParam, LONG lParam)
    {
    switch (message)
        {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            switch (wParam)
                {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, TRUE);
                    return (TRUE);
                }
            break;
        }
    return (FALSE);
    }
