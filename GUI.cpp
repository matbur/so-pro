#include "GUI.hpp"

GUI::GUI(std::vector<URL> *urls, int max_threads, std::mutex *mtx)
        : _urls(urls), _max_threads(max_threads) {

    std::lock_guard<std::mutex> lg(*mtx);
    _number = _urls->size();
    auto max_len = 0;
    for (auto url: *_urls) {
        max_len = std::max(max_len, url.get_len());
    }
    _max_len = max_len;
}

void GUI::operator()() {
    initscr();
    raw();

    bool done = false;
    while (!done) {
        done = true;
        for (auto url: *_urls) {
            if (!url.is_done()) {
                done = false;
                break;
            }
        }

        _paint();
    }

    getch();
    endwin();
}

void GUI::_paint() {
    erase();

    move(1, 1);
    auto plural_f = _number == 1 ? "" : "s";
    auto plural_t = _max_threads == 1 ? "" : "s";
    auto max_threads = _max_threads == 1 << 16 ? "all" : std::to_string(_max_threads).c_str();
    printw("matbur's web scraper (%d file%s, %s thread%s)", _number, plural_f, max_threads, plural_t);

    for (auto url: *_urls) {
        auto y = url.get_id() + 3;
        move(y, 1);
        printw("%4d) %s ", url.get_id(), url.get_path().c_str());
        move(y, _max_len + 10);
        printw("%8d / %8d [", url.get_now(), url.get_total());
        for (auto i = 0; i < url.get_progress() / 5; i++) {
            addch('#');
        }
        move(y, _max_len + 51);
        printw("] %3d %%", url.get_progress());
    }

    move(0, 0);
    refresh();
}
