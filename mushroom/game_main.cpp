#include "mushroom.h"

void GameMain(Game &game, Player &player, HDC hdc[]) {
    while (!game.on_exit) {
        GetAndDispatchCommand(game, player);
        if (!game.paused) {
            player.Move();
            GetGrassFocus(game, player);
            game.GameTimer();
            game.GrassTimer();
        }
        DrawGameGraphic(hdc, game, player);
        SleepMs(5);
    }
}

wchar_t temp_name[11]; int temp_num[6];
bool InitNewGame(Game &game, Player &player) {
    if (DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_INPUTDIALOG), GetHWnd(), InputDialog) == IDCANCEL)
        return false;
    srand(clock());
    wchar_t emoji[3] = { (wchar_t)55357,(wchar_t)(56832 + rand() % 80),(wchar_t)0 }; //Unicode U+1F600-U+1F64F
    wcscpy_s(game.player_name, temp_name[0] ? temp_name : emoji);
    game.time_left = temp_num[0];
    game.init_num = temp_num[1];
    game.num_at_a_time = temp_num[2];
    game.interval = temp_num[3];
    player.speed = temp_num[4];
    player.skin = temp_num[5];
    player.Reset();
    game.InitGrass();
    game.score = 0;
    game.HandleReturnKey();
    game.paused = true;
    game.grayscale_ready = false;
    return true;
}

void DrawGameGraphic(HDC hdc[], Game &game, Player &player) {
    //Info:Player name, Score, Time Center point X, Y
    const int kTextsXY[3][2] = { { 433,537 },{ 573,537 },{ 715,537 } };
    //Buttons:Start, Save, Clear, Quit X, Y
    const int kButtonsXY[4][2] = { { 14,505 },{ 94,505 },{ 174,505 },{ 254,505 } };
    const int kButtons[6][4] = { { 487,246,80,80 },{ 1,327,80,80 },{ 406,246,80,80 },{ 325,246,80,80 },{ 244,246,80,80 },{ 568,246,80,80 } };
    const int kPlayer[8][4] = { { 34,1,32,32 },{ 100,1,32,32 },{ 1,1,32,32 },{ 67,1,32,32 },{ 565,165,67,80 },{ 503,165,61,80 },{ 95,246,67,80 },{ 163,246,80,80 } };
    const int kMushrooms[9][4] = { { 329,327,64,90 },{ 1,418,65,90 },{ 591,327,43,90 },{ 172,327,77,86 },{ 82,327,89,83 },{ 496,327,94,90 },{ 1,246,93,80 },{ 394,327,101,90 },{ 250,327,78,90 } };
    const int kGrass[3][4] = { { 1,559,187,140 },{ 325,418,108,140 },{ 434,418,180,140 } };
    const int kGrassHighlight[3][4] = { { 189,559,187,140 },{ 558,559,108,140 },{ 377,559,180,140 } };
    const int kBomb[2][4] = { { 67,418,145,100 },{ 213,418,111,103 } };
    const int kGrassX[4] = { 100,300,500,700 };
    const int kGrassY[3] = { 81,243,405 };
    wchar_t buffer[11];

    BeginBatchDraw();
    //draw background
    BitBlt(hdc[0], 0, 0, kWidth, kHeight, hdc[2], 0, 0, SRCCOPY);
    if (!game.grayscale_ready || !game.paused) {
        //draw mushrooms and its scores
        GrassNode *p = game.h->next;
        while (p) {
            MRTransparentBlt(hdc[0], kGrassX[p->x], kGrassY[p->y], hdc[1],
                game.grass_focus && p->id == game.grass_focus->id ?
                kGrassHighlight[p->grass_style] : kGrass[p->grass_style]);
            if (p->picked) {
                switch (p->type) {
                    case MUSHROOM:
                        MRTransparentBlt(hdc[0], kGrassX[p->x], kGrassY[p->y], hdc[1], kMushrooms[p->score - 1]);
                        break;
                    case BOMB:
                        MRTransparentBlt(hdc[0], kGrassX[p->x], kGrassY[p->y], hdc[1], kBomb[p->exploded ? 1 : 0]);
                        break;
                    case NOTHING:
                        break;
                }
            }
            if (!p->picked) {
                _itow_s(p->score, buffer, 10);
                MRTextOut(hdc[0], kGrassX[p->x], kGrassY[p->y], buffer);
            }
            p = p->next;
        }
        //draw player
        MRTransparentBlt(hdc[0], player.x, player.y, hdc[1], kPlayer[player.skin * 4 + player.direction]);
    }
    //grayscale when paused
    if (game.paused) {
        if (!game.grayscale_ready) {
            BitBlt(hdc[4], 0, 0, kWidth, kBottom, hdc[0], 0, 0, SRCCOPY);
            game.grayscale_ready = true;
        }
        BitBlt(hdc[0], 0, 0, kWidth, kBottom, hdc[4], 0, 0, SRCCOPY);
    }
    //draw score and time
    MRTextOut(hdc[0], kTextsXY[0], game.player_name);
    _itow_s(game.score, buffer, 10);
    MRTextOut(hdc[0], kTextsXY[1], buffer);
    swprintf_s(buffer, sizeof buffer / sizeof(wchar_t), L"%02d:%02d", game.time_left / 60, game.time_left % 60);
    MRTextOut(hdc[0], kTextsXY[2], buffer);
    //draw buttons
    if (!game.paused)
        MRBitBlt(hdc[0], kButtonsXY[0], hdc[1], kButtons[5]); //Pause
    if (game.button_on_click)
        MRBitBlt(hdc[0], kButtonsXY[game.button_focus], hdc[1], kButtons[game.button_focus]);
    if (game.paused && game.button_on_click && game.button_focus == 0)
        MRBitBlt(hdc[0], kButtonsXY[0], hdc[1], kButtons[4]); //Start on click
    EndBatchDraw();
}

void GetAndDispatchCommand(Game &game, Player &player) {
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
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        game.HandleReturnKey();
    }
    MOUSEMSG message;
    if (MouseHit()) {
        message = GetMouseMsg();
        switch (message.uMsg) {
            case WM_MOUSEMOVE:
                game.button_focus = GetGameButtonFocus(message.x, message.y);
                if (game.button_focus == -1)
                    game.button_on_click = false;
                MRSetCursor(game.button_focus);
                break;
            case WM_LBUTTONDOWN:
                game.button_on_click = game.button_focus != -1 ? true : false;
                break;
            case WM_LBUTTONUP:
                game.button_on_click = false;
                switch (game.button_focus) {
                    case 0:
                        game.paused = !game.paused;
                        game.grayscale_ready = false;
                        break;
                    case 1:
                        SaveGameToFile(game, player);
                        break;
                    case 2:
                        game.ClearGrass();
                        game.grayscale_ready = false;
                        break;
                    case 3:
                        game.ExitGame(false);
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