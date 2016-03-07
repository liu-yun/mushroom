#include "mushroom.h"

void GameMain(Game &game, Player &player, HDC hdc[]) {
    while (true) {
        GetAndDispatchCommand(game, player);
        if (!game.paused) {
            player.Move();
            GetGrassFocus(game, player);
            game.UpdateTimer();
            game.NewGrassTimer();
        }
        DrawGameGraphic(hdc, game, player);
        SleepMs(5);
        if (game.on_exit)
            return;
    }
}

wchar_t temp_name[11]; int temp_num[5];
bool InitNewGame(Game &game, Player &player) {
    if (DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_INPUTDIALOG), GetHWnd(), InputDialog) == IDCANCEL)
        return false;
    wcscpy_s(game.player_name, temp_name[0] ? temp_name : L"?");
    game.time_left = temp_num[0];
    game.init_num = temp_num[1];
    game.num_at_a_time = temp_num[2];
    game.interval = temp_num[3];
    player.speed = temp_num[4];
    game.InitGrass();
    return true;
}

void DrawGameGraphic(HDC hdc[], Game &game, Player &player) {
    //Info:Player name, Score, Time Center point X, Y
    const int kTextsXY[3][2] = { { 433,537 },{ 573,537 },{ 715,537 } };
    //Buttons:Start, Save, Clear, Quit X, Y
    const int kButtonsXY[4][2] = { { 15,505 },{ 95,505 },{ 175,505 },{ 255,505 } };
    const int kButtons[6][4] = { { 227,1,80,82 },
    { 632,1,80,82 },
    { 389,1,80,82 },
    { 470,1,80,82 },
    { 308,1,80,82 },
    { 551,1,80,82 } };
    const int kPlayer[4][4] = { { 1,1,32,32 },
    { 34,1,32,32 },
    { 67,1,32,32 },
    { 100,1,32,32 } };
    const int kMushrooms[9][4] = { { 1,92,64,90 },
    { 1201,1,65,90 },
    { 1157,1,43,90 },
    { 803,1,77,86 },
    { 713,1,89,83 },
    { 881,1,94,90 },
    { 133,1,93,80 },
    { 1055,1,101,90 },
    { 976,1,78,90 } };
    const int kGrass[3][4] = { { 324,92,187,140 },
    { 693,92,108,140 },
    { 1099,92,180,140 } };
    const int kGrassHighlight[3][4] = { { 802,92,187,140 },
    { 990,92,108,140 },
    { 512,92,180,140 } };
    const int kBomb[2][4] = { { 66,92,145,100 },
    { 212,92,111,103 } };
    const int kGrassX[4] = { 100,300,500,700 };
    const int kGrassY[3] = { 81,243,405 };
    wchar_t buffer[11];

    BeginBatchDraw();
    //draw background
    BitBlt(hdc[0], 0, 0, 800, 600, hdc[2], 0, 0, SRCCOPY);
    //draw mushrooms and scores
    GrassNode *p = game.h->next;
    while (p) {
        Transparent(hdc[0], kGrassX[p->x], kGrassY[p->y], hdc[1],
            game.grass_focus && p->id == game.grass_focus->id ?
            kGrassHighlight[p->grass_style] : kGrass[p->grass_style]);
        if (p->visible) {
            switch (p->type) {
                case MUSHROOM:
                    Transparent(hdc[0], kGrassX[p->x], kGrassY[p->y], hdc[1], kMushrooms[p->score - 1]);
                    break;
                case BOMB:
                    Transparent(hdc[0], kGrassX[p->x], kGrassY[p->y], hdc[1], kBomb[0]);
                    break;
                case NOTHING:
                    break;
            }
        }
        if (!p->visible) {
            _itow_s(p->score, buffer, 10);
            OutputText(hdc[0], kGrassX[p->x], kGrassY[p->y], buffer);
        }
        p = p->next;
    }
    //draw score and time
    OutputText(hdc[0], kTextsXY[0], game.player_name);
    _itow_s(game.score, buffer, 10);
    OutputText(hdc[0], kTextsXY[1], buffer);
    _itow_s(game.time_left, buffer, 10);
    OutputText(hdc[0], kTextsXY[2], buffer);
    //draw player
    Transparent(hdc[0], player.x, player.y, hdc[1], kPlayer[player.direction]);
    //draw buttons
    if (!game.paused)
        PutImage(hdc[0], kButtonsXY[0], hdc[1], kButtons[5]); //Pause
    if (game.button_on_click)
        PutImage(hdc[0], kButtonsXY[game.button_focus], hdc[1], kButtons[game.button_focus]);
    if (game.paused && game.button_on_click && game.button_focus == 0)
        PutImage(hdc[0], kButtonsXY[0], hdc[1], kButtons[4]); //Start on click
    EndBatchDraw();
}

void GetAndDispatchCommand(Game &game, Player &player) {
    const HCURSOR arrow = LoadCursor(nullptr, IDC_ARROW);
    const HCURSOR hand = LoadCursor(nullptr, IDC_HAND);
    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(65) & 0x8000)
        player.SetDirection(LEFT);
    if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(87) & 0x8000)
        player.SetDirection(UP);
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(68) & 0x8000)
        player.SetDirection(RIGHT);
    if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(83) & 0x8000)
        player.SetDirection(DOWN);
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        game.PickMushroom();
    if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        game.paused = !game.paused;
    MOUSEMSG message;
    if (MouseHit()) {
        message = GetMouseMsg();
        switch (message.uMsg) {
            case WM_MOUSEMOVE:
                game.button_focus = GetGameButtonFocus(message.x, message.y);
                if (game.button_focus == -1)
                    game.button_on_click = false;
                SetClassLong(GetHWnd(), GCL_HCURSOR, game.button_focus != -1 ? (long)hand : (long)arrow);
                break;
            case WM_LBUTTONDOWN:
                game.button_on_click = game.button_focus != -1 ? true : false;
                break;
            case WM_LBUTTONUP:
                game.button_on_click = false;
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
        }
    }
}

void GetGrassFocus(Game &game, Player &player) {
    const int kX[4] = { 100,300,500,700 };
    const int kY[3] = { 81,243,405 };
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

int GetGameButtonFocus(int x, int y) {
    const int kButtons[4][2] = { { 55,542 },{ 135,542 },{ 215,542 },{ 295,542 } };
    for (int i = 0; i < 4; i++) {
        if (pow(x - kButtons[i][0], 2) + pow(y - kButtons[i][1], 2) < 32 * 32)
            return i;
    }
    return -1;
}