#include "mushroom.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    IMAGE images[3];
    HDC hdc[5];
    Game game;
    Player player;
    InitScene(images, hdc);
    GameMenu(game, player, hdc);
    return 0;
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

    loadimage(images + 0, L"IMAGE", MAKEINTRESOURCE(IDR_IMAGERES));
    loadimage(images + 1, L"IMAGE", MAKEINTRESOURCE(IDR_UI));
    loadimage(images + 2, L"IMAGE", MAKEINTRESOURCE(IDR_MENU));
    hdc[0] = GetImageHDC();
    hdc[1] = GetImageHDC(images);
    hdc[2] = GetImageHDC(images + 1);
    hdc[3] = GetImageHDC(images + 2);
    hdc[4] = CreateCompatibleDC(hdc[0]);
    CreateGrayscaleBitmap(hdc[4]);
    SetTextColor(hdc[0], BLACK);
    SetBkMode(hdc[0], TRANSPARENT);
    SetTextAlign(hdc[0], TA_CENTER);
}

void SaveGameToFile(Game &game, Player &player) {
    FILE *fp = GetFilePtr(1);
    if (!fp)
        return;
    fwprintf_s(fp, L"%s\t%d\t%d\t%d\t%d\t%d\t%d\n", game.player_name, game.time_left, game.score, game.grass_num, game.num_at_a_time, game.interval, game.last_id);
    fwprintf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\t%d\n", player.skin, player.x, player.y, player.dx, player.dy, player.speed, player.direction);
    GrassNode *p = game.h->next;
    for (int i = 0; i < game.grass_num; i++) {
        fwprintf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p->id, p->type, p->grass_style, p->score, p->x, p->y, p->picked,p->exploded);
        p = p->next;
    }
    fclose(fp);
    InfoBox(L"已保存");
}

bool LoadGameFromFile(Game &game, Player &player) {
    FILE *fp = GetFilePtr(0);
    if (!fp)
        return false;
    fwscanf_s(fp, L"%s\t%d\t%d\t%d\t%d\t%d\t%d\n", &game.player_name, sizeof game.player_name / sizeof(wchar_t), &game.time_left, &game.score, &game.grass_num, &game.num_at_a_time, &game.interval, &game.last_id);
    fwscanf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &player.skin, &player.x, &player.y, &player.dx, &player.dy, &player.speed, &player.direction);
    game.paused = true;
    game.h = new GrassNode(-1);
    GrassNode *p = game.h, *s;
    int temp_bool[2]; //C4477
    for (int i = 0; i < game.grass_num; i++) {
        s = new GrassNode(i);
        fwscanf_s(fp, L"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &p->id, &p->type, &p->grass_style, &p->score, &p->x, &p->y, &temp_bool[0], &temp_bool[1]);
        p->picked = temp_bool[0] != 0;
        p->exploded = temp_bool[1] != 0;
        GrassNode::grid[p->y][p->x] = 1;
        p->next = s;
        p = p->next;
    }
    fclose(fp);
    return true;
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