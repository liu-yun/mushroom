#include "mushroom.h"

void GameMenu(Game &game, Player &player, HDC hdc[]) {
    while (true) {
        GetAndDispatchMenuCommand(game, player, hdc);
        DrawMenuGraphic(hdc, game);
        SleepMs(5);
    }
}

void DrawMenuGraphic(HDC hdc[], Game &game) {
    const int kMenuItemsXY[5][2] = { { 430,180 },{ 430,250 },{ 430,320 },{ 430,390 },{ 430,460 } };
    const int kMenuItems[10][4] = { { 252,124,250,40 },{ 1,124,250,40 },{ 252,165,250,40 },{ 252,83,250,40 },{ 133,1,250,40 },{ 384,1,250,40 },{ 252,42,250,40 },{ 1,83,250,40 },{ 1,42,250,40 },{ 1,165,250,40 } };
    BeginBatchDraw();
    BitBlt(hdc[0], 0, 0, kWidth, kHeight, hdc[3], 0, 0, SRCCOPY);
    if (game.button_focus != -1 && !game.button_on_click)
        MRBitBlt(hdc[0], kMenuItemsXY[game.button_focus], hdc[1], kMenuItems[game.button_focus]);
    if (game.button_on_click)
        MRBitBlt(hdc[0], kMenuItemsXY[game.button_focus], hdc[1], kMenuItems[5 + game.button_focus]);
    EndBatchDraw();
}

void GetAndDispatchMenuCommand(Game &game, Player &player, HDC hdc[]) {
    MOUSEMSG message;
    if (MouseHit()) {
        message = GetMouseMsg();
        switch (message.uMsg) {
            case WM_MOUSEMOVE:
                game.button_focus = GetMenuButtonFocus(message.x, message.y);
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
                        game.on_exit = false;
                        game.button_focus = -1;
                        if (InitNewGame(game, player))
                            GameMain(game, player, hdc);
                        break;
                    case 1:
                        game.on_exit = false;
                        game.button_focus = -1;
                        if (LoadGameFromFile(game, player))
                            GameMain(game, player, hdc);
                        break;
                    case 2:
                        DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_LEADERBOARDDIALOG), GetHWnd(), LeaderboardDialog);
                        break;
                    case 3:
                        ShowHelpDialog();
                        break;
                    case 4:
                        closegraph();
                        exit(0);
                }
                break;
        }
    }
}

int GetMenuButtonFocus(int x, int y) {
    const int kButtons[5][2] = { { 555,200 },{ 555,270 },{ 555,340 },{ 555,410 },{ 555,480 } };
    for (int i = 0; i < 5; i++) {
        if (x > kButtons[i][0] - 125 && y > kButtons[i][1] - 20 && x < kButtons[i][0] + 125 && y < kButtons[i][1] + 20)
            return i;
    }
    return -1;
}