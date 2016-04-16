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
                    temp_num[5] = SendMessage(GetDlgItem(hDlg, IDC_COMBOBOXEX1), CB_GETCURSEL, 0, 0);
                    EndDialog(hDlg, LOWORD(wParam));
                    return 1;
                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    return 1;
            }
            return 0;
        case WM_CLOSE:
            ImageList_Destroy((HIMAGELIST)GetWindowLongPtr(hDlg, GWLP_USERDATA));
            DestroyWindow(hDlg);
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
            if (LOWORD(wParam) == IDOK || IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return 1;
            }
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code) {
                case LVN_GETDISPINFO:
                    NMLVDISPINFO* p;
                    p = (NMLVDISPINFO*)lParam;
                    p->item.pszText = data[p->item.iItem][p->item.iSubItem - 1];
                    break;
                case LVN_COLUMNCLICK:
                    static bool descending = false;
                    static int n = ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTVIEW1));
                    LPNMLISTVIEW pl = (LPNMLISTVIEW)lParam;
                    SortData(data, n, pl->iSubItem - 1, !descending);
                    descending = !descending;
                    ListView_RedrawItems(GetDlgItem(hDlg, IDC_LISTVIEW1), 0, n);
                    break;
            }
            return 1;
    }
    return 0;
}

void SortData(wchar_t data[50][3][11], int n, int item, bool descending) {
    if (item != 1)
        return;
    wchar_t temp[3][11];
    int a, b, i, j;
    for (i = n - 1; i >= 1; i--) {
        for (j = 0; j < i; j++) {
            a = _wtoi(data[j][item]);
            b = _wtoi(data[j + 1][item]);
            if (descending ? a < b : a > b) {
                for (int k = 0; k < 3; k++) {
                    wcscpy_s(temp[k], data[j][k]);
                    wcscpy_s(data[j][k], data[j + 1][k]);
                    wcscpy_s(data[j + 1][k], temp[k]);
                }
            }
        }
    }
}

bool OnInitInputDialog(HWND hDlg) {
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_UPDOWN_CLASS | ICC_PROGRESS_CLASS | ICC_USEREX_CLASSES;
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
        130, 195, 80, 30, hDlg, (HMENU)IDC_TRACKBAR1, GetModuleHandle(nullptr), nullptr);
    SendMessage(hTrack, TBM_SETRANGE, 1, MAKELONG(1, 3));
    SendMessage(hTrack, TBM_SETPOS, 1, 1);
    SendMessage(hTrack, TBM_SETBUDDY, 1, (LPARAM)GetDlgItem(hDlg, IDC_STATIC1));
    SendMessage(hTrack, TBM_SETBUDDY, 0, (LPARAM)GetDlgItem(hDlg, IDC_STATIC2));

    const int kSkinIcons[] = { IDI_MUSHROOM,  IDI_MUSHROOM };
    const int kSkinNum = _countof(kSkinIcons);
    wchar_t kSkinNames[2][10] = { L"Ball",L"Umaru" };
    HWND hComboEx = CreateWindowEx(0, WC_COMBOBOXEX, nullptr,
        CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP | WS_VISIBLE,
        120, 230, 100, 90, hDlg, (HMENU)IDC_COMBOBOXEX1, GetModuleHandle(nullptr), nullptr);
    HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_MASK | ILC_COLOR32, kSkinNum, 0);
    for (int i = 0; i < kSkinNum; i++)
        ImageList_AddIcon(hImageList, LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(kSkinIcons[i])));
    SendMessage(hComboEx, CBEM_SETIMAGELIST, 0, (LPARAM)hImageList);
    for (int i = 0; i < kSkinNum; i++) {
        COMBOBOXEXITEM item = { 0 };
        item.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
        item.iItem = i;
        item.iImage = i;
        item.iSelectedImage = i;
        item.pszText = kSkinNames[i];
        SendMessage(hComboEx, CBEM_INSERTITEM, 0, (LPARAM)&item);
    }
    SendMessage(hComboEx, CB_SETCURSEL, 0, 0);
    SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)hImageList);
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
        16, 16, 248, 270, hDlg, (HMENU)IDC_LISTVIEW1, GetModuleHandle(nullptr), nullptr);
    ListView_SetExtendedListViewStyle(hListview, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER);

    wchar_t kHeaders[4][3] = { L"",L"玩家",L"分数",L"日期" };
    const int kColumnWidth[4] = { 0,100,50,80 };
    LVCOLUMN column;
    column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    column.fmt = LVCFMT_LEFT;
    for (int i = 0; i < 4; i++) {
        column.iSubItem = i;
        column.pszText = kHeaders[i];
        column.cx = kColumnWidth[i];
        ListView_InsertColumn(hListview, i, &column);
    }

    wchar_t path[260];
    GetModuleFileName(nullptr, path, sizeof path / sizeof(wchar_t));
    PathRemoveFileSpec(path);
    wcscat_s(path, L"\\leaderboard.txt");
    wfstream f;
    f.open(path, ios_base::in);
    if (!f.is_open())
        return false;
    f.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    LVITEM item;
    item.mask = LVIF_TEXT | LVIF_STATE;
    item.stateMask = 0;
    item.iSubItem = 0;
    item.state = 0;
    for (int i = 0; !f.eof() && i < 50; i++) {
        f >> data[i][0] >> data[i][1] >> data[i][2];
        item.pszText = L"";
        item.iItem = i;
        ListView_InsertItem(hListview, &item);
    }
    f.close();
    return true;
}

int ShowExitGameDialog(int score, bool timeout) {
    int selected = 0;
    wchar_t buffer[30];
    swprintf_s(buffer, L"分数: %d\n是否记录分数?", score);
    TASKDIALOGCONFIG config = { 0 };
    TASKDIALOG_BUTTON buttons[] = { { IDYES,L"记录" },{ IDNO,L"不记录" } };
    config.cbSize = sizeof config;
    config.hInstance = GetModuleHandle(nullptr);
    config.hwndParent = GetHWnd();
    config.dwCommonButtons = timeout ? 0 : TDCBF_CANCEL_BUTTON;
    config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_USE_COMMAND_LINKS;
    config.pButtons = buttons;
    config.cButtons = _countof(buttons);
    config.pszWindowTitle = kMushroom;
    config.pszMainIcon = TD_WARNING_ICON;
    config.pszMainInstruction = timeout ? L"游戏结束！" : L"退出游戏";
    config.pszContent = buffer;
    TaskDialogIndirect(&config, &selected, nullptr, nullptr);
    return selected;
}

void ShowHelpDialog() {
    wchar_t buffer[30], copyright[110];
    mbstowcs_s(nullptr, buffer, __DATE__ " " __TIME__, sizeof buffer / sizeof(wchar_t));
    swprintf_s(copyright, sizeof copyright / sizeof(wchar_t), L"版本 %s (%s) %s\n© 2016 刘云 15071018 保留所有权利。\nPhoto credits © 2016 iStockphoto", kVersion, buffer, kBuildType);
    TASKDIALOGCONFIG config = { 0 };
    config.cbSize = sizeof config;
    config.hInstance = GetModuleHandle(nullptr);
    config.hwndParent = GetHWnd();
    config.dwCommonButtons = TDCBF_OK_BUTTON;
    config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION /*| TDF_EXPAND_FOOTER_AREA*/;
    config.pszWindowTitle = L"帮助";
    config.pszMainIcon = MAKEINTRESOURCE(IDI_MUSHROOM);
    config.pszMainInstruction = L"这是一个有趣的采蘑菇游戏！";
    config.pszContent = L"使用 ↑↓←→ 或 WASD 键控制移动方向，按空格键尝试采蘑菇。\n采到蘑菇得分，采到炸弹减分。分数随机，范围在 1 到 9 之间。踩到炸弹后有 0.5 秒的躲避时间。\n点击清空按钮可将游戏区所有的草、蘑菇、炸弹清除。\n点击保存按钮可保存游戏状态。";
    config.pszExpandedControlText = L"？？？";
    config.pszExpandedInformation = L"？？？";
    config.pszFooterIcon = TD_INFORMATION_ICON;
    config.pszFooter = copyright;
    TaskDialogIndirect(&config, nullptr, nullptr, nullptr);
}

wfstream GetFileStream(int mode, wchar_t* filename) {
    wchar_t filepath[260] = { 0 };
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof ofn);
    ofn.lStructSize = sizeof ofn;
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = filepath;
    ofn.nMaxFile = sizeof filepath / sizeof(wchar_t);
    ofn.lpstrFilter = L"所有文件(*.*)\0*.*\0采蘑菇存档文件(*.mrs)\0*.mrs\0";
    ofn.nFilterIndex = 2;
    ofn.Flags = OFN_PATHMUSTEXIST;
    if (filename) {
        wcscpy_s(filepath, filename);
        ofn.lpstrFileTitle = filename;
        ofn.nMaxFileTitle = 30;
    }
    wfstream f;
    switch (mode) {
        case 0:
            ofn.Flags |= OFN_FILEMUSTEXIST;
            ofn.lpstrTitle = L"载入游戏";
            if (GetOpenFileName(&ofn) == 1) {
                f.open(filepath, ios_base::in);
            }
            break;
        case 1:
            ofn.Flags |= OFN_OVERWRITEPROMPT;
            ofn.lpstrDefExt = L"mrs";
            ofn.lpstrTitle = L"保存游戏";
            if (GetSaveFileName(&ofn) == 1) {
                f.open(filepath, ios_base::out);
            }
            break;
    }
    f.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
    return f;
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
