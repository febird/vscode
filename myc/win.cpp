#include <stdio.h>
#include <tchar.h>
#include <sys/types.h>
#include <windowsx.h>
#include <windows.h>
#include <commdlg.h>

#define CTRL_ID_EDIT_TEXT (0)
#define CTRL_ID_EDIT_INPUT (1)
#define CTRL_ID_BTN_INPUT (2)
#define CTRL_ID_BTN_CANCEL (3)

#define MENU_ID_NEW (10)
#define MENU_ID_OPEN (11)
#define MENU_ID_SAVE (12)
#define MENU_ID_SAVEAS (13)
#define MENU_ID_EXIT (14)

#define MENU_ID_UNDO (20)
#define MENU_ID_COPY (21)
#define MENU_ID_PASTE (22)
#define MENU_ID_CUT (23)
#define MENU_ID_DEL (24)
#define MENU_ID_SELALL (25)
#define MENU_ID_GOTO (26)

#define MENU_ID_WRET (30)

#define MENU_ID_ABOUT (40)

#define MAX_STRING (256)
#define MAX_EDIT (65535U)

TCHAR *szClassName = TEXT("MySimpleNotepad");
TCHAR *szCaptionMain = TEXT("Simple Notepad Ver0.01");

HANDLE hInstance;
HWND hWinMain;
HWND hInputDlg;

BOOL bUndo;
BOOL bWantReturn;

TCHAR cCrtFile[MAX_PATH];
TCHAR cInputEdit[MAX_STRING];

INT _WinMain(VOID);

LRESULT CALLBACK _ProcWinMain(HWND hWnd, DWORD uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK _ProcInputDlg(HWND hWnd, DWORD uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateEdit(DWORD dwStyle);
HWND CreatInputDlg(TCHAR *pTitle);
HMENU CreateMainMenu(VOID);
HACCEL CreateAccelerator(VOID);

VOID OnPaint(VOID);
VOID OnAbout(VOID);
VOID OnQuit(VOID);
VOID OnNew(VOID);
VOID OnOpen(VOID);
VOID OnSave(INT nFlag);
VOID OnUndo(VOID);
VOID OnCopy(VOID);
VOID OnPaste(VOID);
VOID OnCut(VOID);
VOID OnDel(VOID);
VOID OnSelAll(VOID);
VOID OnWantReturn(VOID);
VOID OnGoto(INT nFlag);

VOID ModifyWinTitle(VOID);
VOID OpenGivenFile(TCHAR *pFile);

VOID ModifyWinTitle(VOID)
{
    TCHAR *cTitle;
    INT nSize = MAX_PATH + strlen(szCaptionMain) + 3;
    cTitle = malloc(nSize);
    ZeroMemory(cTitle, nSize);
    sprintf(cTitle, TEXT("%s%s - %s"),
            (strlen(cCrtFile) == 0) ? TEXT("Untitled") : cCrtFile,
            (Edit_GetModify(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT)) ? TEXT("*") : TEXT("")),
            szCaptionMain);
    SetWindowText(hWinMain, cTitle);
    free(cTitle);
}

VOID OnPaint(VOID)
{
    RECT stRect;

    GetClientRect(hWinMain, &stRect);
    MoveWindow(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, FALSE);
    UpdateWindow(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT));
    UpdateWindow(hWinMain);
}

VOID OnAbout(VOID)
{
    MessageBox(NULL, TEXT("Copyleft (C) 2010/n/nCompiler:MinGW-5.1.6[gcc version 3.4.5 (mingw-vista special r3) (with option -mwindows)]/n/nComment:The max file size is 65535 Bytes."), TEXT("About..."), MB_OK);
}

VOID OnQuit(VOID)
{
    if (Edit_GetModify(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT)))
    {
        OnSave(0);
    }
    else
    {
    }
    DestroyWindow(hWinMain);
    PostQuitMessage(0);
}

VOID OnNew(VOID)
{
    SetWindowText(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), "");
}

VOID OpenGivenFile(TCHAR *pFile)
{
    HANDLE hFile;
    DWORD dwSize, Num;
    TCHAR *cBuffer;
    hFile = CreateFile(cCrtFile, GENERIC_READ, 0, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE != hFile)
    {
        SetFilePointer(hFile, 0, 0, FILE_BEGIN);

        dwSize = GetFileSize(hFile, NULL);
        cBuffer = malloc(dwSize + 1); /*NOT sure if need to leave a byte for '/0' */
        ZeroMemory(cBuffer, dwSize + 1);

        ReadFile(hFile, cBuffer, dwSize, &Num, NULL);
        Edit_SetText(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), cBuffer);

        free(cBuffer);
        CloseHandle(hFile);
    }

    ModifyWinTitle();

    SetFocus(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT));
}

VOID OnOpen(VOID)
{
    TCHAR *cFilter = TEXT("Text(*.txt)/0*.TXT/0");
    OPENFILENAME ofFile;

    memset(&ofFile, 0, sizeof(OPENFILENAME));
    ofFile.lStructSize = sizeof(OPENFILENAME);
    ofFile.hwndOwner = hWinMain;
    ofFile.hInstance = hInstance;
    ofFile.lpstrFilter = cFilter;
    ofFile.lpstrTitle = TEXT("Open");
    ofFile.lpstrFile = cCrtFile;
    ofFile.nMaxFile = MAX_PATH;
    ofFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofFile))
    {
        OpenGivenFile(cCrtFile);
    }
}

VOID OnSave(INT nFlag)
{
    if ((nFlag) || (strlen(cCrtFile) == 0))
    {
        TCHAR *cFilter = TEXT("Text(*.txt)|*.TXT|");
        OPENFILENAME ofFile;

        memset(&ofFile, 0, sizeof(OPENFILENAME));
        ofFile.lStructSize = sizeof(OPENFILENAME);
        ofFile.hwndOwner = hWinMain;
        ofFile.hInstance = hInstance;
        ofFile.lpstrFilter = cFilter;
        ofFile.lpstrTitle = TEXT("Save As");
        ofFile.lpstrFile = cCrtFile;
        ofFile.nMaxFile = MAX_PATH;
        ofFile.lpstrDefExt = TEXT(".txt");
        ofFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        GetSaveFileName(&ofFile);
    }
    else
    {
    }

    if (strlen(cCrtFile) != 0)
    {
        HANDLE hFile;
        DWORD dwSize, Num;
        TCHAR *cBuffer;
        hFile = CreateFile(cCrtFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE != hFile)
        {

            SetFilePointer(hFile, 0, 0, FILE_BEGIN);

            dwSize = Edit_GetTextLength(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT)) + 1;
            cBuffer = malloc(dwSize);
            ZeroMemory(cBuffer, dwSize);

            Edit_GetText(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), cBuffer, dwSize);
            WriteFile(hFile, cBuffer, dwSize, &Num, NULL);

            free(cBuffer);
            CloseHandle(hFile);

            Edit_SetModify(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), FALSE);
        }
    }

    ModifyWinTitle();
}

VOID OnUndo(VOID)
{
    Edit_Undo(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT));
}

VOID OnCopy(VOID)
{
    TCHAR *pText;
    HANDLE hGlobal;
    PTSTR pGlobal;
    DWORD dwGetSel;
    DWORD dwEditLen;

    dwGetSel = Edit_GetSel(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT));
    dwEditLen = Edit_GetTextLength(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT));

    pText = malloc(dwEditLen + 1);
    Edit_GetText(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), pText, dwEditLen);

    hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (HIWORD(dwGetSel) - LOWORD(dwGetSel) + 1) * sizeof(TCHAR));
    pGlobal = GlobalLock(hGlobal);
    strncpy(pGlobal, &pText[LOWORD(dwGetSel)], HIWORD(dwGetSel) - LOWORD(dwGetSel));
    GlobalUnlock(hGlobal);
    OpenClipboard(hWinMain);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hGlobal);
    CloseClipboard();
}

VOID OnPaste(VOID)
{
    HANDLE hGlobal;
    PTSTR pGlobal;
    OpenClipboard(hWinMain);
    if (IsClipboardFormatAvailable(CF_TEXT))
    {
        hGlobal = GetClipboardData(CF_TEXT);
        pGlobal = GlobalLock(hGlobal);
        Edit_ReplaceSel(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), pGlobal);
        GlobalUnlock(hGlobal);
    }
    CloseClipboard();
}

VOID OnCut(VOID)
{
    OnCopy();
    OnDel();
}

VOID OnDel(VOID)
{
    Edit_ReplaceSel(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), TEXT(""));
}

VOID OnSelAll(VOID)
{
    Edit_SetSel(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT),
                0,
                Edit_GetTextLength(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT)));
}

VOID OnWantReturn(VOID)
{
    DWORD dwStyle;

    bWantReturn = !bWantReturn;

    dwStyle = GetWindowLong(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), GWL_STYLE);
    if (bWantReturn)
    {
        dwStyle &= (~(ES_AUTOHSCROLL | WS_HSCROLL));
    }
    else
    {
        dwStyle |= (ES_AUTOHSCROLL) | (WS_HSCROLL);
    }

    DestroyWindow(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT));
    CreateEdit(dwStyle);
    if (strlen(cCrtFile) != 0)
    {
        OpenGivenFile(cCrtFile);
    }
}

VOID OnGoto(INT nFlag)
{
    INT nLine;
    //TCHAR cBuff[MAX_STRING];

    if (0 == nFlag)
    {
        CreatInputDlg(TEXT("Goto:"));
        //nLine = Edit_LineIndex(GetDlgItem(hWinMain,CTRL_ID_EDIT_TEXT),-1);
        //itoa(nLine,cBuff,10);
        //Edit_SetText(GetDlgItem(hInputDlg,CTRL_ID_EDIT_INPUT),cBuff);
    }
    else
    {
        Edit_GetText(GetDlgItem(hInputDlg, CTRL_ID_EDIT_INPUT), cInputEdit, MAX_STRING);
        nLine = Edit_LineIndex(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), atoi(cInputEdit) - 1);
        Edit_SetSel(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT), nLine, nLine);
        SetFocus(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT));
    }
}

LRESULT CALLBACK _ProcWinMain(HWND hWnd, DWORD uMsg, WPARAM wParam, LPARAM lParam)
{
    if (WM_PAINT == uMsg)
    {
        OnPaint();
    }
    else if (WM_CLOSE == uMsg)
    {
        OnQuit();
    }
    else if (WM_INITMENUPOPUP == uMsg)
    {
        //if((UINT) LOWORD(lParam) == 1){
        BOOL bEnable;
        bEnable = (Edit_GetSel(GetDlgItem(hWinMain, CTRL_ID_EDIT_TEXT)) != 0) ? MF_ENABLED : MF_GRAYED;
        EnableMenuItem((HMENU)wParam, MENU_ID_UNDO, bUndo ? MF_ENABLED : MF_GRAYED);
        EnableMenuItem((HMENU)wParam, MENU_ID_PASTE, IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);
        EnableMenuItem((HMENU)wParam, MENU_ID_CUT, bEnable);
        EnableMenuItem((HMENU)wParam, MENU_ID_COPY, bEnable);
        EnableMenuItem((HMENU)wParam, MENU_ID_DEL, bEnable);
        //}

        CheckMenuItem((HMENU)wParam, MENU_ID_WRET, bWantReturn ? MF_CHECKED : MF_UNCHECKED);
    }
    else if (WM_DROPFILES == uMsg)
    {
        TCHAR lpszFileName[MAX_PATH];
        DragQueryFile((HANDLE)wParam, 0, lpszFileName, MAX_PATH);
        OpenGivenFile(lpszFileName);
        strcpy(cCrtFile, lpszFileName);
    }
    else if ((WM_COMMAND == uMsg) /*&&(HIWORD(wParam) == BN_CLICKED)*/)
    {
        WORD wNotifyCode = HIWORD(wParam); // notification code
        WORD wID = LOWORD(wParam);         // item, control, or accelerator identifier
        HWND hCtl = (HWND)lParam;          // handle of control
        /*if(HIWORD(wParam) == BN_CLICKED){*/
        switch (wID)
        {
        case MENU_ID_ABOUT:
            OnAbout();
            break;
        case MENU_ID_EXIT:
            OnQuit();
            break;
        case MENU_ID_NEW:
            OnNew();
            break;
        case MENU_ID_OPEN:
            OnOpen();
            break;
        case MENU_ID_SAVE:
            OnSave(0);
            break;
        case MENU_ID_SAVEAS:
            OnSave(1);
            break;
        case MENU_ID_UNDO:
            OnUndo();
            break;
        case MENU_ID_COPY:
            OnCopy();
            break;
        case MENU_ID_PASTE:
            OnPaste();
            break;
        case MENU_ID_CUT:
            OnCut();
            break;
        case MENU_ID_DEL:
            OnDel();
            break;
        case MENU_ID_SELALL:
            OnSelAll();
            break;
        case MENU_ID_WRET:
            OnWantReturn();
            break;
        case MENU_ID_GOTO:
            OnGoto(0);
            break;
        default:
            DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        }
        /*} 
        else */ if (EN_CHANGE == HIWORD(wParam))
        {
            ModifyWinTitle();
            bUndo = TRUE;
        }
        else
        {
            DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
    else
    {
        DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK _ProcInputDlg(HWND hWnd, DWORD uMsg, WPARAM wParam, LPARAM lParam)
{
    if (WM_PAINT == uMsg)
    {
        UpdateWindow(hWnd);
    }
    else if (WM_CLOSE == uMsg)
    {
        DestroyWindow(hWnd);
    }
    else if ((WM_COMMAND == uMsg) /*&&(HIWORD(wParam) == BN_CLICKED)*/)
    {
        WORD wNotifyCode = HIWORD(wParam); // notification code
        WORD wID = LOWORD(wParam);         // item, control, or accelerator identifier
        HWND hCtl = (HWND)lParam;          // handle of control
        switch (wID)
        {
        case CTRL_ID_BTN_INPUT:
            OnGoto(1);
            DestroyWindow(hWnd);
            break;
        case CTRL_ID_BTN_CANCEL:
            DestroyWindow(hWnd);
            break;
        default:
            DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        }
    }
    else
    {
        DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

HMENU CreateMainMenu(VOID)
{
    HMENU hMenu = CreateMenu();
    HMENU hFile = CreateMenu();
    HMENU hEdit = CreateMenu();
    HMENU hForm = CreateMenu();
    HMENU hHelp = CreateMenu();

    if (hMenu && hFile)
    {
        AppendMenu(hFile, MF_STRING, MENU_ID_NEW, TEXT("&New        Ctrl + N"));
        AppendMenu(hFile, MF_STRING, MENU_ID_OPEN, TEXT("&Open...    Ctrl + O"));
        AppendMenu(hFile, MF_STRING, MENU_ID_SAVE, TEXT("&Save       Ctrl + S"));
        AppendMenu(hFile, MF_STRING, MENU_ID_SAVEAS, TEXT("Save &As..."));
        AppendMenu(hFile, MF_SEPARATOR, 0, "");
        AppendMenu(hFile, MF_STRING, MENU_ID_EXIT, TEXT("E&xit"));

        AppendMenu(hEdit, MF_STRING, MENU_ID_UNDO, TEXT("&Undo"));
        AppendMenu(hEdit, MF_SEPARATOR, 0, "");
        AppendMenu(hEdit, MF_STRING, MENU_ID_COPY, TEXT("&Copy"));
        AppendMenu(hEdit, MF_STRING, MENU_ID_PASTE, TEXT("&Paste"));
        AppendMenu(hEdit, MF_STRING, MENU_ID_CUT, TEXT("Cu&t"));
        AppendMenu(hEdit, MF_STRING, MENU_ID_DEL, TEXT("De&l"));
        AppendMenu(hEdit, MF_SEPARATOR, 0, "");
        AppendMenu(hEdit, MF_STRING, MENU_ID_GOTO, TEXT("&Goto"));
        AppendMenu(hEdit, MF_SEPARATOR, 0, "");
        AppendMenu(hEdit, MF_STRING, MENU_ID_SELALL, TEXT("Select &All"));

        AppendMenu(hForm, MF_STRING, MENU_ID_WRET, TEXT("&Want Return"));

        AppendMenu(hHelp, MF_STRING, MENU_ID_ABOUT, TEXT("&About"));

        AppendMenu(hMenu, MF_POPUP, (UINT)hFile, TEXT("&File"));
        AppendMenu(hMenu, MF_POPUP, (UINT)hEdit, TEXT("&Edit"));
        AppendMenu(hMenu, MF_POPUP, (UINT)hForm, TEXT("F&ormat"));
        AppendMenu(hMenu, MF_POPUP, (UINT)hHelp, TEXT("&Help"));
    }

    return hMenu;
}

HACCEL CreateAccelerator(VOID)
{
    ACCEL accel[] =
        {
            {FCONTROL | FVIRTKEY, TEXT('S'), MENU_ID_SAVE},
            {FCONTROL | FVIRTKEY, TEXT('N'), MENU_ID_NEW},
            {FCONTROL | FVIRTKEY, TEXT('O'), MENU_ID_OPEN},
        };

    return CreateAcceleratorTable(accel, 3);
}

HWND CreateEdit(DWORD dwStyle)
{
    HWND hWnd;
    RECT rect;

    GetClientRect(hWinMain, &rect);
    hWnd = CreateWindow(TEXT("Edit"), TEXT(""), dwStyle, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hWinMain, (HMENU)CTRL_ID_EDIT_TEXT, hInstance, NULL);
    Edit_CanUndo(hWnd);
    SendMessage(hWnd, EM_SETLIMITTEXT, MAX_EDIT, 0);
}

HWND CreatInputDlg(TCHAR *pTitle)
{
    TCHAR *pClassName = TEXT("MyInputDlg");

    WNDCLASSEX stWndClass;
    MSG stMsg;
    HWND hNewWnd;
    RECT rect;

    RtlZeroMemory(&stWndClass, sizeof(stWndClass));
    stWndClass.hCursor = LoadCursor(0, IDC_ARROW);
    stWndClass.hInstance = GetModuleHandle(NULL);
    stWndClass.cbSize = sizeof(WNDCLASSEX);
    stWndClass.style = CS_HREDRAW | CS_VREDRAW;
    stWndClass.lpfnWndProc = (WNDPROC)_ProcInputDlg;
    stWndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    stWndClass.lpszClassName = pClassName;

    RegisterClassEx(&stWndClass);

    hInputDlg = CreateWindowEx(WS_EX_CLIENTEDGE,
                               pClassName, pTitle, WS_OVERLAPPEDWINDOW, 200, 200, 160, 120, NULL, NULL, hInstance, NULL);

    hNewWnd = CreateWindow(TEXT("Edit"), TEXT(""), WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER, 10, 15, 130, 24, hInputDlg, (HMENU)CTRL_ID_EDIT_INPUT, hInstance, NULL);
    SetFocus(hNewWnd);
    ShowWindow(hNewWnd, SW_SHOWNORMAL);

    hNewWnd = CreateWindow(TEXT("Button"), TEXT("&OK"), WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 10, 50, 60, 24, hInputDlg, (HMENU)CTRL_ID_BTN_INPUT, hInstance, NULL);
    ShowWindow(hNewWnd, SW_SHOWNORMAL);

    hNewWnd = CreateWindow(TEXT("Button"), TEXT("&Cancel"), WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 80, 50, 60, 24, hInputDlg, (HMENU)CTRL_ID_BTN_CANCEL, hInstance, NULL);
    ShowWindow(hNewWnd, SW_SHOWNORMAL);

    ShowWindow(hInputDlg, SW_SHOWNORMAL);
    UpdateWindow(hInputDlg);

    return hInputDlg;
}

INT _WinMain(VOID)
{
    MSG stMsg;
    HWND hNewWnd;
    HMENU hMenu;
    HACCEL hAccelerator;
    WNDCLASSEX stWndClass;

    hMenu = CreateMainMenu();
    hAccelerator = CreateAccelerator();
    hInstance = GetModuleHandle(NULL);

    memset(&stWndClass, 0, sizeof(stWndClass));
    stWndClass.hCursor = LoadCursor(0, IDC_ARROW);
    stWndClass.hInstance = hInstance;
    stWndClass.cbSize = sizeof(WNDCLASSEX);
    stWndClass.style = CS_HREDRAW | CS_VREDRAW;
    stWndClass.lpfnWndProc = (WNDPROC)_ProcWinMain;
    stWndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    stWndClass.lpszClassName = szClassName;

    RegisterClassEx(&stWndClass);

    hWinMain = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES, szClassName, szCaptionMain, WS_OVERLAPPEDWINDOW, 100, 100, 600, 400, NULL, hMenu, hInstance, hAccelerator);
    ModifyWinTitle();

    hNewWnd = CreateEdit(WS_CHILDWINDOW | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN |  ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL | WS_HSCROLL);

    ShowWindow(hWinMain, SW_SHOWNORMAL);
    UpdateWindow(hWinMain);

    while (TRUE)
    {

        if (GetMessage(&stMsg, NULL, 0, 0) == 0)
        {
            break;
        }

        if (!(TranslateAccelerator(hWinMain, hAccelerator, &stMsg)))
        {
            TranslateMessage(&stMsg);
            DispatchMessage(&stMsg);
        }
    }

    return 0;
}

INT _tmain(INT argc, TCHAR *argv[])
{
    bUndo = FALSE;
    bWantReturn = FALSE;
    memset(cCrtFile, 0, MAX_PATH);

    _WinMain();
    ExitProcess(0);

    return 0;
}