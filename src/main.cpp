#include "common.hpp"
#include "Arguments.hpp"
#include "GUI.hpp"


//int fun_with_ncurses() {
//
//    initscr();
//    raw();
//    printw("Hello world");
//    addch('a');
//
//    move(10, 10);
//    attron(A_STANDOUT | A_UNDERLINE);
//    printw("Hello %d", 5);
//    attroff(A_STANDOUT | A_UNDERLINE);
//
//    mvprintw(20, 20, "d");
//    mvaddch(21, 21, 'x');
//
//    printw("Hello world");
//
//    getch();
//    endwin();
//
//    return 0;
//}


int main(int argc, const char *const *argv) {
    std::mutex urls_mutex;
    std::vector<URL> urls;

    Arguments args(argc, argv, &urls, &urls_mutex);

    if (auto ret = args.is_valid())
        return ret;

    auto gui_thread = std::thread(GUI(&urls, args.get_max_threads(), &urls_mutex));

    Semaphore sem(args.get_max_threads());

    std::vector<std::thread> threads;
    for (auto &url: urls)
        threads.push_back(std::thread(std::ref(url), &sem));

    for (auto &t: threads)
        t.join();

    gui_thread.join();

    return 0;
}