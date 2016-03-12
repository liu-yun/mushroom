#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.lib")

#include <Windows.h>
#include <commctrl.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <SDKDDKVer.h>
#include <Shlwapi.h>

#include "lib/graphics.h"
#include "resource.h"

const int kWidth = 800;
const int kHeight = 600;
const int kBottom = 490;
const int kPlayerSize = 16;
const wchar_t kMushroom[] = L"采蘑菇";
enum Direction { LEFT, UP, RIGHT, DOWN };
enum Grass { MUSHROOM, BOMB, NOTHING };

class GrassNode {
public:
    int id;
    int type;
    int grass_style;
    int score;
    int x;
    int y;
    bool picked;
    time_t time_picked;
    GrassNode *next;
    static int grid[3][4];

    GrassNode(int i);
};

class Game {
public:
    wchar_t player_name[11];
    int time_left;
    int score;
    int init_num;
    int num_at_a_time;
    int interval;
    int grass_num;
    int last_id;
    GrassNode *h;
    GrassNode *grass_focus;
    int button_focus;
    bool button_on_click;
    bool paused;
    bool grayscale_ready;
    bool on_exit;

    Game();
    void InitGrass();
    void NewGrass();
    void DeleteGrassById(int id);
    void ClearGrass();
    void PickMushroom();
    void GameTimer();
    void GrassTimer();
    void SaveScoreToLeaderboard();
    void Timeout();
    void ExitGame();
    void HandleReturnKey();
};

//dx = 0;  dy = -1; 上
//dx = 0;  dy = 1;  下
//dx = -1; dy = 0;  左
//dx = 1;  dy = 0;  右
class Player {
public:
    int x;
    int y;
    int dx;
    int dy;
    int speed;
    int direction;

    Player();
    void Move();
    void SetDirection(Direction d);
    void Reset();
};

inline int MRBitBlt(HDC hdc, const int xy[], HDC hdcsrc, const int src[]) {
    return BitBlt(hdc, xy[0], xy[1], src[2], src[3], hdcsrc, src[0], src[1], SRCCOPY);
}
inline int MRTransparentBlt(HDC hdc, int x, int y, HDC hdcsrc, int w, int h, int xsrc, int ysrc) {
    return GdiTransparentBlt(hdc, x - w / 2, y - h / 2, w, h, hdcsrc, xsrc, ysrc, w, h, 0x0);
}
inline int MRTransparentBlt(HDC hdc, int x, int y, HDC hdcsrc, const int src[]) {
    return GdiTransparentBlt(hdc, x - src[2] / 2, y - src[3] / 2, src[2], src[3], hdcsrc, src[0], src[1], src[2], src[3], 0x0);
}
inline int MRTextOut(HDC hdc, const int t[], wchar_t str[]) {
    return ExtTextOut(hdc, t[0], t[1], 0, nullptr, str, wcslen(str), nullptr);
}
inline int MRTextOut(HDC hdc, int x, int y, wchar_t str[]) {
    return ExtTextOut(hdc, x, y, 0, nullptr, str, wcslen(str), nullptr);
}
inline int ErrorBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, kMushroom, MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
}
inline int InfoBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, kMushroom, MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);
}
inline int YesNoBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, kMushroom, MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON1);
}
inline int YesNoCancelBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, kMushroom, MB_ICONINFORMATION | MB_YESNOCANCEL | MB_DEFBUTTON1);
}

int CALLBACK InputDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK HelpDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK LeaderboardDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
bool OnInitInputDialog(HWND hWnd);
bool OnInitLeaderboardDialog(HWND hDlg, wchar_t data[50][3][11]);
void HandleSubitems(LPARAM lParam, wchar_t data[50][3][11]);
FILE *GetFilePtr(int mode);
void CreateGrayscaleBitmap(HDC hdc);
extern wchar_t temp_name[11]; extern int temp_num[5];

void InitScene(IMAGE *images, HDC hdc[]);
void SaveGameToFile(Game &game, Player &player);
bool LoadGameFromFile(Game &game, Player &player);
void SleepMs(int ms);

void GameMain(Game &game, Player &player, HDC hdc[]);
bool InitNewGame(Game &game, Player &player);
void DrawGameGraphic(HDC hdc[], Game &game, Player &player);
void GetAndDispatchCommand(Game &game, Player &player);
void GetGrassFocus(Game &game, Player &player);
int GetGameButtonFocus(int x, int y);

void GameMenu(Game &game, Player &player, HDC hdc[]);
void DrawMenuGraphic(HDC hdc[], Game &game);
void GetAndDispatchMenuCommand(Game &game, Player &player, HDC hdc[]);
int GetMenuButtonFocus(int x, int y);