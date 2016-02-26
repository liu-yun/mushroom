#include "mushroom.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    IMAGE images[2];
    HDC hdc[3];
    InitScene(images, hdc);
    GameMain(hdc);
    return 0;
}

void GameMain(HDC hdc[]) {
    Game game;
    Player player;
    //DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INSTRUCTIONDIALOG), GetHWnd(), InstructionDialog);
    GetGameData(game, player);

    while (true) {
        GetAndDispatchCommand(game, player);
        if (!game.paused) {
            player.Move();
            GetGrassFocus(game, player);
            game.UpdateTimer();
            game.NewGrassTimer();
        }
        DrawGraphic(hdc, game, player);
        SleepMs(5);
    }
}

void InitScene(IMAGE *images, HDC hdc[]) {
    initgraph(kWidth, kHeight);
    SetWindowText(GetHWnd(), L"采蘑菇");
    //Fix the blurry taskbar icon.
    HICON icon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_MUSHROOM));
    SendMessage(GetHWnd(), WM_SETICON, ICON_BIG, (LPARAM)icon);
    DestroyIcon(icon);

    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 30;
    wcscpy_s(f.lfFaceName, L"微软雅黑");
    f.lfQuality = CLEARTYPE_QUALITY;
    settextstyle(&f);

    loadimage(images + 0, L"Image", MAKEINTRESOURCE(IDR_IMAGE1)); //ImageRes
    loadimage(images + 1, L"Image", MAKEINTRESOURCE(IDR_UI)); //Background
    hdc[0] = GetImageHDC();
    hdc[1] = GetImageHDC(images);
    hdc[2] = GetImageHDC(images + 1);
    SetTextColor(hdc[0], BLACK);
    SetBkMode(hdc[0], TRANSPARENT);
    SetTextAlign(hdc[0], TA_CENTER);
}

void DrawGraphic(HDC hdc[], Game &game, Player &player) {
    //Buttons:Start, Save, Clear, Quit
    const int kButtons[4][4] = { {55,542,64,64},{135,542,64,64},{215,542,64,64},{295,542,64,64} };//X, Y, W, H
    //Info:Player name, Score, Time
    const int kTexts[3][2] = { {433,537},{573,537},{715,537} };//Center point X, Y
    const int kPlayer[4][4] = { { 461,424,32,32 },
    { 494,424,32,32 },
    { 1,515,32,32 },
    { 34,515,32,32 } };
    const int kMushrooms[9][4] = { { 291,283,64,90 },
    { 356,283,65,90 },
    { 422,283,43,90 },
    { 466,283,77,86 },
    { 1,424,89,83 },
    { 91,424,94,90 },
    { 186,424,93,80 },
    { 280,424,101,90 },
    { 382,424,78,90 } };
    const int kGrass[3][4] = { { 259,1,187,140 },
    { 1,142,108,140 },
    { 110,142,180,140 } };
    const int kGrassHighlight[3][4] = { { 291,142,187,140 },
    { 1,283,108,140 },
    { 110,283,180,140 } };
    const int kBomb[2][4] = { { 1,1,145,100 },
    { 147,1,111,103 } };
    const int kX[] = { 100,300,500,700 };
    const int kY[] = { 81,243,405 };
    wchar_t buffer[10];

    BeginBatchDraw();
    //draw background
    BitBlt(hdc[0], 0, 0, 800, 600, hdc[2], 0, 0, SRCCOPY);

    //draw mushrooms scores
    GrassNode *p = game.h->next;
    while (p) {
        Transparent(hdc[0], kX[p->x], kY[p->y], hdc[1],
            game.grass_focus && p->id == game.grass_focus->id ?
            kGrassHighlight[p->grass_style] : kGrass[p->grass_style]);
        if (p->visible) {
            switch (p->type) {
                case MUSHROOM:
                    Transparent(hdc[0], kX[p->x], kY[p->y], hdc[1], kMushrooms[p->score - 1]);
                    break;
                case BOMB:
                    Transparent(hdc[0], kX[p->x], kY[p->y], hdc[1], kBomb[0]);
                    break;
                case NOTHING:
                    break;
            }
        }
        if (!p->visible) {
            _itow_s(p->score, buffer, 10);
            OutputText(hdc[0], kX[p->x], kY[p->y], buffer);
        }
        p = p->next;
    }

    //draw score, time (if needed)
    OutputText(hdc[0], kTexts[0], game.player_name); //outtextxy(texts[0][0], texts[0][1], game.player_name);
    _itow_s(game.score, buffer, 10);
    OutputText(hdc[0], kTexts[1], buffer);
    _itow_s(game.time_left, buffer, 10);
    OutputText(hdc[0], kTexts[2], buffer);
    //draw player
    Transparent(hdc[0], player.x, player.y, hdc[1], kPlayer[player.direction]);
    //draw buttons
    EndBatchDraw();

    //Transparent(hdc[2], player.x, player.y, hdc[0], im[player.direction][2], im[player.direction][3], im[player.direction][0], im[player.direction][1]);
    //MoveToEx(hdc, 10, 10, NULL);
    //LineTo(hdc, 100, 100);
    //DEBUG
    //TransparentBlt(images_hdc[2], 0, 0, 793, 233, hdc[0], 0, 0, 793, 233, 0x0);
}

void GetGameData(Game &game, Player &player) {
    DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_INPUTDIALOG), GetHWnd(), InputDialog);
    wcscpy_s(game.player_name, temp_name[0] ? temp_name : L"?");
    game.time_left = temp_num[0];
    game.init_num = temp_num[1];
    game.num_at_a_time = temp_num[2];
    game.interval = temp_num[3];
    player.speed = temp_num[4];
    game.InitGrass();
}

void GetAndDispatchCommand(Game &game, Player &player) {
    const HCURSOR arrow = LoadCursor(nullptr, IDC_ARROW);
    const HCURSOR hand = LoadCursor(nullptr, IDC_HAND);
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        player.SetDirection(LEFT);
    if (GetAsyncKeyState(VK_UP) & 0x8000)
        player.SetDirection(UP);
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        player.SetDirection(RIGHT);
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        player.SetDirection(DOWN);
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        game.PickMushroom();
    }
    MOUSEMSG message;
    if (MouseHit()) {
        message = GetMouseMsg();
        switch (message.uMsg) {
            case WM_MOUSEMOVE:
                game.button_focus = GetButtonFocus(message.x, message.y);
                SetClassLong(GetHWnd(), GCL_HCURSOR, game.button_focus != -1 ? (long)hand : (long)arrow);
                break;
            case WM_LBUTTONDOWN:
                game.button_on_click = game.button_focus != -1 ? true : false;
                break;
            case WM_LBUTTONUP:
                switch (game.button_focus) {
                    case 0:
                        game.paused = !game.paused;
                        break;
                    case 1:
                        SaveGameToFile(game, player);
                        break;
                    case 2:
                        game.ClearGrass();
                        break;
                    case 3:
                        game.ExitGame();
                        break;
                    default:
                        break;
                }
                break;
            case WM_RBUTTONUP:
                break;
        }
    }
}

void GetGrassFocus(Game &game, Player &player) {
    const int kX[] = { 100,300,500,700 };
    const int kY[] = { 81,243,405 };
    GrassNode *p = game.h->next;
    while (p) {
        if (pow(player.x - kX[p->x], 2) + pow(player.y - kY[p->y], 2) < 70 * 70) {
            game.grass_focus = p;
            return;
        }
        p = p->next;
    }
    game.grass_focus = nullptr;
}

Game::Game() {
    time_left = 0;
    score = 0;
    init_num = 5;
    num_at_a_time = 1;
    interval = 10;
    grass_num = 0;
    last_id = 0;
    //h = InitGrass();
    h = nullptr;
    grass_focus = nullptr;
    button_focus = -1;
    button_on_click = false;
    paused = true;
}

void Game::UpdateTimer() {
    static clock_t old_clock = clock();
    if (clock() - old_clock >= 1000) {
        time_left--;
        old_clock = clock();
    }
    if (time_left == -1)
        Timeout();
}

void Game::NewGrassTimer() {
    static clock_t old_clock = clock();
    if (clock() - old_clock >= interval * 1000) {
        old_clock = clock();
        NewGrass();
    }
}

void Game::InitGrass() {
    GrassNode *p, *s;
    p = h = new GrassNode(-1);
    for (int i = 0; i < init_num; i++) {
        s = new GrassNode(i);
        p->next = s;
        p = s;
    }
    grass_num = init_num;
    last_id = init_num;
}

void Game::NewGrass() {
    GrassNode *p = h;
    while (p->next) {
        p = p->next;
    }
    for (int i = 0; i < num_at_a_time && grass_num < 12; i++) {
        p->next = new GrassNode(++last_id);
        grass_num++;
        p = p->next;
    }
}

void Game::DeleteGrass(int id) {
    GrassNode *p = h;
    while (p) {
        if (p->next->id == id) {
            p->next = p->next->next;
            delete p->next;
            grass_num--;
            return;
        }
        p = p->next;
    }
}

void Game::ClearGrass() {
    GrassNode *p = h, *q = h->next;
    while (q) {
        delete p;
        p = q;
        q = q->next;
    }
    delete p;

    h = new GrassNode(0);
    grass_num = 0;
    last_id = 0;
}

void Game::PickMushroom() {
    if (!grass_focus)
        return;
    if (grass_focus->visible)
        return;
    grass_focus->visible = true;
    switch (grass_focus->type) {
        case MUSHROOM:
            score += grass_focus->score;
            break;
        case BOMB:
            score -= grass_focus->score;
            break;
        case NOTHING:
            break;
        default:
            break;
    }
}

void Game::GameOver() {
    wchar_t buffer[100];
    swprintf_s(buffer, L"Game over!\nScore:%d", score);
    InfoBox(buffer);
    FILE *fp;
    if (_wfopen_s(&fp, L"highscores.txt", L"at+, ccs=UTF-8") == 1) {
        ErrorBox(L"fopen failed");
        return;
    }
    fwprintf_s(fp, L"%s\t%d", player_name, score);
    fclose(fp);
}

void Game::Timeout() {
    switch (YesNoBox(L"是否记录分数？")) {
        case IDYES:
            GameOver();
        case IDNO:
            closegraph();
            exit(0);
    }
}

void Game::ExitGame() {
    switch (YesNoCancelBox(L"是否退出并记录分数？\n点击“取消”可返回。")) {
        case IDYES:
            GameOver();
        case IDNO:
            closegraph();
            exit(0);
        case IDCANCEL:
            break;
    }
}

void SaveGameToFile(Game &game, Player &player) {
    //wchar_t kMushroom[20];
    //LoadStringW(GetModuleHandle(NULL), IDS_MUSHROOM, kMushroom, 20);
    //HANDLE hf;
    //if (GetSaveFileName(&ofn) == TRUE) {
    //    hf = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, 0,
    //        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    //}
    //else {
    //    ErrorBox(L"Error");
    //}
    FILE *fp = GetFilePtr(1);
    if (!fp) {
        ErrorBox(L"未保存");
        return;
    }
    fwprintf_s(fp, L"%s\t%d\t%d\t%d\t%d\t%d\t%d\n", game.player_name, game.time_left, game.score, game.grass_num, game.num_at_a_time, game.interval, game.last_id);
    fwprintf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\n", player.x, player.y, player.dx, player.dy, player.speed, player.direction);
    GrassNode *p = game.h->next;
    for (int i = 0; i < game.grass_num; i++) {
        fwprintf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p->id, p->type, p->grass_style, p->score, p->x, p->y, p->visible);
        p = p->next;
    }
    fclose(fp);
    InfoBox(L"已保存");
}

void ReadGameFromFile(Game &game, Player &player) {
    FILE *fp = GetFilePtr(0);
    if (!fp) {
        InfoBox(L"读取失败");
        return;
    }
    fwscanf_s(fp, L"%s\t%d\t%d\t%d\t%d\t%d\t%d\n", &game.player_name, 10, &game.time_left, &game.score, &game.grass_num, &game.num_at_a_time, &game.interval, &game.last_id);
    fwscanf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\n", &player.x, &player.y, &player.dx, &player.dy, &player.speed, &player.direction);
    GrassNode *p = game.h, *s;
    int bool_temp;//hack C4477
    for (int i = 0; i < game.grass_num; i++) {
        s = new GrassNode(i);
        fwscanf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &p->id, &p->type, &p->grass_style, &p->score, &p->x, &p->y, &bool_temp);
        p->visible = bool_temp != 0;
        GrassNode::grid[p->y][p->x] = 1;
        p->next = s;
        p = p->next;
    }
    fclose(fp);
}

void SleepMs(int ms) {
    static clock_t oldclock = clock();
    oldclock += ms;
    if (clock() > oldclock) {
        oldclock = clock();
        return;
    }
    while (clock() < oldclock)
        Sleep(1);

}

int GetButtonFocus(int x, int y) {
    const int button[4][2] = { {55,542},{135,542},{215,542},{295,542} };
    for (int i = 0; i < 4; i++) {
        if (pow(x - button[i][0], 2) + pow(y - button[i][1], 2) < 32 * 32)
            return i;
    }
    return -1;
}

GrassNode::GrassNode(int i) : id(i) {
    srand(clock());
    visible = false;
    next = nullptr;
    if (id == -1) {
        type = NOTHING;
        return;
    }
    type = rand() % 3;
    grass_style = rand() % 3;
    score = rand() % 9 + 1;
    do {
        x = rand() % 4;
        y = rand() % 3;
    } while (grid[y][x] == 1);
    grid[y][x] = 1;
    Sleep(1);
}

int GrassNode::grid[3][4] = { 0 };

Player::Player() {
    x = kWidth / 2 - 16;
    y = kBottom - 16 - 1;
    speed = 2;
    direction = UP;
    dx = 0;
    dy = -1;
}

void Player::Move() {
    if (x <= 16 || y <= 16 || x >= kWidth - 16 || y >= kBottom - 16) {
        dx = -dx;
        dy = -dy;
        direction += direction > 1 ? -2 : 2;
        x = x <= 16 ? 16 + 1 : x;
        y = y <= 16 ? 16 + 1 : y;
        x = x >= kWidth - 16 ? kWidth - 16 - 1 : x;
        y = y >= kBottom - 16 ? kBottom - 16 - 1 : y;
        /*if (x <= 16)
            x = 16 + 1;
        if (y <= 16)
            y = 16 + 1;
        if (x >= 800 - 16)
            x = 800 - 16 - 1;
        if (y >= 490 - 16)
            y = 490 - 16 - 1;*/
    }
    x += speed*dx;
    y += speed*dy;
}

void Player::SetDirection(Direction d) {
    direction = d;
    switch (d) {
        case LEFT:
            dx = -1;
            dy = 0;
            break;
        case UP:
            dx = 0;
            dy = -1;
            break;
        case RIGHT:
            dx = 1;
            dy = 0;
            break;
        case DOWN:
            dx = 0;
            dy = 1;
            break;
        default:
            break;
    }
}

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
    const int kRanges[4][2] = { {1,60},{1,6},{1,5},{1,10} };
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
