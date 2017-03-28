#include <ncurses.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>
#include <mutex>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

std::vector<std::string> urls;
std::mutex urls_mutex;

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
    while (file >> url) {
        urls.push_back(url);
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


int main(int argc, const char *const *argv) {
    if (parse_args(argc, argv)) {
        return 1;
    }

    if (manage_files()) {
        return 1;
    }

    read_urls();
    for (auto url: urls) {
        printf("%s\n", url.c_str());
    }

//    printf("args: -i |%s| -n |%d| -o |%s|\n", arg.input_file.c_str(), arg.thread_number, arg.output_dir.c_str());
//    std::cout << "|" << arg.input_file << "|" << arg.thread_number << "|" << arg.output_dir << "|\n";

//    int x = 2, n = 4;
//    std::thread t(MyThread(5), n);
//    t.join();

    return 0;
}