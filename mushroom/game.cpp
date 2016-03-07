#include "mushroom.h"

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
    on_exit = false;
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

    h = new GrassNode(-1);
    grass_num = 0;
    last_id = 0;
    ZeroMemory(GrassNode::grid, sizeof GrassNode::grid);
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

void Game::SaveScoreToLeaderboard() {
    wchar_t path[260];
    GetModuleFileName(nullptr, path, sizeof path);
    PathRemoveFileSpec(path);
    wcscat_s(path, L"\\leaderboard.txt");
    FILE *fp;
    if (_wfopen_s(&fp, path, L"at+, ccs=UTF-8") == 1) {
        ErrorBox(L"fopen failed");
        return;
    }
    time_t now = time(nullptr);
    tm tstruct;
    localtime_s(&tstruct, &now);
    wchar_t buffer[100];
    wcsftime(buffer, sizeof buffer, L"%Y/%m/%d", &tstruct);
    fwprintf_s(fp, L"%s\t%d\t%s\n", player_name, score, buffer);
    fclose(fp);
}

void Game::Timeout() {
    wchar_t buffer[50];
    swprintf_s(buffer, L"游戏结束！\n分数: %d\n是否记录分数？", score);
    switch (YesNoBox(buffer)) {
        case IDYES:
            SaveScoreToLeaderboard();
        case IDNO:
            on_exit = 1;
            ClearGrass();
            break;
    }
}

void Game::ExitGame() {
    wchar_t buffer[50];
    swprintf_s(buffer, L"分数: %d\n是否退出并记录分数？\n点击“取消”可返回。", score);
    switch (YesNoCancelBox(buffer)) {
        case IDYES:
            SaveScoreToLeaderboard();
        case IDNO:
            on_exit = 1;
            ClearGrass();
            break;
        case IDCANCEL:
            break;
    }
}