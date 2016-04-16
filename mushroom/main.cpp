#include "mushroom.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    IMAGE images[3];
    HDC hdc[5];
    Game game;
    Player player;
    InitGraphics(images, hdc);
    GameMenu(game, player, hdc);
    return 0;
}

void InitGraphics(IMAGE *images, HDC hdc[]) {
    initgraph(kWidth, kHeight);
    SetWindowText(GetHWnd(), L"采蘑菇 刘云 15071018");
    HICON icon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_MUSHROOM)); //Fix the blurry taskbar icon.
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
    wchar_t filename[30];
    swprintf_s(filename, L"%s.mrs", game.player_name);
    wchar_t t = '\t';
    wfstream f = GetFileStream(1, filename);
    if (!f.is_open())
        return;
    f << game.player_name << t << game.time_left << t << game.score << t << game.grass_num << t << game.num_at_a_time << t << game.interval << t << game.last_id << endl;
    f << player.skin << t << player.x << t << player.y << t << player.dx << t << player.dy << t << player.speed << t << player.direction << endl;
    GrassNode *p = game.h->next;
    for (int i = 0; i < game.grass_num; i++) {
        f << p->id << t << p->type << t << p->style << t << p->score << t << p->x << t << p->y << t << p->picked << t << p->exploded << endl;
        p = p->next;
    }
    f.close();
    wchar_t buffer[40];
    swprintf_s(buffer, L"游戏已保存至 %s", filename);
    InfoBox(buffer);
}

bool LoadGameFromFile(Game &game, Player &player) {
    wfstream f = GetFileStream(0, nullptr);
    if (!f.is_open())
        return false;
    game.Reset();
    f >> game.player_name >> game.time_left >> game.score >> game.grass_num >> game.num_at_a_time >> game.interval >> game.last_id;
    f >> player.skin >> player.x >> player.y >> player.dx >> player.dy >> player.speed >> player.direction;
    game.h = new GrassNode(-1);
    GrassNode *p = game.h, *s;
    for (int i = 0; i < game.grass_num; i++) {
        s = new GrassNode(i);
        f >> p->id >> p->type >> p->style >> p->score >> p->x >> p->y >> p->picked >> p->exploded;
        GrassNode::grid[p->y][p->x] = 1;
        p->next = s;
        p = p->next;
    }
    f.close();
    return true;
}

void SleepMs(int ms) {
    static clock_t old = clock();
    old += ms;
    if (clock() > old) {
        old = clock();
        return;
    }
    while (clock() < old)
        Sleep(1);
}