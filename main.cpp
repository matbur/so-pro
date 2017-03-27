
#include <ncurses.h>
#include <boost/program_options.hpp>
#include <iostream>

//#include <iostream>
//#include <pthread.h>



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


void parse_args(int argc, const char *const *argv) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "show this message and exit")
            ("input_file,i", po::value<std::string>(&arg.input_file), "(REQUIRED) file with urls")
            ("thread_num,n", po::value<int>(&arg.thread_number), "(REQUIRED) number of threads")
            ("output_dir,o", po::value<std::string>(&arg.output_dir), "destination directory");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("input_file") || !vm.count("thread_num")) {
        std::cout << desc;
        exit(0);
    }
}

int main(int argc, const char *const *argv) {
    parse_args(argc, argv);

    printf("args: -i |%s| -n |%d| -o |%s|\n", arg.input_file.c_str(), arg.thread_number, arg.output_dir.c_str());
    std::cout << "|" << arg.input_file << "|" << arg.thread_number << "|" << arg.output_dir << "|\n";
    return 0;
}