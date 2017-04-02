#include "GUI.hpp"

GUI::GUI(std::vector<URL> *urls, int max_threads)
        : _urls(urls), _max_threads(max_threads) {}

void GUI::operator()(std::mutex *mtx) {
    initscr();
    raw();

    mtx->lock();
    auto number = _urls->size();
    auto max_len = 0;
    for (auto url: *_urls) {
        max_len = std::max(max_len, url.get_len());
    }
    mtx->unlock();

    bool done = false;
    while (!done) {
        done = true;
        for (auto url: *_urls) {
            if (!url.is_done()) {
                done = false;
                break;
            }
        }

        _paint(number, max_len);
    }

    getch();
    endwin();
}

void GUI::_paint(size_t number, int len) {
    erase();

    move(1, 1);
    auto plural_f = number == 1 ? "" : "s";
    auto plural_t = _max_threads == 1 ? "" : "s";
    auto max_threads = _max_threads == 1 << 16 ? "all" : std::to_string(_max_threads).c_str();
    printw("matbur's web scraper (%d file%s, %s thread%s)", number, plural_f, max_threads, plural_t);

    for (auto url: *_urls) {
        auto y = url.get_id() + 3;
        move(y, 1);
        printw("%4d) %s ", url.get_id(), url.get_path().c_str());
        move(y, len + 10);
        addch('[');
        for (auto i = 0; i < url.get_pipes(); i++) {
            addch('|');
        }
        move(y, len + 21);
        printw("] %.2f %%", url.get_progress());
    }

    move(0, 0);
    refresh();
}
