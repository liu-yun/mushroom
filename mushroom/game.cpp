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
    switch (YesNoBox(L"游戏结束！\n是否记录分数？")) {
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