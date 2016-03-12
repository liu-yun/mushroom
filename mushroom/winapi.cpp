#include "mushroom.h"

int CALLBACK InputDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            OnInitInputDialog(hDlg);
            return 1;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    GetDlgItemText(hDlg, IDC_EDIT1, temp_name, 11);
                    for (int i = 0; i < 4; i++)
                        temp_num[i] = GetDlgItemInt(hDlg, IDC_EDIT2 + i, nullptr, 0);
                    temp_num[4] = SendMessage(GetDlgItem(hDlg, IDC_TRACKBAR1), TBM_GETPOS, 0, 0);
                    EndDialog(hDlg, LOWORD(wParam));
                    return 1;
                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    return 1;
            }
            return 0;
    }
    return 0;
}

int CALLBACK HelpDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
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

int CALLBACK LeaderboardDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static wchar_t data[50][3][11];
    switch (message) {
        case WM_INITDIALOG:
            OnInitLeaderboardDialog(hDlg, data);
            return 1;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK) {
                EndDialog(hDlg, LOWORD(wParam));
                return 1;
            }
            return 0;
        case WM_NOTIFY:
            HandleSubitems(lParam, data);
            return 1;
    }
    return 0;
}

bool OnInitInputDialog(HWND hDlg) {
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_UPDOWN_CLASS | ICC_PROGRESS_CLASS;
    if (!InitCommonControlsEx(&iccx))
        return false;

    const int kRanges[4][2] = { { 1,3600 },{ 1,12 },{ 1,11 },{ 1,10 } };
    const int kDefaults[4] = { 60,4,1,2 };
    for (int i = 0; i < 4; i++) {
        HWND hUpdown = CreateWindowEx(0, UPDOWN_CLASS, nullptr,
            WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_WRAP,
            0, 0, 0, 0, hDlg, nullptr, GetModuleHandle(nullptr), nullptr);
        SendMessage(hUpdown, UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_EDIT2 + i), 0);
        SendMessage(hUpdown, UDM_SETRANGE32, kRanges[i][0], kRanges[i][1]);
        SetDlgItemInt(hDlg, IDC_EDIT2 + i, kDefaults[i], 0);
    }

    HWND hTrack = CreateWindowEx(0, TRACKBAR_CLASS, nullptr,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS,
        125, 195, 85, 30, hDlg, (HMENU)IDC_TRACKBAR1, GetModuleHandle(nullptr), nullptr);
    SendMessage(hTrack, TBM_SETRANGE, 1, MAKELONG(1, 4));
    SendMessage(hTrack, TBM_SETPOS, 1, 1);
    SendMessage(hTrack, TBM_SETBUDDY, 1, (LPARAM)GetDlgItem(hDlg, IDC_STATIC1));
    SendMessage(hTrack, TBM_SETBUDDY, 0, (LPARAM)GetDlgItem(hDlg, IDC_STATIC2));

    return true;
}

bool OnInitLeaderboardDialog(HWND hDlg, wchar_t data[50][3][11]) {
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_LISTVIEW_CLASSES;
    if (!InitCommonControlsEx(&iccx))
        return false;

    HWND hListview = CreateWindowEx(0, WC_LISTVIEW, nullptr,
        WS_CHILD | LVS_REPORT | WS_VISIBLE,
        16, 16, 248, 240, hDlg, (HMENU)IDC_LISTVIEW1, GetModuleHandle(nullptr), nullptr);

    wchar_t kHeaders[3][3] = { L"玩家",L"分数",L"日期" };
    const int kColumnWidth[3] = { 100,50,80 };
    LVCOLUMN column;
    column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    column.fmt = LVCFMT_LEFT;
    for (int i = 0; i < 3; i++) {
        column.iSubItem = i;
        column.pszText = kHeaders[i];
        column.cx = kColumnWidth[i];
        ListView_InsertColumn(hListview, i, &column);
    }

    wchar_t path[260];
    GetModuleFileName(nullptr, path, sizeof path / sizeof(wchar_t));
    PathRemoveFileSpec(path);
    wcscat_s(path, L"\\leaderboard.txt");
    FILE *fp;
    if (_wfopen_s(&fp, path, L"at+, ccs=UTF-8") == 1) {
        ErrorBox(L"fopen failed");
    }
    LVITEM item;
    item.mask = LVIF_TEXT | LVIF_STATE;
    item.stateMask = 0;
    item.iSubItem = 0;
    item.state = 0;
    for (int i = 0; !feof(fp) && i < 50; i++) {
        fwscanf_s(fp, L"%s\t%s\t%s\n", &data[i][0], sizeof data[i][0] / sizeof(wchar_t), &data[i][1], sizeof data[i][1] / sizeof(wchar_t), &data[i][2], sizeof data[i][2] / sizeof(wchar_t));
        item.pszText = data[i][0];
        item.iItem = i;
        ListView_InsertItem(hListview, &item);
    }
    fclose(fp);
    return true;
}

void HandleSubitems(LPARAM lParam, wchar_t data[50][3][11]) {
    NMLVDISPINFO* p;
    if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO) {
        p = (NMLVDISPINFO*)lParam;
        p->item.pszText = data[p->item.iItem][p->item.iSubItem];
    }
}

FILE *GetFilePtr(int mode) {
    wchar_t file[260];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof ofn);
    ofn.lStructSize = sizeof ofn;
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = file;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof file / sizeof(wchar_t);
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

void CreateGrayscaleBitmap(HDC hdc) {
    PBITMAPINFO p = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + (1 << 8) * sizeof(RGBQUAD));
    p->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    p->bmiHeader.biWidth = kWidth;
    p->bmiHeader.biHeight = kBottom;
    p->bmiHeader.biPlanes = 1;
    p->bmiHeader.biBitCount = 8;
    p->bmiHeader.biCompression = BI_RGB;
    p->bmiHeader.biSizeImage = p->bmiHeader.biWidth * p->bmiHeader.biHeight;
    p->bmiHeader.biClrUsed = 1 << 8;
    p->bmiHeader.biClrImportant = 0;
    for (SIZE_T i = 0; i < 1 << 8; i++) {
        p->bmiColors[i].rgbRed = (BYTE)i;
        p->bmiColors[i].rgbGreen = (BYTE)i;
        p->bmiColors[i].rgbBlue = (BYTE)i;
    }
    HBITMAP bitmap = CreateDIBSection(hdc, p, 0, DIB_RGB_COLORS, nullptr, 0);
    SelectObject(hdc, bitmap);
}

void MRSetCursor(int focus) {
    static HCURSOR arrow = LoadCursor(nullptr, IDC_ARROW);
    static HCURSOR hand = LoadCursor(nullptr, IDC_HAND);
    SetClassLong(GetHWnd(), GCL_HCURSOR, focus != -1 ? (long)hand : (long)arrow);
}