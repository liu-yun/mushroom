#include "mushroom.h"

int CALLBACK InputDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            OnInitInputDialog(hDlg, (HWND)wParam, lParam);
            return 1;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK) {
                GetDlgItemText(hDlg, IDC_EDIT1, temp_name, 10);
                for (int i = 0; i < 4; i++)
                    temp_num[i] = GetDlgItemInt(hDlg, IDC_EDIT2 + i, nullptr, 0);
                temp_num[4] = SendMessage(GetDlgItem(hDlg, IDC_TRACKBAR1), TBM_GETPOS, 0, 0);
                EndDialog(hDlg, LOWORD(wParam));
                return 1;
            }
            return 0;
    }
    return 0;
}

int CALLBACK InstructionDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            return 1;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK) {
                EndDialog(hDlg, LOWORD(wParam));
                return 1;
            }
            return 0;
    }
    return 0;
}

bool OnInitInputDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam) {
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_UPDOWN_CLASS | ICC_PROGRESS_CLASS;
    if (!InitCommonControlsEx(&iccx))
        return false;
    //RECT rc = { 20, 20, 100, 24 };
    //HWND  hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", 0,
    //    WS_CHILD | WS_VISIBLE, rc.left, rc.top, rc.right, rc.bottom,
    //    hDlg, (HMENU)IDC_EDIT2, GetModuleHandle(NULL), 0);
    //SetRect(&rc, 20, 60, 180, 20);
    const int kRanges[4][2] = { { 1,60 },{ 1,6 },{ 1,5 },{ 1,10 } };
    for (int i = 0; i < 4; i++) {
        HWND hUpdown = CreateWindowEx(0, UPDOWN_CLASS, nullptr,
            WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_WRAP,
            0, 0, 0, 0, hDlg, nullptr, GetModuleHandle(nullptr), nullptr);
        SendMessage(hUpdown, UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_EDIT2 + i), 0);
        SendMessage(hUpdown, UDM_SETRANGE32, kRanges[i][0], kRanges[i][1]);
    }

    HWND hTrack = CreateWindowEx(0, TRACKBAR_CLASS, nullptr,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS,
        125, 195, 85, 30, hDlg, (HMENU)IDC_TRACKBAR1, GetModuleHandle(nullptr), nullptr);
    SendMessage(hTrack, TBM_SETRANGE, 1, MAKELONG(1, 4));
    SendMessage(hTrack, TBM_SETPOS, 1, 1);
    SendMessage(hTrack, TBM_SETBUDDY, 1, (LPARAM)GetDlgItem(hDlg, IDC_STATIC1));
    SendMessage(hTrack, TBM_SETBUDDY, 0, (LPARAM)GetDlgItem(hDlg, IDC_STATIC2));

    const int kDefaults[4] = { 60,4,1,10 };
    for (int i = 0; i < 4; i++)
        SetDlgItemInt(hDlg, IDC_EDIT2 + i, kDefaults[i], 0);
    return true;
}

FILE *GetFilePtr(int mode) {
    wchar_t file[260];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = file;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(file);
    ofn.lpstrFilter = L"所有文件(*.*)\0*.*\0采蘑菇存档文件(*.mrs)\0*.mrs\0";
    ofn.nFilterIndex = 2;
    //ofn.lpstrFileTitle = NULL;
    //ofn.nMaxFileTitle = 0;
    //ofn.lpstrInitialDir = NULL;
    FILE *fp = nullptr;
    switch (mode) {
        case 0:
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&ofn) == 1) {
                _wfopen_s(&fp, file, L"rt+, ccs=UTF-8");
            }
            break;
        case 1:
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
            ofn.lpstrDefExt = L"mrs";
            if (GetSaveFileName(&ofn) == 1) {
                _wfopen_s(&fp, file, L"wt+, ccs=UTF-8");
            }
            break;
    }
    return fp;
}
