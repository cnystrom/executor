/*
 *  Executor 2.1
 *
 *  A program launcher for Windows 10 and 11.
 *
 *  Original Executor (1991) by Chris Nystrom and J.R. McConnell.
 *  Executor 2.0 (2026). Executor 2.1 adds a Clear list button.
 *
 *  Public domain. See LICENSE.
 */

#define WIN32_LEAN_AND_MEAN
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <string.h>
#include <wchar.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>

#include "executor.h"

#define MAX_RECENT      10
#define PATH_BUF        1024
#define ARGS_BUF        1024
#define RECENT_NAME     L"recent.txt"
#define APP_FOLDER      L"Executor"
#define APP_TITLE       L"Executor"

struct recent_list {
    int count;
    wchar_t item[MAX_RECENT][PATH_BUF];
};

static void
trim(wchar_t *s)
{
    wchar_t *start, *end;

    if (s == NULL || s[0] == L'\0')
        return;

    start = s;
    while (*start == L' ' || *start == L'\t' || *start == L'\r' || *start == L'\n')
        start++;

    if (start != s)
        memmove(s, start, (wcslen(start) + 1) * sizeof(wchar_t));

    end = s + wcslen(s);
    while (end > s && (end[-1] == L' ' || end[-1] == L'\t' ||
                       end[-1] == L'\r' || end[-1] == L'\n'))
        end--;
    *end = L'\0';
}

static BOOL
is_url(const wchar_t *s)
{
    return PathIsURLW(s);
}

static BOOL
dir_from_path(const wchar_t *file, wchar_t *dir, size_t dir_cch)
{
    const wchar_t *slash, *slash2;
    size_t n;

    if (file == NULL || dir == NULL || dir_cch < 4)
        return FALSE;

    slash = wcsrchr(file, L'\\');
    slash2 = wcsrchr(file, L'/');
    if (slash2 != NULL && (slash == NULL || slash2 > slash))
        slash = slash2;

    if (slash == NULL) {
        /* "C:foo" — treat the drive as the folder. */
        if (file[0] != L'\0' && file[1] == L':' && file[2] != L'\0') {
            dir[0] = file[0];
            dir[1] = L':';
            dir[2] = L'\\';
            dir[3] = L'\0';
            return TRUE;
        }
        return FALSE;
    }

    n = (size_t)(slash - file);
    if (n == 0) {
        if (dir_cch < 2)
            return FALSE;
        dir[0] = slash[0];
        dir[1] = L'\0';
        return TRUE;
    }
    if (n >= dir_cch)
        return FALSE;

    memcpy(dir, file, n * sizeof(wchar_t));
    dir[n] = L'\0';
    if (n == 2 && dir[1] == L':' && n + 1 < dir_cch) {
        dir[2] = L'\\';
        dir[3] = L'\0';
    }
    return TRUE;
}

static BOOL
recent_path(wchar_t *path, size_t cch)
{
    wchar_t appdata[MAX_PATH];
    DWORD n;

    n = GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
    if (n == 0 || n >= MAX_PATH)
        return FALSE;
    if (FAILED(StringCchPrintfW(path, cch, L"%s\\%s", appdata, APP_FOLDER)))
        return FALSE;
    return TRUE;
}

static BOOL
recent_file_path(wchar_t *path, size_t cch)
{
    wchar_t dir[PATH_BUF];

    if (!recent_path(dir, PATH_BUF))
        return FALSE;
    if (FAILED(StringCchPrintfW(path, cch, L"%s\\%s", dir, RECENT_NAME)))
        return FALSE;
    return TRUE;
}

static void
load_recent(struct recent_list *list)
{
    wchar_t path[PATH_BUF];
    HANDLE hf;
    DWORD size, read;
    char *utf8, *line, *next;
    wchar_t wide[PATH_BUF];
    int n;

    list->count = 0;
    if (!recent_file_path(path, PATH_BUF))
        return;

    hf = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE)
        return;

    size = GetFileSize(hf, NULL);
    if (size == INVALID_FILE_SIZE || size == 0 || size > 64 * 1024) {
        CloseHandle(hf);
        return;
    }

    utf8 = (char *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size + 1);
    if (utf8 == NULL) {
        CloseHandle(hf);
        return;
    }
    if (!ReadFile(hf, utf8, size, &read, NULL)) {
        HeapFree(GetProcessHeap(), 0, utf8);
        CloseHandle(hf);
        return;
    }
    CloseHandle(hf);
    utf8[read] = '\0';

    line = utf8;
    /* Skip a UTF-8 BOM if present. */
    if ((unsigned char)line[0] == 0xEF &&
        (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF)
        line += 3;

    for (; line != NULL && list->count < MAX_RECENT; line = next) {
        next = strchr(line, '\n');
        if (next != NULL) {
            *next = '\0';
            next++;
        }
        if (line[0] != '\0' && line[strlen(line) - 1] == '\r')
            line[strlen(line) - 1] = '\0';
        if (line[0] == '\0')
            continue;
        n = MultiByteToWideChar(CP_UTF8, 0, line, -1, wide, PATH_BUF);
        if (n <= 1)
            continue;
        trim(wide);
        if (wide[0] == L'\0')
            continue;
        StringCchCopyW(list->item[list->count], PATH_BUF, wide);
        list->count++;
    }

    HeapFree(GetProcessHeap(), 0, utf8);
}

static void
save_recent(const struct recent_list *list)
{
    wchar_t dir[PATH_BUF], path[PATH_BUF];
    HANDLE hf;
    int i;
    char utf8[PATH_BUF * 3 + 4];
    DWORD written;
    int n;

    if (!recent_path(dir, PATH_BUF))
        return;
    CreateDirectoryW(dir, NULL);
    if (!recent_file_path(path, PATH_BUF))
        return;

    hf = CreateFileW(path, GENERIC_WRITE, 0, NULL,
                     CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE)
        return;

    /* UTF-8 BOM so a text editor will open the list correctly. */
    WriteFile(hf, "\xEF\xBB\xBF", 3, &written, NULL);

    for (i = 0; i < list->count; i++) {
        n = WideCharToMultiByte(CP_UTF8, 0, list->item[i], -1,
                                utf8, (int)sizeof(utf8) - 2, NULL, NULL);
        if (n <= 1)
            continue;
        /* n includes the terminator; replace it with CRLF. */
        utf8[n - 1] = '\r';
        utf8[n] = '\n';
        WriteFile(hf, utf8, (DWORD)(n + 1), &written, NULL);
    }
    CloseHandle(hf);
}

static void
add_recent(struct recent_list *list, const wchar_t *path)
{
    int i, found = -1;
    wchar_t tmp[PATH_BUF];

    if (path == NULL || path[0] == L'\0')
        return;
    if (FAILED(StringCchCopyW(tmp, PATH_BUF, path)))
        return;
    trim(tmp);
    if (tmp[0] == L'\0')
        return;

    for (i = 0; i < list->count; i++) {
        if (_wcsicmp(list->item[i], tmp) == 0) {
            found = i;
            break;
        }
    }
    if (found == 0)
        return;
    if (found > 0) {
        for (i = found; i > 0; i--)
            StringCchCopyW(list->item[i], PATH_BUF, list->item[i - 1]);
    } else {
        if (list->count < MAX_RECENT)
            list->count++;
        for (i = list->count - 1; i > 0; i--)
            StringCchCopyW(list->item[i], PATH_BUF, list->item[i - 1]);
    }
    StringCchCopyW(list->item[0], PATH_BUF, tmp);
    save_recent(list);
}

static void
fill_combo(HWND combo, const struct recent_list *list)
{
    int i;

    SendMessageW(combo, CB_RESETCONTENT, 0, 0);
    for (i = 0; i < list->count; i++)
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)list->item[i]);
}

static void
clear_recent(HWND hdlg, struct recent_list *list)
{
    if (list->count == 0)
        return;

    if (MessageBoxW(hdlg,
                    L"Clear the list of recent programs?",
                    APP_TITLE, MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    list->count = 0;
    save_recent(list);
    fill_combo(GetDlgItem(hdlg, IDC_FILE), list);
}

static void
show_win_error(HWND hwnd, const wchar_t *what, DWORD err)
{
    wchar_t *sys = NULL;
    wchar_t buf[2048];
    DWORD n;

    n = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, err, 0, (LPWSTR)&sys, 0, NULL);
    if (n > 0 && sys != NULL) {
        while (n > 0 && (sys[n - 1] == L'\r' || sys[n - 1] == L'\n'))
            sys[--n] = L'\0';
    }

    if (what != NULL && what[0] != L'\0') {
        StringCchPrintfW(buf, ARRAYSIZE(buf),
                         L"Could not run \"%s\".\n\n%s",
                         what,
                         (sys != NULL && sys[0] != L'\0')
                             ? sys
                             : L"Unknown error.");
    } else {
        StringCchPrintfW(buf, ARRAYSIZE(buf), L"%s",
                         (sys != NULL && sys[0] != L'\0')
                             ? sys
                             : L"Unknown error.");
    }

    if (sys == NULL || sys[0] == L'\0') {
        /* FormatMessage produced nothing useful; include the error number. */
        if (what != NULL && what[0] != L'\0')
            StringCchPrintfW(buf, ARRAYSIZE(buf),
                             L"Could not run \"%s\".\n\nWindows error %lu.",
                             what, (unsigned long)err);
        else
            StringCchPrintfW(buf, ARRAYSIZE(buf),
                             L"Windows error %lu.", (unsigned long)err);
    }

    MessageBoxW(hwnd, buf, APP_TITLE, MB_OK | MB_ICONERROR);
    if (sys != NULL)
        LocalFree(sys);
}

static void
do_browse(HWND hdlg)
{
    wchar_t file[PATH_BUF];
    OPENFILENAMEW ofn;
    const wchar_t *filter =
        L"Programs (*.exe;*.com;*.bat;*.cmd)\0*.exe;*.com;*.bat;*.cmd\0"
        L"All files (*.*)\0*.*\0";

    ZeroMemory(&ofn, sizeof(ofn));
    file[0] = L'\0';
    GetDlgItemTextW(hdlg, IDC_FILE, file, PATH_BUF);
    trim(file);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hdlg;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = file;
    ofn.nMaxFile = PATH_BUF;
    ofn.lpstrTitle = L"Select a program to run";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER |
                OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    if (!GetOpenFileNameW(&ofn))
        return;

    SetDlgItemTextW(hdlg, IDC_FILE, file);
}

static BOOL
do_run(HWND hdlg, struct recent_list *recent)
{
    wchar_t file[PATH_BUF], args[ARGS_BUF], work[PATH_BUF], defdir[PATH_BUF];
    const wchar_t *dir = NULL;
    SHELLEXECUTEINFOW sei;
    BOOL admin;

    file[0] = args[0] = work[0] = L'\0';
    GetDlgItemTextW(hdlg, IDC_FILE, file, PATH_BUF);
    GetDlgItemTextW(hdlg, IDC_ARGS, args, ARGS_BUF);
    GetDlgItemTextW(hdlg, IDC_WORKDIR, work, PATH_BUF);
    trim(file);
    trim(args);
    trim(work);

    if (file[0] == L'\0') {
        MessageBoxW(hdlg,
                    L"Please enter a program, document, or URL.",
                    APP_TITLE, MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(hdlg, IDC_FILE));
        return FALSE;
    }

    if (work[0] != L'\0') {
        dir = work;
    } else if (!is_url(file) && dir_from_path(file, defdir, PATH_BUF)) {
        dir = defdir;
    }

    admin = (IsDlgButtonChecked(hdlg, IDC_ADMIN) == BST_CHECKED);

    ZeroMemory(&sei, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.hwnd = hdlg;
    sei.lpVerb = admin ? L"runas" : L"open";
    sei.lpFile = file;
    sei.lpParameters = (args[0] != L'\0') ? args : NULL;
    sei.lpDirectory = dir;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        DWORD err = GetLastError();
        if (err == 0)
            err = ERROR_FILE_NOT_FOUND;
        show_win_error(hdlg, file, err);
        return FALSE;
    }

    add_recent(recent, file);
    fill_combo(GetDlgItem(hdlg, IDC_FILE), recent);
    SetDlgItemTextW(hdlg, IDC_FILE, file);
    return TRUE;
}


static void
set_dialog_icons(HWND hdlg)
{
    HINSTANCE inst = GetWindowInstance(hdlg);
    HICON icon, icon_sm;

    icon = (HICON)LoadImageW(inst, MAKEINTRESOURCEW(IDI_EXECUTOR),
                             IMAGE_ICON,
                             GetSystemMetrics(SM_CXICON),
                             GetSystemMetrics(SM_CYICON),
                             LR_DEFAULTCOLOR);
    icon_sm = (HICON)LoadImageW(inst, MAKEINTRESOURCEW(IDI_EXECUTOR),
                                IMAGE_ICON,
                                GetSystemMetrics(SM_CXSMICON),
                                GetSystemMetrics(SM_CYSMICON),
                                LR_DEFAULTCOLOR);
    if (icon != NULL)
        SendMessageW(hdlg, WM_SETICON, ICON_BIG, (LPARAM)icon);
    if (icon_sm != NULL)
        SendMessageW(hdlg, WM_SETICON, ICON_SMALL, (LPARAM)icon_sm);
}

static INT_PTR CALLBACK
AboutProc(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
    (void)lparam;
    switch (msg) {
    case WM_INITDIALOG:
        set_dialog_icons(hdlg);
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wparam) == IDOK || LOWORD(wparam) == IDCANCEL) {
            EndDialog(hdlg, IDOK);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void
set_cue_banners(HWND hdlg)
{
    HWND combo, edit;
    COMBOBOXINFO info;

    edit = GetDlgItem(hdlg, IDC_FILE);
    ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);
    if (GetComboBoxInfo(edit, &info) && info.hwndItem != NULL)
        combo = info.hwndItem;
    else
        combo = NULL;
    if (combo != NULL)
        SendMessageW(combo, EM_SETCUEBANNER, TRUE,
                     (LPARAM)L"Program, document, or URL");

    SendMessageW(GetDlgItem(hdlg, IDC_ARGS), EM_SETCUEBANNER, TRUE,
                 (LPARAM)L"Optional");
    SendMessageW(GetDlgItem(hdlg, IDC_WORKDIR), EM_SETCUEBANNER, TRUE,
                 (LPARAM)L"Program folder");
}

static INT_PTR CALLBACK
MainProc(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
    struct recent_list *recent;

    switch (msg) {
    case WM_INITDIALOG:
        recent = (struct recent_list *)lparam;
        SetWindowLongPtrW(hdlg, DWLP_USER, (LONG_PTR)recent);
        set_dialog_icons(hdlg);

        fill_combo(GetDlgItem(hdlg, IDC_FILE), recent);
        set_cue_banners(hdlg);
        SendDlgItemMessageW(hdlg, IDC_FILE, CB_LIMITTEXT, PATH_BUF - 1, 0);
        SendDlgItemMessageW(hdlg, IDC_ARGS, EM_LIMITTEXT, ARGS_BUF - 1, 0);
        SendDlgItemMessageW(hdlg, IDC_WORKDIR, EM_LIMITTEXT, PATH_BUF - 1, 0);
        SetFocus(GetDlgItem(hdlg, IDC_FILE));
        return FALSE;

    case WM_COMMAND:
        recent = (struct recent_list *)GetWindowLongPtrW(hdlg, DWLP_USER);
        switch (LOWORD(wparam)) {
        case IDOK:
            do_run(hdlg, recent);
            return TRUE;
        case IDCANCEL:
            EndDialog(hdlg, IDCANCEL);
            return TRUE;
        case IDC_BROWSE:
            do_browse(hdlg);
            return TRUE;
        case IDC_ABOUT:
            DialogBoxW(GetWindowInstance(hdlg),
                       MAKEINTRESOURCEW(IDD_ABOUT), hdlg, AboutProc);
            return TRUE;
        case IDC_CLEAR:
            clear_recent(hdlg, recent);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hdlg, IDCANCEL);
        return TRUE;
    }
    return FALSE;
}

int WINAPI
wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show)
{
    INITCOMMONCONTROLSEX icc;
    struct recent_list recent;

    (void)prev;
    (void)cmd;
    (void)show;

    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    load_recent(&recent);
    DialogBoxParamW(inst, MAKEINTRESOURCEW(IDD_MAIN), NULL, MainProc,
                    (LPARAM)&recent);
    return 0;
}
