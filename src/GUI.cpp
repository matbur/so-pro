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
    auto begin = std::chrono::system_clock::now();
    initscr();
    raw();

    _height = getmaxy(stdscr);

    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

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

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - begin;
    move(_height - 1, 1);
    printw("All done in %.3fs. Press any key.", duration.count());

    getch();
    endwin();
}

void GUI::_paint() {
    erase();

    move(1, 1);
    auto plural_f = _number == 1 ? "" : "s";
    auto plural_t = _max_threads == 1 ? "" : "s";
    auto max_threads = _max_threads == 1 << 16 ? "all" : std::to_string(_max_threads).c_str();
    auto num_done = 0;
    for (auto url: *_urls)
        num_done += url.is_done();

    printw("matbur's web scraper (%d file%s, %s thread%s, %d done)",
           _number, plural_f, max_threads, plural_t, num_done);

    for (auto url: *_urls) {
        auto y = url.get_id() + 3;
        auto progress = url.get_progress();

        move(y, 1);
        printw("%4d) %s ", url.get_id(), url.get_path().c_str());
        move(y, _max_len + 10);
        printw("%8d / %8d [", url.get_now(), url.get_total());

        for (auto i = 0; i < progress / 5; i++) {
            addch('#');
        }
        move(y, _max_len + 51);
        addch(']');
        if (progress == 100) {
            attron(COLOR_PAIR(2));
            printw(" %3d%%", progress);
            attroff(COLOR_PAIR(2));
        } else {
            printw(" %3d%%", progress);
        }
        attron(COLOR_PAIR(1));
        printw("    %s", url.get_error());
        attroff(COLOR_PAIR(1));
    }

    move(0, 0);
    refresh();
}
