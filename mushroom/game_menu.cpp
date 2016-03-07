#include "mushroom.h"

void GameMenu(Game &game, Player &player, HDC hdc[]) {
    while (true) {
        GetAndDispatchMenuCommand(game, player, hdc);
        DrawMenuGraphic(hdc, game);
        SleepMs(5);
    }
}

void DrawMenuGraphic(HDC hdc[], Game &game) {
    BeginBatchDraw();
    BitBlt(hdc[0], 0, 0, 800, 600, hdc[3], 0, 0, SRCCOPY);
    EndBatchDraw();
}

void GetAndDispatchMenuCommand(Game &game, Player &player, HDC hdc[]) {
    const HCURSOR arrow = LoadCursor(nullptr, IDC_ARROW);
    const HCURSOR hand = LoadCursor(nullptr, IDC_HAND);
    MOUSEMSG message;
    if (MouseHit()) {
        message = GetMouseMsg();
        switch (message.uMsg) {
            case WM_MOUSEMOVE:
                game.button_focus = GetMenuButtonFocus(message.x, message.y);
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
                        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LEADERBOARDDIALOG), GetHWnd(), LeaderboardDialog);
                        break;
                    case 3:
                        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_HELPDIALOG), GetHWnd(), HelpDialog);
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