#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <ncurses.h>
#include <thread>

#include "Semaphore.h"
#include "URL_Thread.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

//std::mutex gui_mutex;
std::mutex urls_mutex;


std::vector<URL_Thread> urls;

int fun_with_ncurses() {

    initscr();
    raw();
    printw("Hello world");
    addch('a');

    move(10, 10);
    attron(A_STANDOUT | A_UNDERLINE);
    printw("Hello %d", 5);
    attroff(A_STANDOUT | A_UNDERLINE);

    mvprintw(20, 20, "d");
    mvaddch(21, 21, 'x');

    printw("Hello world");

    getch();
    endwin();

    return 0;
}

struct {
    std::string input_file;
    int max_threads;
    std::string output_dir;
} arg;

int parse_args(int argc, const char *const *argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "show this message and exit")
            ("input_file,i", po::value<std::string>(&arg.input_file), "(REQUIRED) file with urls")
            ("thread_num,n", po::value<int>(&arg.max_threads), "(REQUIRED) number of threads")
            ("output_dir,o", po::value<std::string>(&arg.output_dir)->default_value("output"), "destination directory");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("input_file") || !vm.count("thread_num")) {
        std::cout << desc;
        return 1;
    }

    return 0;
}

int manage_files() {
    if (!fs::is_regular_file(arg.input_file)) {
        printf("No such file: %s\n", arg.input_file.c_str());
        return 1;
    }

    if (!fs::is_directory(arg.output_dir)) {
        try {
            return !fs::create_directories(arg.output_dir);
        } catch (fs::filesystem_error) {
            printf("Couldn't create directory: %s\n", arg.output_dir.c_str());
            return 0;
        }
    }

    return 0;
}

int read_urls() {
    std::ifstream file(arg.input_file);
    std::string url, fname;

    std::lock_guard<std::mutex> lk(urls_mutex);
    auto url_id = 0;
    while (file >> url >> fname) {
        if (url[0] == '#')
            continue;

        auto path = arg.output_dir + "/" + fname;
        urls.push_back(URL_Thread{url_id++, url, path, &urls_mutex});
    }

    return 0;
}

void paint() {
    erase();

    for (auto url: urls) {
        move(url._id, 0);
        printw(url._path.c_str());
        addch(' ');
        printw("%d %.2f", url._pipes, url._progress);
//        for (auto i = 0; i < url._pipes; i++) {
//            addch('|');
//        }
    }

    refresh();
}

void print_urls() {
    urls_mutex.lock();
    auto n = urls.size();
    urls_mutex.unlock();

    auto plural_f = n == 1 ? "" : "s";
    auto plural_t = arg.max_threads == 1 ? "" : "s";
    printw("Downloading %d file%s by %d thread%s",
           n, plural_f, arg.max_threads, plural_t);

    auto len = 0;
    for (auto url: urls) {
        len = std::max(url._len, len);
    }

    for (auto url: urls) {
        const auto y = url._id + 1;
        move(y, 0);
        printw(url._url.c_str());
        move(y, len + 1);
        addch('[');
        move(y, len + 21);
        printw("]  0.00 %%");
    }
}

void gui_func() {
    initscr();
    raw();

    bool done = false;
    while (!done) {
        done = true;
        for (auto url: urls) {
            if (!url._done) {
                done = false;
                break;
            }
        }

        paint();
    }

    getch();
    endwin();
}



int main(int argc, const char *const *argv) {
    if (auto ret = parse_args(argc, argv)) {
        return ret;
    }

    if (auto ret = manage_files()) {
        return ret;
    }

    read_urls();
//    for (auto url:urls) {
//        std::cout << url._path << ' ' << &url << '\n';
//    }
//    print_urls();

//    printf("args: -i |%s| -n |%d| -o |%s|\n", arg.input_file.c_str(), arg.max_threads, arg.output_dir.c_str());
//    std::cout << "|" << arg.input_file << "|" << arg.max_threads << "|" << arg.output_dir << "|\n";

    std::thread gui_thread(gui_func);

    Semaphore sem(arg.max_threads);

    std::vector<std::thread> threads;
    for (auto &url: urls) {
        threads.push_back(std::thread(std::ref(url), &sem));
    }

    for (auto &t: threads) {
        t.join();
    }

    gui_thread.join();

    return 0;
}