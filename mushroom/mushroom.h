#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#include <Windows.h>
#include <commctrl.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <SDKDDKVer.h>

#include "lib/graphics.h"
#include "resource.h"

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
    bool visible;
    GrassNode *next;
    static int grid[3][4];

    GrassNode(int i);
};

class Game {
public:
    wchar_t player_name[10];
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

    Game();
    void InitGrass();
    void NewGrass();
    void DeleteGrass(int id);
    void ClearGrass();
    void PickMushroom();
    void UpdateTimer();
    void NewGrassTimer();
    void GameOver();
    void Timeout();
    void ExitGame();
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
};

inline int Transparent(HDC hdc, int x, int y, HDC hdcsrc, int w, int h, int xsrc, int ysrc) {
    return GdiTransparentBlt(hdc, x - w / 2, y - h / 2, w, h, hdcsrc, xsrc, ysrc, w, h, 0x0);
}
inline int Transparent(HDC hdc, int x, int y, HDC hdcsrc, const int src[]) {
    return GdiTransparentBlt(hdc, x - src[2] / 2, y - src[3] / 2, src[2], src[3], hdcsrc, src[0], src[1], src[2], src[3], 0x0);
}
inline int OutputText(HDC hdc, const int t[], wchar_t str[]) {
    return ExtTextOut(hdc, t[0], t[1], 0, 0, str, wcslen(str), 0);
}
inline int OutputText(HDC hdc,  int x, int y, wchar_t str[]) {
    return ExtTextOut(hdc, x, y, 0, 0, str, wcslen(str), 0);
}
inline int ErrorBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, L"采蘑菇", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
}
inline int InfoBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, L"采蘑菇", MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);
}
inline int YesNoBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, L"采蘑菇", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON1);
}
inline int YesNoCancelBox(const wchar_t str[]) {
    return MessageBox(GetHWnd(), str, L"采蘑菇", MB_ICONINFORMATION | MB_YESNOCANCEL | MB_DEFBUTTON1);
}
 
int CALLBACK InputDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK InstructionDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
bool OnInitInputDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
FILE *GetFilePtr(int mode);
wchar_t temp_name[10]; int temp_num[5];

void GameMain(HDC hdc[]);

void InitScene(IMAGE *images, HDC hdc[]);
void DrawGraphic(HDC hdc[], Game &game, Player &player);
void GetGameData(Game &game, Player &player);
void GetAndDispatchCommand(Game &game, Player &player);
void GetGrassFocus(Game &game, Player &player);
void SaveGameToFile(Game &game, Player &player);
void ReadGameFromFile(Game &game, Player &player);

void SleepMs(int ms);
int GetButtonFocus(int x, int y);