#include <ncurses.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include "URL_Thread.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


std::vector<URL_Thread> urls;
std::mutex urls_mutex;

std::mutex gui_mutex;

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
    int thread_number;
    std::string output_dir;
} arg;

int parse_args(int argc, const char *const *argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "show this message and exit")
            ("input_file,i", po::value<std::string>(&arg.input_file), "(REQUIRED) file with urls")
            ("thread_num,n", po::value<int>(&arg.thread_number), "(REQUIRED) number of threads")
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
    std::string url;

    urls_mutex.lock();
    auto url_id = 0;
    while (file >> url) {
        urls.push_back(URL_Thread{url_id++, url});
    }
    urls_mutex.unlock();

    return 0;
}

class MyThread {
    int _x;
public:
    MyThread() : _x(-1) {}

    MyThread(int x) : _x(x) {}

    void operator()() const { std::cout << _x; }

    void operator()(int n) { std::cout << n + _x; }
};

void open_gui() {
    initscr();
    raw();
}

void print_urls() {
    urls_mutex.lock();
    auto n = urls.size();
    urls_mutex.unlock();

    auto plural_f = n == 1 ? "" : "s";
    auto plural_t = arg.thread_number == 1 ? "" : "s";
    printw("Downloading %d file%s by %d thread%s",
           n, plural_f, arg.thread_number, plural_t);

    auto len = 0;
    for (auto url: urls) {
        len = std::max((int) url.get_url().size(), len);
    }

    for (auto url: urls) {
        const auto y = url.get_id() + 1;
        move(y, 0);
        printw(url.get_url().c_str());
        move(y, len + 1);
        addch('[');
        move(y, len + 21);
        printw("]  0.00 %%");
    }
}

void close_gui() {
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
//    for (auto _url: urls) { printf("%s\n", _url.c_str()); }
    open_gui();
    print_urls();

//    printf("args: -i |%s| -n |%d| -o |%s|\n", arg.input_file.c_str(), arg.thread_number, arg.output_dir.c_str());
//    std::cout << "|" << arg.input_file << "|" << arg.thread_number << "|" << arg.output_dir << "|\n";

//    int x = 2, n = 4;
//    std::thread t(MyThread(5), n);
//    t.join();
    close_gui();

    return 0;
}